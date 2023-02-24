/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Walter Van Herck (IO)
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

#ifndef SUP_EPICS_PV_ACCESS_RPC_SERVER_H_
#define SUP_EPICS_PV_ACCESS_RPC_SERVER_H_

#include "pv_access_rpc_server_config.h"

#include <sup/epics/pv_access_rpc_client.h>

#include <sup/dto/any_functor.h>

#include <memory>

namespace sup
{
namespace epics
{
class PvAccessRPCServerImpl;

/** @brief PvAccess based implementation of an RPC server
 *
 * @details This PvAccess based implementation of an RPC server forwards requests to an AnyFunctor
 * interface (see sup-dto). It also implicitly assumes the transport protocol defined in sup-protocol:
 * although one can send (and receive) AnyValues that are not conforming to the transport protocol,
 * all error messages are implemented according to that protocol, meaning they contain a status
 * field, a timestamp and a reason field.
  */
class PvAccessRPCServer
{
public:
  struct IsolatedTag {};
  static IsolatedTag Isolated;

  PvAccessRPCServer(const PvAccessRPCServerConfig& config, sup::dto::AnyFunctor& handler);

  PvAccessRPCServer(IsolatedTag isolated, const PvAccessRPCServerConfig& config,
                    sup::dto::AnyFunctor& handler);

  ~PvAccessRPCServer();

  PvAccessRPCClient CreateClient(const PvAccessRPCClientConfig& config);

private:
  std::unique_ptr<PvAccessRPCServerImpl> m_impl;
};

PvAccessRPCServerConfig GetDefaultRPCServerConfig(const std::string& service_name);

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_PV_ACCESS_RPC_SERVER_H_
