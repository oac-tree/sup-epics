/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Gennady Pospelov (IO)
 *
 * Copyright (c) : 2010-2025 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 *****************************************************************************/

#include "pv_access_server_impl.h"

#include "pv_access_utils.h"

#include <pvxs/client.h>
#include <pvxs/server.h>

#include <stdexcept>

namespace sup
{
namespace epics
{
PvAccessServerImpl::PvAccessServerImpl(std::unique_ptr<pvxs::server::Server>&& context,
                                       PvAccessServer::VariableChangedCallback callback)
  : m_context(std::move(context))
  , m_callback(std::move(callback))
  , m_variables{}
  , m_client_context{}
{}

PvAccessServerImpl::~PvAccessServerImpl()
{
  m_context->stop();
}

//! Adds channel with given name to the map of channels.
void PvAccessServerImpl::AddVariable(const std::string& name, const dto::AnyValue& any_value)
{
  auto iter = m_variables.find(name);
  if (iter != m_variables.end())
  {
    throw std::runtime_error("Error in PvAccessServer: existing variable name '" + name + "'.");
  }

  PvAccessServerPV::VariableChangedCallback variable_callback;
  if (m_callback)
  {
    variable_callback = [this, name](const sup::dto::AnyValue& any_value)
    { OnVariableChanged(name, any_value); };
  }

  std::unique_ptr<PvAccessServerPV> variable(
      new PvAccessServerPV(name, any_value, variable_callback));
  m_variables.emplace(name, std::move(variable));
}

std::vector<std::string> PvAccessServerImpl::GetVariableNames() const
{
  std::vector<std::string> result;
  std::transform(std::begin(m_variables), end(m_variables), back_inserter(result),
                 [](decltype(m_variables)::value_type const& pair) { return pair.first; });
  return result;
}

const std::map<std::string, std::unique_ptr<PvAccessServerPV>>&
PvAccessServerImpl::GetVariables() const
{
  return m_variables;
}

std::unique_ptr<pvxs::server::Server>& PvAccessServerImpl::GetContext()
{
  return m_context;
}

std::shared_ptr<pvxs::client::Context> PvAccessServerImpl::GetClientContext()
{
  if (!m_client_context)
  {
    m_client_context = std::make_shared<pvxs::client::Context>(m_context->clientConfig().build());
  }
  return m_client_context;
}

void PvAccessServerImpl::OnVariableChanged(const std::string& name,
                                           const sup::dto::AnyValue& any_value)
{
  if (m_callback)
  {
    m_callback(name, any_value);
  }
}

std::unique_ptr<PvAccessServerImpl> CreateIsolatedServerImpl(
  PvAccessServer::VariableChangedCallback cb)
{
  auto server = utils::CreateIsolatedServer();
  return std::unique_ptr<PvAccessServerImpl>(
    new PvAccessServerImpl(std::move(server), std::move(cb)));
}

std::unique_ptr<PvAccessServerImpl> CreateServerImplFromEnv(
  PvAccessServer::VariableChangedCallback cb)
{
  auto server = utils::CreateServerFromEnv();
  return std::unique_ptr<PvAccessServerImpl>(
    new PvAccessServerImpl(std::move(server), std::move(cb)));
}

}  // namespace epics

}  // namespace sup
