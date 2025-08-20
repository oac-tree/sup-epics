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
#include <sup/protocol/protocol_factory_utils.h>

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

double ParsePositiveParameter(sup::cli::CommandLineParser& parser, const std::string& option_name);

std::unique_ptr<sup::dto::AnyFunctor> GetFixedReplyFunctor(const sup::dto::AnyValue& fixed_reply,
                                                           double delay);
std::unique_ptr<sup::protocol::Protocol> GetFixedProtocolOutputFunctor(
  const sup::dto::AnyValue& fixed_reply, sup::protocol::ProtocolResult result, double delay);

std::string CreateProtocolOutputTitle(const std::string& base, sup::protocol::ProtocolResult result);
void PrintAnyvaluePacket(const std::string& title, const sup::dto::AnyValue& value);

}  // unnamed namespace

sup::dto::AnyValue GetFromJSONFile(sup::cli::CommandLineParser& parser)
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
  config.timeout = ParsePositiveParameter(parser, "--timeout");
  return config;
}

sup::protocol::ProtocolRPCClientConfig GetProtocolRPCClientConfiguration(
  sup::cli::CommandLineParser& parser)
{
  sup::protocol::ProtocolRPCClientConfig config{};
  auto encoding_str = parser.GetValue<std::string>("--encoding");
  if (encoding_str == sup::protocol::kEncoding_None)
  {
    config.m_encoding = sup::protocol::PayloadEncoding::kNone;
  }
  else if (encoding_str != sup::protocol::kEncoding_Base64)
  {
    throw std::runtime_error("Unknown encoding: " + encoding_str);
  }
  if (parser.IsSet("--polling-interval"))
  {
    config.m_polling_interval_sec = ParsePositiveParameter(parser, "--polling-interval");
    config.m_async = true;
  }
  config.m_timeout_sec = ParsePositiveParameter(parser, "--timeout");
  return config;
}

std::unique_ptr<sup::dto::AnyFunctor> GetFixedReplyFunctor(sup::cli::CommandLineParser& parser)
{
  auto fixed_reply = GetFromJSONFile(parser);
  double delay{0.0};
  if (parser.IsSet("--delay"))
  {
    delay = ParsePositiveParameter(parser, "--delay");
  }
  return GetFixedReplyFunctor(fixed_reply, delay);
}

std::unique_ptr<sup::protocol::Protocol> GetFixedOutputProtocol(
  sup::cli::CommandLineParser& parser)
{
  auto fixed_reply = GetFromJSONFile(parser);
  double delay{0.0};
  if (parser.IsSet("--delay"))
  {
    delay = ParsePositiveParameter(parser, "--delay");
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
                               sup::protocol::LogAnyFunctorDecorator::PacketDirection direction,
                               const std::string& input_title, const std::string& output_title)
{
  using PacketDirection = sup::protocol::LogAnyFunctorDecorator::PacketDirection;
  switch (direction)
  {
  case PacketDirection::kInput:
    PrintAnyvaluePacket(input_title, packet);
    break;
  case PacketDirection::kOutput:
    PrintAnyvaluePacket(output_title, packet);
    break;
  default:
    break;
  }
}

void LogInputProtocolPacketToStdOut(const sup::dto::AnyValue& packet,
                                    sup::protocol::LogProtocolDecorator::PacketType type,
                                    const std::string& normal_title,
                                    const std::string& service_title)
{
  using PacketType = sup::protocol::LogProtocolDecorator::PacketType;
  switch (type)
  {
  case PacketType::kNormal:
    PrintAnyvaluePacket(normal_title, packet);
    break;
  case PacketType::kService:
    PrintAnyvaluePacket(service_title, packet);
    break;
  default:
    break;
  }
}

void LogOutputProtocolPacketToStdOut(sup::protocol::ProtocolResult result,
                                     const sup::dto::AnyValue& packet,
                                     sup::protocol::LogProtocolDecorator::PacketType type,
                                     const std::string& normal_title,
                                     const std::string& service_title)
{
  using PacketType = sup::protocol::LogProtocolDecorator::PacketType;
  switch (type)
  {
  case PacketType::kNormal:
    PrintAnyvaluePacket(CreateProtocolOutputTitle(normal_title, result), packet);
    break;
  case PacketType::kService:
    PrintAnyvaluePacket(CreateProtocolOutputTitle(service_title, result), packet);
    break;
  default:
    break;
  }
}

namespace
{

double ParsePositiveParameter(sup::cli::CommandLineParser& parser, const std::string& option_name)
{
  auto val = parser.GetValue<double>(option_name);
  if (val < 0.0)
  {
    const std::string error = "Option " + option_name + " must be positive";
    throw std::runtime_error(error);
  }
  return val;
}

std::unique_ptr<sup::dto::AnyFunctor> GetFixedReplyFunctor(const sup::dto::AnyValue& fixed_reply, double delay)
{
  return std::make_unique<FixedReplyFunctor>(fixed_reply, delay);
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
