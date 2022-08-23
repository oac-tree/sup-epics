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

#include <mutex>
#include <stdexcept>

namespace sup
{
namespace epics
{

struct PVAccessServerVariable::PVAccessServerVariableImpl
{
  const std::string m_variable_name;
  callback_t m_callback;
  sup::dto::AnyType m_original_type; //!< Type used during the construction.
  pvxs::Value m_pvxs_value;  //!< value cache
  std::mutex m_mutex;

  explicit PVAccessServerVariableImpl(const std::string& variable_name,
                                      const sup::dto::AnyValue& any_value, callback_t callback)
      : m_variable_name(variable_name), m_callback(std::move(callback))
  {
    m_original_type = any_value.GetType();
  }

  pvxs::Value GetPVXSValue(const sup::dto::AnyValue& any_value)
  {
//    if (sup::dto::IsScalarValue(any_value))
//    {

//    }
  }


  //! Sets the cache variable and schedules update of the shared variable.
  bool SetCache(const sup::dto::AnyValue& any_value)
  {
    if (any_value.GetType() != m_original_type)
    {
      throw std::runtime_error(
          "Error in  PVAccessServerVariable::SetValue(): attempt to change type");
    }

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
  return {};
}

bool PVAccessServerVariable::SetValue(const dto::AnyValue& value)
{
  return false;
}

PVAccessServerVariable::~PVAccessServerVariable()
{
  delete p_impl;
}

}  // namespace epics

}  // namespace sup
