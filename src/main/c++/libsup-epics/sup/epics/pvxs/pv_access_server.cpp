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

#include "sup/epics/pv_access_server.h"

#include <pvxs/server.h>
#include <sup/epics/pv_access_server_pv.h>

#include <map>
#include <stdexcept>
#include <sup/dto/anyvalue.h>

namespace sup
{
namespace epics
{

// ----------------------------------------------------------------------------
// PVAccessServerImpl
// ----------------------------------------------------------------------------

struct PvAccessServer::PVAccessServerImpl
{
  context_t m_context;
  callback_t m_callback;
  std::map<std::string, std::unique_ptr<PvAccessServerPV>> m_variables;

  PVAccessServerImpl(context_t context, callback_t callback)
      : m_context(std::move(context)), m_callback(std::move(callback))
  {
  }

  //! Adds channel with given name to the map of channels.
  void AddVariable(const std::string& name, const dto::AnyValue& any_value)
  {
    auto iter = m_variables.find(name);
    if (iter != m_variables.end())
    {
      throw std::runtime_error("Error in PvAccessServer: existing variable name '" + name + "'.");
    }

    PvAccessServerPV::callback_t variable_callback;
    if (m_callback)
    {
      variable_callback = [this, name](const sup::dto::AnyValue& any_value)
      { OnVariableChanged(name, any_value); };
    }

    std::unique_ptr<PvAccessServerPV> variable(
        new PvAccessServerPV(name, any_value, variable_callback));
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
// PvAccessServer
// ----------------------------------------------------------------------------

PvAccessServer::PvAccessServer(context_t context, callback_t callback)
    : p_impl(new PVAccessServerImpl(std::move(context), std::move(callback)))
{
}

PvAccessServer::~PvAccessServer()
{
  delete p_impl;
}

void PvAccessServer::AddVariable(const std::string& name, const dto::AnyValue& any_value)
{
  p_impl->AddVariable(name, any_value);
}

std::vector<std::string> PvAccessServer::GetVariableNames() const
{
  return p_impl->GetVariableNames();
}

dto::AnyValue PvAccessServer::GetValue(const std::string &name) const
{
  auto iter = p_impl->m_variables.find(name);
  if (iter == p_impl->m_variables.end())
  {
    throw std::runtime_error("Error in PvAccessServer: non-existing variable name '" + name + "'.");
  }
  return iter->second->GetValue();
}

bool PvAccessServer::SetValue(const std::string &name, const dto::AnyValue &value)
{
  auto iter = p_impl->m_variables.find(name);
  if (iter == p_impl->m_variables.end())
  {
    throw std::runtime_error("Error in PvAccessServer: non-existing variable name '" + name + "'.");
  }
  return iter->second->SetValue(value);
}

void PvAccessServer::Start()
{
  for(const auto& entry : p_impl->m_variables)
  {
    entry.second->AddToServer(*p_impl->m_context);
  }

  // starting PVXS server
  p_impl->m_context->start();
}

}  // namespace epics

}  // namespace sup
