/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Gennady Pospelov (IO)
 *
 * Copyright (c) : 2010-2022 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 *****************************************************************************/

#include "sup/epics/pvxs/pv_access_server_variable.h"

#include <pvxs/data.h>
#include <pvxs/server.h>
#include <pvxs/sharedpv.h>
#include <sup/dto/anyvalue.h>
#include <sup/epics/dto_conversion_utils.h>

#include <iostream>
#include <mutex>
#include <stdexcept>

namespace sup
{
namespace epics
{

// ----------------------------------------------------------------------------
// PVAccessServerVariableImpl
// ----------------------------------------------------------------------------

struct PVAccessServerVariable::PVAccessServerVariableImpl
{
  const std::string m_variable_name;
  sup::dto::AnyValue m_any_value;  //!< The main value of this variable.
  pvxs::Value m_pvxs_cache;        //!< Necessary for open/post operations of SharedPV
  callback_t m_callback;
  pvxs::server::SharedPV m_shared_pv;
  std::mutex m_mutex;

  PVAccessServerVariableImpl(const std::string& variable_name, const sup::dto::AnyValue& any_value,
                             callback_t callback)
      : m_variable_name(variable_name)
      , m_any_value(any_value)
      , m_pvxs_cache(BuildScalarAwarePVXSValue(m_any_value))
      , m_callback(std::move(callback))
      , m_shared_pv(pvxs::server::SharedPV::buildMailbox())
  {
    using namespace std::placeholders;
    m_shared_pv.onPut(
        std::bind(&PVAccessServerVariableImpl::OnSharedValueChanged, this, _1, _2, _3));
  }

  //! Get AnyValue stored in cache.
  sup::dto::AnyValue GetAnyValue()
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_any_value;
  }

  //! Sets the cache variable and schedules update of the shared variable.
  bool SetAnyValue(const sup::dto::AnyValue& any_value)
  {
    std::lock_guard<std::mutex> lock(m_mutex);

    m_any_value = any_value;

    // assigning value to shared variable
    if (m_shared_pv.isOpen())
    {
      // Simple copy doesn't work. We have to keep m_pvxs_cache internal storage's pointer
      // alive since server::SharedPV relies on that.
      auto pvxs_value = BuildScalarAwarePVXSValue(m_any_value);
      m_pvxs_cache.assign(pvxs_value);
      m_shared_pv.post(m_pvxs_cache);
    }

    if (m_callback) // for some reason `post` above doesn't trigger OnSharedValueChanged
    {
      m_callback(m_any_value);
    }

    return true;
  }

  //! Add variable to given server. Initialise shared variable with cache value and make
  //! it open for exernal connections.
  void AddToServer(pvxs::server::Server& server)
  {
    if (m_shared_pv.isOpen())
    {
      throw std::runtime_error("Variable was already added to a server");
    }

    server.addPV(m_variable_name, m_shared_pv);
    m_shared_pv.open(m_pvxs_cache);
  }

  void OnSharedValueChanged(pvxs::server::SharedPV& /*pv*/,
                            std::unique_ptr<pvxs::server::ExecOp>&& op, pvxs::Value&& value)
  {
    std::lock_guard<std::mutex> lock(m_mutex);

    m_any_value = BuildScalarAwareAnyValue(value);

    // Simple copy doesn't work. We have to keep m_pvxs_cache internal storage's pointer alive
    // since server::SharedPV relies on that.
    m_pvxs_cache.assign(value);
    m_shared_pv.post(m_pvxs_cache);

    if (m_callback)
    {
      m_callback(m_any_value);
    }
    op->reply();
  }
};

// ----------------------------------------------------------------------------
// PVAccessServerVariable
// ----------------------------------------------------------------------------

PVAccessServerVariable::PVAccessServerVariable(const std::string& variable_name,
                                               const sup::dto::AnyValue& any_value,
                                               callback_t callback)
    : p_impl(new PVAccessServerVariableImpl(variable_name, any_value, std::move(callback)))
{
}

std::string PVAccessServerVariable::GetVariableName() const
{
  return p_impl->m_variable_name;
}

dto::AnyValue PVAccessServerVariable::GetValue() const
{
  return p_impl->GetAnyValue();
}

bool PVAccessServerVariable::SetValue(const dto::AnyValue& value)
{
  return p_impl->SetAnyValue(value);
}

void PVAccessServerVariable::AddToServer(pvxs::server::Server& server)
{
  p_impl->AddToServer(server);
}

PVAccessServerVariable::~PVAccessServerVariable()
{
  delete p_impl;
}

}  // namespace epics

}  // namespace sup
