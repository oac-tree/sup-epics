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

#include <gtest/gtest.h>

#include <sup/epics/epics_protocol_factory.h>
#include <sup/epics/pv_access_rpc_client.h>

#include <sup/protocol/log_any_functor_decorator.h>

using namespace sup::epics;

class TestFunctor : public sup::dto::AnyFunctor
{
public:
  explicit TestFunctor(sup::dto::AnyValue fixed_reply)
      : m_fixed_reply(std::move(fixed_reply))
  {}

  sup::dto::AnyValue operator()(const sup::dto::AnyValue&) override
  {
    return m_fixed_reply;
  }
private:
  sup::dto::AnyValue m_fixed_reply;
};

class PvAccessLoggingClientServerTests : public ::testing::Test
{
protected:

  PvAccessLoggingClientServerTests() = default;
  ~PvAccessLoggingClientServerTests() override = default;

  std::vector<sup::dto::AnyValue> m_client_packages_sent;
  std::vector<sup::dto::AnyValue> m_client_packages_received;
  std::vector<sup::dto::AnyValue> m_server_packages_sent;
  std::vector<sup::dto::AnyValue> m_server_packages_received;
};

TEST_F(PvAccessLoggingClientServerTests, PacketLogging)
{
  using PacketDirection = sup::protocol::LogAnyFunctorDecorator::PacketDirection;
  auto client_log_function = [this](const sup::dto::AnyValue& packet, PacketDirection direction)
  {
    if (direction == PacketDirection::kInput)
    {
      m_client_packages_sent.push_back(packet);
    }
    else
    {
      m_client_packages_received.push_back(packet);
    }
  };
  auto server_log_function = [this](const sup::dto::AnyValue& packet, PacketDirection direction)
  {
    if (direction == PacketDirection::kInput)
    {
      m_server_packages_received.push_back(packet);
    }
    else
    {
      m_server_packages_sent.push_back(packet);
    }
  };
  sup::dto::AnyValue request = {{
    { "setpoint", { sup::dto::Float64Type, 3.14 }},
    { "enabled", false }
  }};
  sup::dto::AnyValue reply = {{
    { "counter", { sup::dto::UnsignedInteger16Type, 42u }},
    { "message", "ok" }
  }};
  TestFunctor fixed_reply_functor(reply);
  std::string server_name = "LoggingClientServerTest::Server";
  sup::epics::PvAccessRPCServerConfig server_config{server_name};
  auto client_config = sup::epics::GetDefaultRPCClientConfig(server_name);
  auto server = CreateLoggingEPICSRPCServer(server_config, fixed_reply_functor, server_log_function);
  auto client = CreateLoggingEPICSRPCClient(client_config, client_log_function);
  client->operator()(request);
  EXPECT_EQ(m_client_packages_sent.size(), 1);
  EXPECT_EQ(m_client_packages_received.size(), 1);
  EXPECT_EQ(m_server_packages_received.size(), 1);
  EXPECT_EQ(m_server_packages_sent.size(), 1);
  EXPECT_EQ(m_client_packages_sent[0], request);
  EXPECT_EQ(m_client_packages_received[0], reply);
  EXPECT_EQ(m_server_packages_received[0], request);
  EXPECT_EQ(m_server_packages_sent[0], reply);
}

