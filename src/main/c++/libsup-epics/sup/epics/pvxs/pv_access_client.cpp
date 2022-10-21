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
#include <sup/dto/anyvalue.h>
#include <sup/epics/pv_access_client_pv_old.h>

#include <algorithm>
#include <map>
#include <stdexcept>

namespace sup
{
namespace epics
{

// ----------------------------------------------------------------------------
// PVAccessClientImpl
// ----------------------------------------------------------------------------

struct PVAccessClient::PVAccessClientImpl
{
  context_t m_context;
  callback_t m_callback;
  std::map<std::string, std::unique_ptr<PvAccessClientPV_old>> m_variables;

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

    PvAccessClientPV_old::callback_t variable_callback;
    if (m_callback)
    {
      variable_callback = [this, name](const sup::dto::AnyValue& any_value)
      { OnVariableChanged(name, any_value); };
    }

    std::unique_ptr<PvAccessClientPV_old> variable(
        new PvAccessClientPV_old(name, m_context, variable_callback));
    m_variables.emplace(name, std::move(variable));
  }

  std::vector<std::string> GetVariableNames() const
  {
    std::vector<std::string> result;
    std::transform(std::begin(m_variables), end(m_variables), back_inserter(result),
                   [](decltype(m_variables)::value_type const& pair) { return pair.first; });
    return result;
  }

  void OnVariableChanged(const std::string& name, const sup::dto::AnyValue& any_value)
  {
    if (m_callback)
    {
      m_callback(name, any_value);
    }
  }
};

// ----------------------------------------------------------------------------
// PVAccessClient
// ----------------------------------------------------------------------------

PVAccessClient::PVAccessClient(context_t context, callback_t callback)
    : p_impl(new PVAccessClientImpl(std::move(context), std::move(callback)))
{
}

PVAccessClient::~PVAccessClient()
{
  delete p_impl;
}

void PVAccessClient::AddVariable(const std::string& name)
{
  p_impl->AddVariable(name);
}

std::vector<std::string> PVAccessClient::GetVariableNames() const
{
  return p_impl->GetVariableNames();
}

bool PVAccessClient::IsConnected(const std::string& name) const
{
  auto iter = p_impl->m_variables.find(name);
  return iter == p_impl->m_variables.end() ? false : iter->second->IsConnected();
}

dto::AnyValue PVAccessClient::GetValue(const std::string& name) const
{
  auto iter = p_impl->m_variables.find(name);
  if (iter == p_impl->m_variables.end())
  {
    throw std::runtime_error("Error in PVAccessClient: non-existing variable name '" + name + "'.");
  }
  return iter->second->GetValue();
}

bool PVAccessClient::SetValue(const std::string& name, const dto::AnyValue& value)
{
  auto iter = p_impl->m_variables.find(name);
  if (iter == p_impl->m_variables.end())
  {
    throw std::runtime_error("Error in PVAccessClient: non-existing variable name '" + name + "'.");
  }
  return iter->second->SetValue(value);
}

}  // namespace epics

}  // namespace sup
