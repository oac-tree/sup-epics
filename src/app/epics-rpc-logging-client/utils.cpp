/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : Supervision and Automation - Configuration
 *
 * Description   : Configuration and CVVF libraries for the Supervision and Automation System.
 *
 * Author        : Walter Van Herck (IO)
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
 ******************************************************************************/

#include "utils.h"

#include <sup/epics/pv_access_rpc_client.h>

#include <sup/dto/anyvalue_helper.h>
#include <sup/dto/json_value_parser.h>

#include <iostream>
#include <stdexcept>

namespace sup
{
namespace epics
{
namespace utils
{
namespace
{
const std::string kInputPacketTitle = "Client sent network packet";
const std::string kOutputPacketTitle = "Client received network packet";

void PrintAnyvaluePacket(const std::string& title, const sup::dto::AnyValue& value);
}  // unnamed namespace

sup::dto::AnyValue GetRequest(sup::cli::CommandLineParser& parser)
{
  auto filename = parser.GetValue<std::string>("--file");
  sup::dto::JSONAnyValueParser av_parser{};
  if (!av_parser.ParseFile(filename))
  {
    throw std::runtime_error("Failed to parse JSON file: " + filename);
  }
  return av_parser.MoveAnyValue();
}

PvAccessRPCClientConfig GetRPCClientConfiguration(sup::cli::CommandLineParser& parser)
{
  auto service_name = parser.GetValue<std::string>("--service");
  auto config = GetDefaultRPCClientConfig(service_name);
  if (parser.IsSet("--timeout"))
  {
    config.timeout = parser.GetValue<double>("--timeout");
  }
  return config;
}

void LogNetworkPacketsToStdOut(const sup::dto::AnyValue& packet,
                               sup::protocol::LogAnyFunctorDecorator::PacketDirection direction)
{
  using PacketDirection = sup::protocol::LogAnyFunctorDecorator::PacketDirection;
  switch (direction)
  {
  case PacketDirection::kInput:
    PrintAnyvaluePacket(kInputPacketTitle, packet);
    break;
  case PacketDirection::kOutput:
    PrintAnyvaluePacket(kOutputPacketTitle, packet);
    break;
  default:
    break;
  }
}

namespace
{
void PrintAnyvaluePacket(const std::string& title, const sup::dto::AnyValue& value)
{
  std::cout << title << std::endl;
  std::cout << std::string(title.size(), '-') << std::endl;
  std::cout << sup::dto::PrintAnyValue(value) << std::endl;
  std::cout << std::endl;
}

} // unnamed namespace

}  // namespace utils

}  // namespace epics

}  // namespace sup
