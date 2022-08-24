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
  const sup::dto::AnyType m_original_type;  //!< Type used during the construction.
  callback_t m_callback;
  pvxs::Value m_pvxs_value;  //!< value cache
  std::mutex m_mutex;

  PVAccessServerVariableImpl(const std::string& variable_name, const sup::dto::AnyValue& any_value,
                             callback_t callback)
      : m_variable_name(variable_name)
      , m_original_type(any_value.GetType())
      , m_callback(std::move(callback))
  {
    SetCache(any_value);
  }

  //! Converts AnyValue to PVXS value. If AnyValue is scalar, turn it into the structure.
  static pvxs::Value GetPVXSValue(const sup::dto::AnyValue& any_value)
  {
    auto struct_any_value =
        sup::dto::IsScalarValue(any_value) ? ConvertScalarToStruct(any_value) : any_value;
    return BuildPVXSValue(struct_any_value);
  }

  //! Converts PVXS value to AnyValue. If original type was a scalar, turn PVXS struct to a scalar.
  sup::dto::AnyValue GetAnyValue(const pvxs::Value& pvxs_value)
  {
    auto result = BuildAnyValue(pvxs_value);
    return sup::dto::IsScalarType(m_original_type) ? ConvertStructToScalar(result) : result;
  }

  //! Sets the cache variable and schedules update of the shared variable.
  bool SetCache(const sup::dto::AnyValue& any_value)
  {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (any_value.GetType() != m_original_type)
    {
      throw std::runtime_error(
          "Error in  PVAccessServerVariable::SetValue(): attempt to change type");
    }

    m_pvxs_value = GetPVXSValue(any_value);

    // FIXME implement shared_pv update

    return true;
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
  return p_impl->GetAnyValue(p_impl->m_pvxs_value);
}

bool PVAccessServerVariable::SetValue(const dto::AnyValue& value)
{
  return p_impl->SetCache(value);
}

PVAccessServerVariable::~PVAccessServerVariable()
{
  delete p_impl;
}

}  // namespace epics

}  // namespace sup
