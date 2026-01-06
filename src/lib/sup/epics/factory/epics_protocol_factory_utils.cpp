/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Gennady Pospelov (IO)
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

#include "epics_protocol_factory_utils.h"

#include <sup/epics/epics_protocol_factory.h>
#include <sup/epics/pv_access_rpc_server.h>

#include <sup/dto/anyvalue.h>
#include <sup/dto/json_type_parser.h>
#include <sup/protocol/exceptions.h>
#include <sup/protocol/protocol_factory_utils.h>

namespace sup
{
namespace epics
{
namespace utils
{
PvAccessRPCServerConfig ParsePvAccessRPCServerConfig(const sup::dto::AnyValue& config)
{
  sup::protocol::ValidateConfigurationField(config, kServiceName, sup::dto::StringType);
  auto service_name = config[kServiceName].As<std::string>();
  return GetDefaultRPCServerConfig(service_name);
}

PvAccessRPCClientConfig ParsePvAccessRPCClientConfig(const sup::dto::AnyValue& config)
{
  sup::protocol::ValidateConfigurationField(config, kServiceName, sup::dto::StringType);
  auto service_name = config[kServiceName].As<std::string>();
  if (!config.HasField(kTimeout))
  {
    return GetDefaultRPCClientConfig(service_name);
  }
  sup::protocol::ValidateConfigurationField(config, kTimeout, sup::dto::Float64Type);
  double timeout = config[kTimeout].As<double>();
  if (timeout < 0.0)
  {
    const std::string error = "Cannot not use negative timeout for PvAccessRPCClient";
    throw sup::protocol::InvalidOperationException(error);
  }
  return PvAccessRPCClientConfig{service_name, timeout};
}

std::unique_ptr<sup::protocol::ProcessVariable> CreateChannelAccessClientVar(
  const sup::dto::AnyValue& config)
{
  sup::protocol::ValidateConfigurationField(config, kChannelName, sup::dto::StringType);
  sup::protocol::ValidateConfigurationField(config, kVariableType, sup::dto::StringType);
  auto channel_name = config[kChannelName].As<std::string>();
  auto json_type = config[kVariableType].As<std::string>();
  sup::dto::JSONAnyTypeParser parser;
  if (!parser.ParseString(json_type))
  {
    const std::string error = "Cannot parse type for ChannelAccessClient ProcessVariable";
    throw sup::protocol::InvalidOperationException(error);
  }
  return CreateCAClientProcessVariable(channel_name, parser.MoveAnyType());
}

std::unique_ptr<sup::protocol::ProcessVariable> CreatePvAccessClientVar(
  const sup::dto::AnyValue& config)
{
  sup::protocol::ValidateConfigurationField(config, kChannelName, sup::dto::StringType);
  auto channel_name = config[kChannelName].As<std::string>();
  return CreatePVAClientProcessVariable(channel_name);
}

std::unique_ptr<sup::protocol::ProcessVariable> CreatePvAccessServerVar(
  const sup::dto::AnyValue& config)
{
  sup::protocol::ValidateConfigurationField(config, kChannelName, sup::dto::StringType);
  auto channel_name = config[kChannelName].As<std::string>();
  if (!config.HasField(kVariableValue))
  {
    const std::string error = "Cannot find value for PvAccessServer ProcessVariable";
    throw sup::protocol::InvalidOperationException(error);
  }
  return CreatePVAServerProcessVariable(channel_name, config[kVariableValue]);
}

LoggingEPICSRPCClient::LoggingEPICSRPCClient(
  const PvAccessRPCClientConfig& config,
  sup::protocol::LogAnyFunctorDecorator::LogFunction log_function)
  : m_epics_client{std::make_unique<PvAccessRPCClient>(config)}
  , m_log_decorator{*m_epics_client, log_function}
{}

sup::dto::AnyValue LoggingEPICSRPCClient::operator()(const sup::dto::AnyValue& input)
{
  return m_log_decorator(input);
}

}  // namespace utils

}  // namespace epics

}  // namespace sup
