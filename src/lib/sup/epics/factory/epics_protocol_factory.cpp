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

#include "epics_protocol_factory_utils.h"

#include <sup/epics/epics_protocol_factory.h>
#include <sup/epics/pv_access_rpc_server.h>

#include <sup/protocol/exceptions.h>
#include <sup/protocol/protocol_factory_utils.h>

namespace sup
{
namespace epics
{
EPICSProtocolFactory::EPICSProtocolFactory() = default;

EPICSProtocolFactory::~EPICSProtocolFactory() = default;

std::unique_ptr<sup::protocol::ProcessVariable> EPICSProtocolFactory::CreateProcessVariable(
  const sup::dto::AnyValue& var_definition) const
{
  return nullptr;
}

std::unique_ptr<sup::protocol::RPCServerInterface> EPICSProtocolFactory::CreateRPCServer(
  sup::protocol::Protocol& protocol,
  const sup::dto::AnyValue& server_definition) const
{
  auto server_config = utils::ParsePvAccessRPCServerConfig(server_definition);
  auto factory_funct = [server_config](sup::dto::AnyFunctor& functor){
    return std::unique_ptr<sup::protocol::RPCServerInterface>(
      new PvAccessRPCServer(server_config, functor));
  };
  return sup::protocol::CreateRPCServerStack(factory_funct, protocol);
}

std::unique_ptr<sup::protocol::Protocol> EPICSProtocolFactory::CreateRPCClient(
  const sup::dto::AnyValue& client_definition) const
{
  auto client_config = utils::ParsePvAccessRPCClientConfig(client_definition);
  auto factory_funct = [client_config](){
    return std::unique_ptr<sup::dto::AnyFunctor>(
      new PvAccessRPCClient(client_config));
  };
  auto encoding = sup::protocol::ParsePayloadEncoding(client_definition);
  return sup::protocol::CreateRPCClientStack(factory_funct, encoding);
}

}  // namespace epics

}  // namespace sup
