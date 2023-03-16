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
 * Copyright (c) : 2010-2021 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 *****************************************************************************/

#include <sup/epics/pv_access_rpc_client.h>
#include <sup/epics/pv_access_rpc_server.h>

#include <sup/di/object_manager.h>

using namespace sup::epics;

// Register PvAccessRPCClient:

std::unique_ptr<PvAccessRPCClientConfig> PvAccessRPCClientConfigDefaultFactoryFunction(
  const std::string& service_name)
{
  return std::unique_ptr<PvAccessRPCClientConfig>{
    new PvAccessRPCClientConfig{GetDefaultRPCClientConfig(service_name)}};
}

const bool PvAccessRPCClientConfigDefault_Registered =
  sup::di::GlobalObjectManager().RegisterFactoryFunction("PvAccessRPCClientConfigDefault",
    PvAccessRPCClientConfigDefaultFactoryFunction);

const bool PvAccessRPCClient_Registered =
  sup::di::GlobalObjectManager().RegisterFactoryFunction("PvAccessRPCClient",
    sup::di::ForwardingInstanceFactoryFunction<sup::dto::AnyFunctor, PvAccessRPCClient,
                                               const PvAccessRPCClientConfig&>);
