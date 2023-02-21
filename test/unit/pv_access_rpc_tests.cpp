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

#include <sup/epics/pv_access_rpc_client.h>
#include <sup/epics/pv_access_rpc_server.h>

#include <sup/dto/anyvalue.h>
#include <sup/rpc/protocol_rpc.h>

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

  std::unique_ptr<sup::dto::AnyFunctor> CreateHandler();

  std::unique_ptr<sup::dto::AnyValue> m_request;
  std::unique_ptr<sup::dto::AnyValue> m_reply;
};

class TestHandler : public sup::dto::AnyFunctor
{
public:
  TestHandler(std::function<void(const sup::dto::AnyValue&,const sup::dto::AnyValue&)> func);
  ~TestHandler();

  sup::dto::AnyValue operator()(const sup::dto::AnyValue& request) override;
private:
  std::function<void(const sup::dto::AnyValue&,const sup::dto::AnyValue&)> m_func;
};

//! Standard scenario. Single server and single client.

TEST_F(PvAccessRPCTests, SingleServerSingleClientSuccess)
{
  std::string channel_name = "PvAccessRPCTests:channel";
  PvAccessRPCServer server(PvAccessRPCServer::Isolated, GetDefaultRPCServerConfig(channel_name),
                           CreateHandler());
  auto client = server.CreateClient(GetDefaultRPCClientConfig(channel_name));

  // Send simple scalar payload over RPC
  sup::dto::AnyValue payload{42};
  auto request = sup::rpc::utils::CreateRPCRequest(payload);
  auto reply = client(request);
  EXPECT_TRUE(sup::rpc::utils::CheckReplyFormat(reply));
  ASSERT_TRUE(static_cast<bool>(m_request));
  ASSERT_TRUE(static_cast<bool>(m_reply));
  EXPECT_EQ(request, *m_request);
  EXPECT_EQ(reply, *m_reply);
}

//! Fail at client side. Message should not arrive at server.

TEST_F(PvAccessRPCTests, RPCClientEmptyRequest)
{
  std::string channel_name = "PvAccessRPCTests:channel";
  PvAccessRPCServer server(PvAccessRPCServer::Isolated, GetDefaultRPCServerConfig(channel_name),
                           CreateHandler());
  auto client = server.CreateClient(GetDefaultRPCClientConfig(channel_name));

  // Send empty value
  sup::dto::AnyValue empty{};
  auto reply = client(empty);
  EXPECT_FALSE(sup::rpc::utils::CheckReplyFormat(reply));
  EXPECT_TRUE(sup::dto::IsEmptyValue(reply));
  EXPECT_FALSE(static_cast<bool>(m_request));
  EXPECT_FALSE(static_cast<bool>(m_reply));
}

TEST_F(PvAccessRPCTests, RPCClientWrongChannel)
{
  std::string channel_name = "PvAccessRPCTests:channel";
  PvAccessRPCServer server(PvAccessRPCServer::Isolated, GetDefaultRPCServerConfig(channel_name),
                           CreateHandler());
  auto client = server.CreateClient({"DOESNOTEXIST", 0.1});

  sup::dto::AnyValue payload{42};
  auto request = sup::rpc::utils::CreateRPCRequest(payload);
  auto reply = client(request);
  ASSERT_TRUE(sup::rpc::utils::CheckReplyFormat(reply));
  EXPECT_FALSE(static_cast<bool>(m_request));
  EXPECT_FALSE(static_cast<bool>(m_reply));
  EXPECT_EQ(reply[sup::rpc::constants::REPLY_RESULT].As<unsigned int>(),
            sup::rpc::NotConnected.GetValue());
}

//! Fail at server side.

TEST_F(PvAccessRPCTests, RPCEmptyReply)
{
  std::string channel_name = "PvAccessRPCTests:channel";
  PvAccessRPCServer server(PvAccessRPCServer::Isolated, GetDefaultRPCServerConfig(channel_name),
                           CreateHandler());
  auto client = server.CreateClient(GetDefaultRPCClientConfig(channel_name));

  // Send empty value
  sup::dto::AnyValue payload{42};
  auto request = sup::rpc::utils::CreateRPCRequest(payload);
  // Although the extra field is not defined by the usual transport protocol (see sup-rpc), our
  // custom handler at the server side will use it to return an empty value.
  request.AddMember(RETURN_EMPTY_FIELD, true);
  auto reply = client(request);
  ASSERT_TRUE(sup::rpc::utils::CheckReplyFormat(reply));
  ASSERT_TRUE(static_cast<bool>(m_request));
  ASSERT_TRUE(static_cast<bool>(m_reply));
  EXPECT_TRUE(sup::dto::IsEmptyValue(*m_reply));
  EXPECT_EQ(reply[sup::rpc::constants::REPLY_RESULT].As<unsigned int>(),
            sup::rpc::ServerNetworkEncodingError.GetValue());
}

//! Standard scenario with non-isolated client/server (created from environment variables).

TEST_F(PvAccessRPCTests, ClientServerFromEnv)
{
  std::string channel_name = "PvAccessRPCTests:channel";
  PvAccessRPCServer server(GetDefaultRPCServerConfig(channel_name), CreateHandler());
  auto client = server.CreateClient(GetDefaultRPCClientConfig(channel_name));

  // Send simple scalar payload over RPC
  sup::dto::AnyValue payload{42};
  auto request = sup::rpc::utils::CreateRPCRequest(payload);
  auto reply = client(request);
  EXPECT_TRUE(sup::rpc::utils::CheckReplyFormat(reply));
  ASSERT_TRUE(static_cast<bool>(m_request));
  ASSERT_TRUE(static_cast<bool>(m_reply));
  EXPECT_EQ(request, *m_request);
  EXPECT_EQ(reply, *m_reply);
}

TEST_F(PvAccessRPCTests, ClientDirectlyFromEnv)
{
  std::string channel_name = "PvAccessRPCTests:channel";
  PvAccessRPCServer server(GetDefaultRPCServerConfig(channel_name), CreateHandler());
  PvAccessRPCClient client{GetDefaultRPCClientConfig(channel_name)};

  // Send simple scalar payload over RPC
  sup::dto::AnyValue payload{42};
  auto request = sup::rpc::utils::CreateRPCRequest(payload);
  auto reply = client(request);
  EXPECT_TRUE(sup::rpc::utils::CheckReplyFormat(reply));
  ASSERT_TRUE(static_cast<bool>(m_request));
  ASSERT_TRUE(static_cast<bool>(m_reply));
  EXPECT_EQ(request, *m_request);
  EXPECT_EQ(reply, *m_reply);
}

PvAccessRPCTests::PvAccessRPCTests()
  : m_request{}
  , m_reply{}
{}

PvAccessRPCTests::~PvAccessRPCTests() = default;

std::unique_ptr<sup::dto::AnyFunctor> PvAccessRPCTests::CreateHandler()
{
  return std::unique_ptr<sup::dto::AnyFunctor>{new TestHandler(
    [this](const sup::dto::AnyValue& request, const sup::dto::AnyValue& reply){
      m_request.reset(new sup::dto::AnyValue(request));
      m_reply.reset(new sup::dto::AnyValue(reply));
    })};
}

TestHandler::TestHandler(std::function<void(const sup::dto::AnyValue&,const sup::dto::AnyValue&)> func)
  : m_func{std::move(func)}
{}

TestHandler::~TestHandler() = default;

sup::dto::AnyValue TestHandler::operator()(const sup::dto::AnyValue& request)
{
  sup::dto::AnyValue reply;
  if (!request.HasField(RETURN_EMPTY_FIELD) || request[RETURN_EMPTY_FIELD].As<bool>() == false)
  {
    reply = sup::rpc::utils::CreateRPCReply(sup::rpc::Success);
  }
  m_func(request, reply);
  return reply;
}