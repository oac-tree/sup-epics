/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2025 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 * SPDX-License-Identifier: MIT
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file LICENSE located in the top level directory
 * of the distribution package.
 *****************************************************************************/

#ifndef SUP_EPICS_PV_ACCESS_RPC_SERVER_IMPL_H_
#define SUP_EPICS_PV_ACCESS_RPC_SERVER_IMPL_H_

#include <sup/epics/pv_access_rpc_server_config.h>

#include <sup/dto/any_functor.h>

#include <pvxs/client.h>
#include <pvxs/server.h>

#include <memory>

namespace sup
{
namespace epics
{
class PvAccessRPCServerImpl
{
public:
  PvAccessRPCServerImpl(std::unique_ptr<pvxs::server::Server>&& server,
                        const PvAccessRPCServerConfig& config, sup::dto::AnyFunctor& handler);

  PvAccessRPCServerImpl(const PvAccessRPCServerImpl&) = delete;
  PvAccessRPCServerImpl(PvAccessRPCServerImpl&&) = delete;
  PvAccessRPCServerImpl& operator=(const PvAccessRPCServerImpl&) = delete;
  PvAccessRPCServerImpl& operator=(PvAccessRPCServerImpl&&) = delete;

  ~PvAccessRPCServerImpl();

  std::shared_ptr<pvxs::client::Context> GetClientContext();

private:
  void Initialise();
  std::unique_ptr<pvxs::server::Server> m_server;
  PvAccessRPCServerConfig m_config;
  sup::dto::AnyFunctor& m_handler;
  std::shared_ptr<pvxs::client::Context> m_client_context;
};

std::unique_ptr<PvAccessRPCServerImpl> CreateIsolatedRPCServerImpl(
  const PvAccessRPCServerConfig& config, sup::dto::AnyFunctor& handler);

std::unique_ptr<PvAccessRPCServerImpl> CreateRPCServerImplFromEnv(
  const PvAccessRPCServerConfig& config, sup::dto::AnyFunctor& handler);

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_PV_ACCESS_RPC_SERVER_IMPL_H_
