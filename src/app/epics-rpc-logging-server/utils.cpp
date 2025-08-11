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

#include <sup/dto/anyvalue_helper.h>
#include <sup/dto/json_value_parser.h>

#include <sup/protocol/protocol.h>

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

sup::dto::AnyValue GetFixedReply(sup::cli::CommandLineParser& parser);
std::unique_ptr<sup::dto::AnyFunctor> GetFixedReplyFunctor(const sup::dto::AnyValue& fixed_reply, double delay);
void PrintAnyvaluePacket(const std::string& title, const sup::dto::AnyValue& value);

}  // unnamed namespace

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
