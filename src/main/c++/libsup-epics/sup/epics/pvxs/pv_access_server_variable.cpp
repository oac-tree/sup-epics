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

#include <mutex>
#include <stdexcept>

namespace sup
{
namespace epics
{

struct PVAccessServerVariable::PVAccessServerVariableImpl
{
  const std::string m_variable_name;
  sup::dto::AnyValue m_cache;
  callback_t m_callback;
  pvxs::server::SharedPV m_shared_pv;
  std::mutex m_mutex;

  PVAccessServerVariableImpl(const std::string& variable_name, const sup::dto::AnyValue& any_value,
                             callback_t callback)
      : m_variable_name(variable_name)
      , m_cache(any_value)
      , m_callback(std::move(callback))
      , m_shared_pv(pvxs::server::SharedPV::buildMailbox())
  {
    using namespace std::placeholders;
    m_shared_pv.onPut(std::bind(&PVAccessServerVariableImpl::OnPutCallback, this, _1, _2, _3));
  }

  //! Get PVXS value from cache.
  pvxs::Value GetPVXSValue()
  {
    // if AnyValue is scalar, turn it into the structure.
    auto struct_any_value =
        sup::dto::IsScalarValue(m_cache) ? ConvertScalarToStruct(m_cache) : m_cache;
    return BuildPVXSValue(struct_any_value);
  }

  //! Get AnyValue stored in cache.
  sup::dto::AnyValue GetAnyValue()
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_cache;
  }

  //! Sets the cache variable and schedules update of the shared variable.
  bool SetCache(const sup::dto::AnyValue& any_value)
  {
    std::lock_guard<std::mutex> lock(m_mutex);

    m_cache = any_value;

    if (m_shared_pv.isOpen())
    {
      auto pvxs_value = GetPVXSValue();
      m_shared_pv.post(pvxs_value);
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
    m_shared_pv.open(GetPVXSValue());
  }

  void OnPutCallback(pvxs::server::SharedPV& /*pv*/, std::unique_ptr<pvxs::server::ExecOp>&& op,
                     pvxs::Value&& value)
  {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto any_value = BuildAnyValue(value);
    // converting back if necessary a struct with a single scalar field into a scalar
    m_cache = sup::dto::IsScalarValue(m_cache) ? ConvertStructToScalar(any_value) : any_value;
    m_shared_pv.post(value);
    if (m_callback)
    {
      m_callback(m_cache);
    }
    op->reply();
  }
};

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
  return p_impl->SetCache(value);
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
