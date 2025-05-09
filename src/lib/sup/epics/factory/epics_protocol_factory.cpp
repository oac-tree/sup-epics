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
 * SPDX-License-Identifier: MIT
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file LICENSE located in the top level directory
 * of the distribution package.
 *****************************************************************************/

#include "epics_protocol_factory_utils.h"

#include "channel_access_pv_wrapper.h"
#include "pv_access_client_pv_wrapper.h"
#include "pv_access_server_pv_wrapper.h"

#include <sup/epics/epics_protocol_factory.h>
#include <sup/epics/pv_access_rpc_server.h>

#include <sup/protocol/exceptions.h>
#include <sup/protocol/protocol_factory_utils.h>

#include <map>

namespace sup
{
namespace epics
{
EPICSProtocolFactory::EPICSProtocolFactory() = default;

EPICSProtocolFactory::~EPICSProtocolFactory() = default;

std::unique_ptr<sup::protocol::ProcessVariable> EPICSProtocolFactory::CreateProcessVariable(
  const sup::dto::AnyValue& var_definition) const
{
  using PVFactoryFunction =
    std::function<std::unique_ptr<sup::protocol::ProcessVariable>(const sup::dto::AnyValue&)>;
  static const std::map<std::string, PVFactoryFunction> factory_map = {
    { kChannelAccessClientClass, &utils::CreateChannelAccessClientVar },
    { kPvAccessClientClass, &utils::CreatePvAccessClientVar },
    { kPvAccessServerClass, &utils::CreatePvAccessServerVar }
  };
  sup::protocol::ValidateConfigurationField(var_definition, kProcessVariableClass,
                                            sup::dto::StringType);
  auto pv_class = var_definition[kProcessVariableClass].As<std::string>();
  auto iter = factory_map.find(pv_class);
  if (iter == factory_map.end())
  {
    const std::string error = "Cannot create ProcessVariable: unknown class";
    throw sup::protocol::InvalidOperationException(error);
  }
  return iter->second(var_definition);
}

std::unique_ptr<sup::protocol::RPCServerInterface> EPICSProtocolFactory::CreateRPCServer(
  sup::protocol::Protocol& protocol,
  const sup::dto::AnyValue& server_definition) const
{
  auto server_config = utils::ParsePvAccessRPCServerConfig(server_definition);
  return CreateEPICSRPCServerStack(protocol, server_config);
}

std::unique_ptr<sup::protocol::Protocol> EPICSProtocolFactory::CreateRPCClient(
  const sup::dto::AnyValue& client_definition) const
{
  auto client_config = utils::ParsePvAccessRPCClientConfig(client_definition);
  auto encoding = sup::protocol::ParsePayloadEncoding(client_definition);
  return CreateEPICSRPCClientStack(client_config, encoding);
}

std::unique_ptr<sup::protocol::ProcessVariable> CreateCAClientProcessVariable(
  const std::string& channel, const sup::dto::AnyType& var_type)
{
  return std::make_unique<ChannelAccessPVWrapper>(channel, var_type);
}

std::unique_ptr<sup::protocol::ProcessVariable> CreatePVAClientProcessVariable(
  const std::string& channel)
{
  return std::make_unique<PVAccessClientPVWrapper>(channel);
}

std::unique_ptr<sup::protocol::ProcessVariable> CreatePVAServerProcessVariable(
  const std::string& channel, const sup::dto::AnyValue& value)
{
  return std::make_unique<PVAccessServerPVWrapper>(channel, value);
}

std::unique_ptr<sup::protocol::RPCServerInterface> CreateEPICSRPCServerStack(
  sup::protocol::Protocol& protocol, const PvAccessRPCServerConfig& server_config)
{
  auto factory_funct = [server_config](sup::dto::AnyFunctor& functor){
    return std::make_unique<PvAccessRPCServer>(server_config, functor);
  };
  return sup::protocol::CreateRPCServerStack(factory_funct, protocol);
}

std::unique_ptr<sup::protocol::Protocol> CreateEPICSRPCClientStack(
  const PvAccessRPCClientConfig& client_config, sup::protocol::PayloadEncoding encoding)
{
  auto factory_funct = [client_config](){
    return std::make_unique<PvAccessRPCClient>(client_config);
  };
  return sup::protocol::CreateRPCClientStack(factory_funct, encoding);
}

}  // namespace epics

}  // namespace sup
