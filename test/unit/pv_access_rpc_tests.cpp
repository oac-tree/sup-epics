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

#include <sup/epics/pv_access_rpc_client.h>
#include <sup/epics/pv_access_rpc_server.h>
#include <sup/epics-test/unit_test_helper.h>

#include <sup/dto/anyvalue.h>
#include <sup/protocol/protocol_rpc.h>

#include <gtest/gtest.h>

#include <functional>

static const std::string RETURN_EMPTY_FIELD = "return_empty";

using namespace sup::epics;

//! Testing PvAccessRPCServer and PvAccessRPCClient together.

class PvAccessRPCTests : public ::testing::Test
{
protected:
  PvAccessRPCTests();
  ~PvAccessRPCTests();

  sup::dto::AnyFunctor& GetHandler();

  std::unique_ptr<sup::dto::AnyValue> m_request;
  std::unique_ptr<sup::dto::AnyValue> m_reply;
private:
  test::FunctionFunctor m_handler;
};

//! Standard scenario. Single server and single client.

TEST_F(PvAccessRPCTests, SingleServerSingleClientSuccess)
{
  std::string channel_name = "PvAccessRPCTests:channel";
  PvAccessRPCServer server(PvAccessRPCServer::Isolated, GetDefaultRPCServerConfig(channel_name),
                           GetHandler());
  auto client = server.CreateClient(GetDefaultRPCClientConfig(channel_name));

  // Send simple scalar payload over RPC
  sup::dto::AnyValue payload{42};
  auto request =
    sup::protocol::utils::CreateRPCRequest(payload, sup::protocol::PayloadEncoding::kNone);
  auto reply = client(request);
  EXPECT_TRUE(sup::protocol::utils::CheckReplyFormat(reply));
  ASSERT_TRUE(static_cast<bool>(m_request));
  ASSERT_TRUE(static_cast<bool>(m_reply));
  EXPECT_EQ(request, *m_request);
  EXPECT_EQ(reply, *m_reply);
}

//! Standard scenario with moved client.

TEST_F(PvAccessRPCTests, MovedClientSuccess)
{
  std::string channel_name = "PvAccessRPCTests:channel";
  PvAccessRPCServer server(PvAccessRPCServer::Isolated, GetDefaultRPCServerConfig(channel_name),
                           GetHandler());
  auto client = server.CreateClient(GetDefaultRPCClientConfig(channel_name));

  // Move ctor
  PvAccessRPCClient moved_client{std::move(client)};

  // Send simple scalar payload over RPC with moved client
  sup::dto::AnyValue payload{42};
  auto request =
    sup::protocol::utils::CreateRPCRequest(payload, sup::protocol::PayloadEncoding::kNone);
  auto reply = moved_client(request);
  EXPECT_TRUE(sup::protocol::utils::CheckReplyFormat(reply));
  ASSERT_TRUE(static_cast<bool>(m_request));
  ASSERT_TRUE(static_cast<bool>(m_reply));
  EXPECT_EQ(request, *m_request);
  EXPECT_EQ(reply, *m_reply);

  // Move assignment
  client = std::move(moved_client);

  // Send simple scalar payload over RPC with moved client
  reply = client(request);
  EXPECT_TRUE(sup::protocol::utils::CheckReplyFormat(reply));
  ASSERT_TRUE(static_cast<bool>(m_request));
  ASSERT_TRUE(static_cast<bool>(m_reply));
  EXPECT_EQ(request, *m_request);
  EXPECT_EQ(reply, *m_reply);
}

//! Fail at client side. Message should not arrive at server.

TEST_F(PvAccessRPCTests, RPCClientEmptyRequest)
{
  const std::string channel_name = "PvAccessRPCTests:channel";
  PvAccessRPCServer server(PvAccessRPCServer::Isolated, GetDefaultRPCServerConfig(channel_name),
                           GetHandler());
  auto client = server.CreateClient(GetDefaultRPCClientConfig(channel_name));

  // Send empty value
  const sup::dto::AnyValue empty{};
  auto reply = client(empty);
  EXPECT_FALSE(sup::protocol::utils::CheckReplyFormat(reply));
  EXPECT_TRUE(sup::dto::IsEmptyValue(reply));
  EXPECT_FALSE(static_cast<bool>(m_request));
  EXPECT_FALSE(static_cast<bool>(m_reply));
}

TEST_F(PvAccessRPCTests, RPCClientWrongChannel)
{
  const std::string channel_name = "PvAccessRPCTests:channel";
  PvAccessRPCServer server(PvAccessRPCServer::Isolated, GetDefaultRPCServerConfig(channel_name),
                           GetHandler());
  auto client = server.CreateClient({"DOESNOTEXIST", 0.1});

  const sup::dto::AnyValue payload{42};
  auto request =
    sup::protocol::utils::CreateRPCRequest(payload, sup::protocol::PayloadEncoding::kNone);
  auto reply = client(request);
  ASSERT_TRUE(sup::protocol::utils::CheckReplyFormat(reply));
  EXPECT_FALSE(static_cast<bool>(m_request));
  EXPECT_FALSE(static_cast<bool>(m_reply));
  EXPECT_EQ(reply[sup::protocol::constants::REPLY_RESULT].As<unsigned int>(),
            sup::protocol::NotConnected.GetValue());
}

//! Fail at server side.

TEST_F(PvAccessRPCTests, RPCEmptyReply)
{
  const std::string channel_name = "PvAccessRPCTests:channel";
  PvAccessRPCServer server(PvAccessRPCServer::Isolated, GetDefaultRPCServerConfig(channel_name),
                           GetHandler());
  auto client = server.CreateClient(GetDefaultRPCClientConfig(channel_name));

  // Send empty value
  const sup::dto::AnyValue payload{42};
  auto request =
    sup::protocol::utils::CreateRPCRequest(payload, sup::protocol::PayloadEncoding::kNone);
  // Although the extra field is not defined by the usual transport protocol (see sup-protocol), our
  // custom handler at the server side will use it to return an empty value.
  request.AddMember(RETURN_EMPTY_FIELD, true);
  auto reply = client(request);
  ASSERT_TRUE(sup::protocol::utils::CheckReplyFormat(reply));
  ASSERT_TRUE(static_cast<bool>(m_request));
  ASSERT_TRUE(static_cast<bool>(m_reply));
  EXPECT_TRUE(sup::dto::IsEmptyValue(*m_reply));
  EXPECT_EQ(reply[sup::protocol::constants::REPLY_RESULT].As<unsigned int>(),
            sup::protocol::ServerNetworkEncodingError.GetValue());
}

//! Standard scenario with non-isolated client/server (created from environment variables).

TEST_F(PvAccessRPCTests, ClientServerFromEnv)
{
  const std::string channel_name = "PvAccessRPCTests:channel";
  PvAccessRPCServer server(GetDefaultRPCServerConfig(channel_name), GetHandler());
  auto client = server.CreateClient(GetDefaultRPCClientConfig(channel_name));

  // Send simple scalar payload over RPC
  const sup::dto::AnyValue payload{42};
  auto request =
    sup::protocol::utils::CreateRPCRequest(payload, sup::protocol::PayloadEncoding::kNone);
  auto reply = client(request);
  EXPECT_TRUE(sup::protocol::utils::CheckReplyFormat(reply));
  ASSERT_TRUE(static_cast<bool>(m_request));
  ASSERT_TRUE(static_cast<bool>(m_reply));
  EXPECT_EQ(request, *m_request);
  EXPECT_EQ(reply, *m_reply);
}

TEST_F(PvAccessRPCTests, ClientDirectlyFromEnv)
{
  const std::string channel_name = "PvAccessRPCTests:channel";
  const PvAccessRPCServer server(GetDefaultRPCServerConfig(channel_name), GetHandler());
  PvAccessRPCClient client{GetDefaultRPCClientConfig(channel_name)};

  // Send simple scalar payload over RPC
  const sup::dto::AnyValue payload{42};
  auto request =
    sup::protocol::utils::CreateRPCRequest(payload, sup::protocol::PayloadEncoding::kNone);
  auto reply = client(request);
  EXPECT_TRUE(sup::protocol::utils::CheckReplyFormat(reply));
  ASSERT_TRUE(static_cast<bool>(m_request));
  ASSERT_TRUE(static_cast<bool>(m_reply));
  EXPECT_EQ(request, *m_request);
  EXPECT_EQ(reply, *m_reply);
}

TEST_F(PvAccessRPCTests, ServerDestructionAndReconstruction)
{
  const std::string channel_name = "PvAccessRPCTests:channel";
  // Run multiple times to ensure creation of RPC server with same name after destruction of
  // another one works:
  const std::size_t n_times = 10;
  for (std::size_t i=0; i<n_times; ++i)
  {
    PvAccessRPCServer server(GetDefaultRPCServerConfig(channel_name), GetHandler());
    PvAccessRPCClient client(PvAccessRPCClientConfig{channel_name, 10.0});

    // Send simple scalar payload over RPC
    const sup::dto::AnyValue payload{42};
    auto request =
      sup::protocol::utils::CreateRPCRequest(payload, sup::protocol::PayloadEncoding::kNone);
    auto reply = client(request);
    EXPECT_TRUE(sup::protocol::utils::CheckReplyFormat(reply));
    ASSERT_TRUE(static_cast<bool>(m_request));
    ASSERT_TRUE(static_cast<bool>(m_reply));
    EXPECT_EQ(request, *m_request);
    EXPECT_EQ(reply, *m_reply);
  }
}

PvAccessRPCTests::PvAccessRPCTests()
  : m_request{}
  , m_reply{}
  , m_handler{[this](const sup::dto::AnyValue& request, const sup::dto::AnyValue& reply){
      m_request = std::make_unique<sup::dto::AnyValue>(request);
      m_reply = std::make_unique<sup::dto::AnyValue>(reply);
    }}
{}

PvAccessRPCTests::~PvAccessRPCTests() = default;

sup::dto::AnyFunctor& PvAccessRPCTests::GetHandler()
{
  return m_handler;
}
