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

#include "app_utils.h"

#include <sup/epics/pv_access_rpc_client.h>

#include <sup/dto/anyvalue_helper.h>
#include <sup/dto/json_value_parser.h>

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <thread>

namespace sup
{
namespace epics
{
namespace utils
{
namespace
{
const std::string kInputPacketTitle = "Server received network packet";
const std::string kOutputPacketTitle = "Server replied with network packet";

const std::string kProtocolInputNormalTitle = "Server received standard protocol packet";
const std::string kProtocolInputServiceTitle = "Server received service protocol packet";

const std::string kProtocolOutputNormalTitle = "Server replied with standard protocol packet";
const std::string kProtocolOutputServiceTitle = "Server replied with service protocol packet";

class FixedReplyFunctor : public sup::dto::AnyFunctor
{
public:
  FixedReplyFunctor(sup::dto::AnyValue fixed_reply, double delay)
      : m_fixed_reply(std::move(fixed_reply))
      , m_delay{delay}
  {}

  sup::dto::AnyValue operator()(const sup::dto::AnyValue&) override
  {
    auto delay = std::chrono::duration<double>(m_delay);
    std::this_thread::sleep_for(delay);
    return m_fixed_reply;
  }
private:
  sup::dto::AnyValue m_fixed_reply;
  double m_delay;
};

class FixedOutputProtocol : public sup::protocol::Protocol
{
public:
  FixedOutputProtocol(sup::dto::AnyValue reply, sup::protocol::ProtocolResult result,
                      double delay)
      : m_output(std::move(reply)), m_result(result), m_delay{delay}
  {}

  sup::protocol::ProtocolResult Invoke(
    const sup::dto::AnyValue& input, sup::dto::AnyValue& output) override
  {
    (void)input;
    auto delay = std::chrono::duration<double>(m_delay);
    std::this_thread::sleep_for(delay);
    if (!sup::dto::TryAssign(output, m_output))
    {
      return sup::protocol::ServerProtocolEncodingError;
    }
    return m_result;
  }

  sup::protocol::ProtocolResult Service(
    const sup::dto::AnyValue& input, sup::dto::AnyValue& output) override
  {
    (void)input;
    auto delay = std::chrono::duration<double>(m_delay);
    std::this_thread::sleep_for(delay);
    if (!sup::dto::TryAssign(output, m_output))
    {
      return sup::protocol::ServerProtocolEncodingError;
    }
    return m_result;
  }

private:
  sup::dto::AnyValue m_output;
  sup::protocol::ProtocolResult m_result;
  double m_delay;
};

sup::dto::AnyValue GetFixedReply(sup::cli::CommandLineParser& parser);
std::unique_ptr<sup::dto::AnyFunctor> GetFixedReplyFunctor(const sup::dto::AnyValue& fixed_reply, double delay);
sup::dto::AnyValue GetFixedOutput(sup::cli::CommandLineParser& parser);
std::unique_ptr<sup::protocol::Protocol> GetFixedProtocolOutputFunctor(
  const sup::dto::AnyValue& fixed_reply, sup::protocol::ProtocolResult result, double delay);
std::string CreateProtocolOutputTitle(const std::string& base, sup::protocol::ProtocolResult result);
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

std::unique_ptr<sup::dto::AnyFunctor> GetFixedReplyFunctor(sup::cli::CommandLineParser& parser)
{
  auto fixed_reply = GetFixedReply(parser);
  double delay{0.0};
  if (parser.IsSet("--delay"))
  {
    auto parsed_delay = parser.GetValue<double>("--delay");
    if (parsed_delay > 0.0)
    {
      delay = parsed_delay;
    }
  }
  return GetFixedReplyFunctor(fixed_reply, delay);
}

std::unique_ptr<sup::protocol::Protocol> GetFixedOutputProtocol(
  sup::cli::CommandLineParser& parser)
{
  auto fixed_reply = GetFixedOutput(parser);
  double delay{0.0};
  if (parser.IsSet("--delay"))
  {
    auto parsed_delay = parser.GetValue<double>("--delay");
    if (parsed_delay > 0.0)
    {
      delay = parsed_delay;
    }
  }
  sup::protocol::ProtocolResult result{sup::protocol::Success};
  if (parser.IsSet("--result"))
  {
    auto parser_result = parser.GetValue<unsigned int>("--result");
    result = sup::protocol::ProtocolResult{parser_result};
  }
  return GetFixedProtocolOutputFunctor(fixed_reply, result, delay);
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

void LogInputProtocolPacketToStdOut(const sup::dto::AnyValue& packet,
                                    sup::protocol::LogProtocolDecorator::PacketType type)
{
  using PacketType = sup::protocol::LogProtocolDecorator::PacketType;
  switch (type)
  {
  case PacketType::kNormal:
    PrintAnyvaluePacket(kProtocolInputNormalTitle, packet);
    break;
  case PacketType::kService:
    PrintAnyvaluePacket(kProtocolInputServiceTitle, packet);
    break;
  default:
    break;
  }
}

void LogOutputProtocolPacketToStdOut(sup::protocol::ProtocolResult result,
                                     const sup::dto::AnyValue& packet,
                                     sup::protocol::LogProtocolDecorator::PacketType type)
{
  using PacketType = sup::protocol::LogProtocolDecorator::PacketType;
  switch (type)
  {
  case PacketType::kNormal:
    PrintAnyvaluePacket(CreateProtocolOutputTitle(kProtocolOutputNormalTitle, result), packet);
    break;
  case PacketType::kService:
    PrintAnyvaluePacket(CreateProtocolOutputTitle(kProtocolOutputServiceTitle, result), packet);
    break;
  default:
    break;
  }
}

namespace
{
sup::dto::AnyValue GetFixedReply(sup::cli::CommandLineParser& parser)
{
  auto filename = parser.GetValue<std::string>("--file");
  sup::dto::JSONAnyValueParser av_parser{};
  if (!av_parser.ParseFile(filename))
  {
    throw std::runtime_error("Failed to parse JSON file: " + filename);
  }
  return av_parser.MoveAnyValue();
}

std::unique_ptr<sup::dto::AnyFunctor> GetFixedReplyFunctor(const sup::dto::AnyValue& fixed_reply, double delay)
{
  return std::make_unique<FixedReplyFunctor>(fixed_reply, delay);
}

sup::dto::AnyValue GetFixedOutput(sup::cli::CommandLineParser& parser)
{
  auto filename = parser.GetValue<std::string>("--file");
  sup::dto::JSONAnyValueParser av_parser{};
  if (!av_parser.ParseFile(filename))
  {
    throw std::runtime_error("Failed to parse JSON file: " + filename);
  }
  return av_parser.MoveAnyValue();
}

std::unique_ptr<sup::protocol::Protocol> GetFixedProtocolOutputFunctor(
  const sup::dto::AnyValue& fixed_reply, sup::protocol::ProtocolResult result, double delay)
{
  return std::make_unique<FixedOutputProtocol>(fixed_reply, result, delay);
}

std::string CreateProtocolOutputTitle(const std::string& base, sup::protocol::ProtocolResult result)
{
  return base + " with result: " + sup::protocol::ProtocolResultToString(result);
}

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
