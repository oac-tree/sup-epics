/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : SUP EPICS
 *
 * Description   : SUP EPICS framework
 *
 * Author        : Walter Van Herck
 *
 * Copyright (c) : 2010-2026 ITER Organization,
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

#include "registered_names.h"

#include <sup/epics/pv_access_rpc_client.h>
#include <sup/epics/pv_access_rpc_server.h>

#include <sup/di/object_manager.h>

using namespace sup::epics;

// Register PvAccessRPCClient:

std::unique_ptr<PvAccessRPCClientConfig> PvAccessRPCClientConfigDefaultFactoryFunction(
  const std::string& service_name)
{
  return std::make_unique<PvAccessRPCClientConfig>(GetDefaultRPCClientConfig(service_name));
}

std::unique_ptr<PvAccessRPCClientConfig> PvAccessRPCClientConfigFactoryFunction(
  const std::string& service_name, double timeout)
{
  return std::make_unique<PvAccessRPCClientConfig>(PvAccessRPCClientConfig{service_name, timeout});
}

const bool PvAccessRPCClientConfigDefault_Registered =
  sup::di::GlobalObjectManager().RegisterFactoryFunction(PV_ACCESS_RPC_CLIENT_CONFIG_DEFAULT,
    PvAccessRPCClientConfigDefaultFactoryFunction);

const bool PvAccessRPCClientConfig_Registered =
  sup::di::GlobalObjectManager().RegisterFactoryFunction(PV_ACCESS_RPC_CLIENT_CONFIG,
    PvAccessRPCClientConfigFactoryFunction);

const bool PvAccessRPCClient_Registered =
  sup::di::GlobalObjectManager().RegisterFactoryFunction(PV_ACCESS_RPC_CLIENT,
    sup::di::ForwardingInstanceFactoryFunction<sup::dto::AnyFunctor, PvAccessRPCClient,
                                               const PvAccessRPCClientConfig&>);

// Register PvAccessRPCServer:

std::unique_ptr<PvAccessRPCServerConfig> PvAccessRPCServerConfigFactoryFunction(
  const std::string& service_name)
{
  return std::make_unique<PvAccessRPCServerConfig>(GetDefaultRPCServerConfig(service_name));
}

const bool PvAccessRPCServerConfig_Registered =
  sup::di::GlobalObjectManager().RegisterFactoryFunction(PV_ACCESS_RPC_SERVER_CONFIG,
    PvAccessRPCServerConfigFactoryFunction);

const bool PvAccessRPCServer_Registered =
  sup::di::GlobalObjectManager().RegisterFactoryFunction(PV_ACCESS_RPC_SERVER,
    sup::di::ForwardingInstanceFactoryFunction<PvAccessRPCServer, PvAccessRPCServer,
                                               const PvAccessRPCServerConfig&,
                                               sup::dto::AnyFunctor&>);
