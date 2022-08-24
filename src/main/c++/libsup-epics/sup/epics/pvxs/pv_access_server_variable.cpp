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

struct PVAccessServerVariable::PVAccessServerVariableImpl
{
  const std::string m_variable_name;
  sup::dto::AnyValue m_cache;
  const sup::dto::AnyType m_original_type;  //!< Type used during the construction.
  callback_t m_callback;
//  pvxs::Value m_pvxs_value;  //!< value cache
  pvxs::server::SharedPV m_shared_pv;
  std::mutex m_mutex;

  PVAccessServerVariableImpl(const std::string& variable_name, const sup::dto::AnyValue& any_value,
                             callback_t callback)
      : m_variable_name(variable_name)
      , m_cache(any_value)
      , m_original_type(any_value.GetType())
      , m_callback(std::move(callback))
      , m_shared_pv(pvxs::server::SharedPV::buildMailbox())
  {
//    m_pvxs_value = GetPVXSValue(any_value);

    using namespace std::placeholders;
    m_shared_pv.onPut(std::bind(&PVAccessServerVariableImpl::OnPutCallback, this, _1, _2, _3));
  }

  //! Converts AnyValue to PVXS value. If AnyValue is scalar, turn it into the structure.
  pvxs::Value GetPVXSValue()
  {
    auto struct_any_value =
        sup::dto::IsScalarValue(m_cache) ? ConvertScalarToStruct(m_cache) : m_cache;
    return BuildPVXSValue(struct_any_value);
  }

  //! Converts PVXS value to AnyValue. If original type was a scalar, turn PVXS struct to a scalar.
  sup::dto::AnyValue GetAnyValue()
  {
    return m_cache;
//    std::lock_guard<std::mutex> lock(m_mutex);

//    auto result = BuildAnyValue(m_pvxs_value);
//    return sup::dto::IsScalarType(m_original_type) ? ConvertStructToScalar(result) : result;
  }

  //! Sets the cache variable and schedules update of the shared variable.
  bool SetCache(const sup::dto::AnyValue& any_value)
  {
    std::lock_guard<std::mutex> lock(m_mutex);

//    if (any_value.GetType() != m_original_type)
//    {
//      throw std::runtime_error(
//          "Error in  PVAccessServerVariable::SetValue(): attempt to change type");
//    }
    m_cache = any_value;

    if (m_shared_pv.isOpen())
    {
      std::cout << "xxxx 1.1" << "\n";
      auto pvxs_value = GetPVXSValue();
      m_shared_pv.post(pvxs_value);
      std::cout << "xxxx 1.2" << "\n";
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

  void OnPutCallback(pvxs::server::SharedPV& pv, std::unique_ptr<pvxs::server::ExecOp>&& op,
                     pvxs::Value&& value)
  {
    std::lock_guard<std::mutex> lock(m_mutex);

    std::cout << "OnPutCallback `" << op->name() << "` \n";
    auto any_value = BuildAnyValue(value);
    m_cache = sup::dto::IsScalarValue(m_cache) ? ConvertStructToScalar(any_value) : any_value;

//    m_pvxs_value = value;
    m_shared_pv.post(value);
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
