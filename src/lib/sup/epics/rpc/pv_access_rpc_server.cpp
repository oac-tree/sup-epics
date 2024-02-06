/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2024 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 *****************************************************************************/

#include <sup/epics/pv_access_rpc_server.h>

#include <sup/epics/rpc/pv_access_rpc_client_impl.h>
#include <sup/epics/rpc/pv_access_rpc_server_impl.h>

namespace sup
{
namespace epics
{

PvAccessRPCServer::IsolatedTag PvAccessRPCServer::Isolated{};

PvAccessRPCServer::PvAccessRPCServer(const PvAccessRPCServerConfig& config,
                                     sup::dto::AnyFunctor& handler)
  : m_impl{CreateRPCServerImplFromEnv(config, handler)}
{}

PvAccessRPCServer::PvAccessRPCServer(IsolatedTag, const PvAccessRPCServerConfig& config,
                                     sup::dto::AnyFunctor& handler)
  : m_impl{CreateIsolatedRPCServerImpl(config, handler)}
{}

PvAccessRPCServer::~PvAccessRPCServer() = default;

PvAccessRPCClient PvAccessRPCServer::CreateClient(const PvAccessRPCClientConfig& config)
{
  auto context = m_impl->GetClientContext();
  std::unique_ptr<PvAccessRPCClientImpl> client_impl{new PvAccessRPCClientImpl(config, context)};
  return PvAccessRPCClient{std::move(client_impl)};
}

PvAccessRPCServerConfig GetDefaultRPCServerConfig(const std::string& service_name)
{
  return { service_name };
}

}  // namespace epics

}  // namespace sup
