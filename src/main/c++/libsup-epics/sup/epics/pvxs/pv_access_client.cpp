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

#include "sup/epics/pv_access_client.h"

#include <pvxs/client.h>
#include <sup/epics/pvxs/pv_access_client_variable.h>

#include <map>
#include <stdexcept>

namespace sup
{
namespace epics
{
struct PVAccessClient::PVAccessClientImpl
{
  context_t m_context;
  callback_t m_callback;
  std::map<std::string, std::unique_ptr<PVAccessClientVariable>> m_variables;

  explicit PVAccessClientImpl(context_t context, callback_t callback)
      : m_context(std::move(context)), m_callback(std::move(callback))
  {
  }

  //! Adds channel with given name to the map of channels.
  void AddVariable(const std::string& name)
  {
    auto iter = m_variables.find(name);
    if (iter != m_variables.end())
    {
      throw std::runtime_error("Error in PVAccessClient: existing variable name '" + name + "'.");
    }

    PVAccessClientVariable::callback_t variable_callback;
    if (m_callback)
    {
      variable_callback = [this, name](const sup::dto::AnyValue& any_value)
      { OnVariableChanged(name, any_value); };
    }

    std::unique_ptr<PVAccessClientVariable> variable(
        new PVAccessClientVariable(name, m_context, variable_callback));
    m_variables.emplace(name, std::move(variable));
  }

  void OnVariableChanged(const std::string& name, const sup::dto::AnyValue& any_value)
  {
    if (m_callback)
    {
      m_callback(name, any_value);
    }
  }
};

PVAccessClient::PVAccessClient(context_t context, callback_t callback)
    : p_impl(new PVAccessClientImpl(std::move(context), std::move(callback)))
{
}

void PVAccessClient::AddVariable(const std::string& name)
{
  p_impl->AddVariable(name);
}

std::vector<std::string> PVAccessClient::GetVariableNames() const
{
  std::vector<std::string> result;
  for (const auto& entry : p_impl->m_variables)
  {
    result.push_back(entry.first);
  }

  return result;
}

bool PVAccessClient::IsConnected(const std::string& name) const
{
  auto iter = p_impl->m_variables.find(name);
  return iter == p_impl->m_variables.end() ? false : iter->second->IsConnected();
}

PVAccessClient::~PVAccessClient()
{
  delete p_impl;
}

}  // namespace epics

}  // namespace sup
