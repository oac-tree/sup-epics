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

#include <sup/dto/anyvalue.h>
#include <sup/epics-test/unit_test_helper.h>
#include <sup/epics/pv_access_rpc_client.h>
#include <sup/epics/pv_access_rpc_server.h>
#include <sup/protocol/protocol_rpc.h>

#include <gtest/gtest.h>

using namespace sup::epics;

class PvAccessRPCUtilsTests : public ::testing::Test
{
protected:
  PvAccessRPCUtilsTests();
  ~PvAccessRPCUtilsTests() override;
};

//! Standard scenario. Single server and single client.

TEST_F(PvAccessRPCUtilsTests, ClientRPCCall)
{
  const std::string channel_name = "PvAccessRPCUtilsTests_1";
  const sup::dto::AnyValue fixed_reply = {{{"result", {sup::dto::UnsignedInteger32Type, 41}}}};
  test::FixedReplyFunctor handler{fixed_reply};
  PvAccessRPCServer server(PvAccessRPCServer::Isolated, GetDefaultRPCServerConfig(channel_name),
                           handler);
  auto client = server.CreateClient(GetDefaultRPCClientConfig(channel_name));

  const sup::dto::AnyValue struct_value1 = {{{"field_name", {sup::dto::SignedInteger32Type, 42}}},
                                            "struct_name"};
  const sup::dto::AnyValue struct_value2 = {{{"field_name", {sup::dto::SignedInteger32Type, 43}}},
                                            "struct_name"};
  auto payload = sup::dto::ArrayValue({struct_value1, struct_value2});

  // reply contains fixed value
  auto reply = client(payload);
  EXPECT_TRUE(sup::protocol::utils::CheckReplyFormat(reply));
  EXPECT_EQ(reply[sup::protocol::constants::REPLY_RESULT].As<sup::dto::uint32>(), 41);
}

TEST_F(PvAccessRPCUtilsTests, HandleRPCCall)
{
  const std::string channel_name = "PvAccessRPCUtilsTests_2";
  // for the moment the passing of names in the array structures is not supported
  const std::string deliberately_empy_struct_name = "";
  const sup::dto::AnyValue struct_value1 = {{{"field_name", {sup::dto::SignedInteger32Type, 42}}},
                                            deliberately_empy_struct_name};
  const sup::dto::AnyValue struct_value2 = {{{"field_name", {sup::dto::SignedInteger32Type, 43}}},
                                            deliberately_empy_struct_name};
  auto array = sup::dto::ArrayValue({struct_value1, struct_value2});
  const sup::dto::AnyValue fixed_reply = {
      {{sup::protocol::constants::REPLY_RESULT, {sup::dto::UnsignedInteger32Type, 41}},
       {sup::protocol::constants::REPLY_PAYLOAD, array}}};

  test::FixedReplyFunctor handler{fixed_reply};
  PvAccessRPCServer server(PvAccessRPCServer::Isolated, GetDefaultRPCServerConfig(channel_name),
                           handler);
  auto client = server.CreateClient(GetDefaultRPCClientConfig(channel_name));

  const sup::dto::AnyValue payload = {{{"result", {sup::dto::UnsignedInteger32Type, 42}}}};
  auto reply = client(payload);
  EXPECT_TRUE(sup::protocol::utils::CheckReplyFormat(reply));
  EXPECT_EQ(reply[sup::protocol::constants::REPLY_PAYLOAD], array);
}

PvAccessRPCUtilsTests::PvAccessRPCUtilsTests() = default;

PvAccessRPCUtilsTests::~PvAccessRPCUtilsTests() = default;
