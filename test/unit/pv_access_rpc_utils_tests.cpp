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

#include <sup/epics/pv_access_rpc_client.h>
#include <sup/epics/pv_access_rpc_server.h>
#include <sup/epics-test/unit_test_helper.h>

#include <sup/dto/anyvalue.h>
#include <sup/protocol/protocol_rpc.h>

#include <gtest/gtest.h>

using namespace sup::epics;

class PvAccessRPCUtilsTests : public ::testing::Test
{
protected:
  PvAccessRPCUtilsTests();
  ~PvAccessRPCUtilsTests();
};

//! Standard scenario. Single server and single client.

TEST_F(PvAccessRPCUtilsTests, ClientRPCCall)
{
  std::string channel_name = "PvAccessRPCUtilsTests_1";
  sup::dto::AnyValue fixed_reply = {{
    {"result", { sup::dto::UnsignedInteger32Type, 42 } }
  }};
  test::FixedReplyFunctor handler{ fixed_reply };
  PvAccessRPCServer server(PvAccessRPCServer::Isolated, GetDefaultRPCServerConfig(channel_name),
                           handler);
  auto client = server.CreateClient(GetDefaultRPCClientConfig(channel_name));

  // Array of structures payload fails
  // building any value
  sup::dto::AnyValue struct_value1 = {{{"field_name", {sup::dto::SignedInteger32Type, 42}}},
                                      "struct_name"};
  sup::dto::AnyValue struct_value2 = {{{"field_name", {sup::dto::SignedInteger32Type, 43}}},
                                      "struct_name"};
  auto payload = sup::dto::ArrayValue({struct_value1, struct_value2});
  auto reply = client(payload);
  EXPECT_TRUE(sup::protocol::utils::CheckReplyFormat(reply));
  EXPECT_EQ(reply[sup::protocol::constants::REPLY_RESULT].As<sup::dto::uint32>(), 2);
}

TEST_F(PvAccessRPCUtilsTests, HandleRPCCall)
{
  std::string channel_name = "PvAccessRPCUtilsTests_2";
  // Array of structures payload fails
  // building any value
  sup::dto::AnyValue struct_value1 = {{{"field_name", {sup::dto::SignedInteger32Type, 42}}},
                                      "struct_name"};
  sup::dto::AnyValue struct_value2 = {{{"field_name", {sup::dto::SignedInteger32Type, 43}}},
                                      "struct_name"};
  auto fixed_reply = sup::dto::ArrayValue({struct_value1, struct_value2});
  test::FixedReplyFunctor handler{ fixed_reply };
  PvAccessRPCServer server(PvAccessRPCServer::Isolated, GetDefaultRPCServerConfig(channel_name),
                           handler);
  auto client = server.CreateClient(GetDefaultRPCClientConfig(channel_name));

  sup::dto::AnyValue payload = {{
    {"result", { sup::dto::UnsignedInteger32Type, 42 } }
  }};
  auto reply = client(payload);
  EXPECT_TRUE(sup::protocol::utils::CheckReplyFormat(reply));
  EXPECT_EQ(reply[sup::protocol::constants::REPLY_RESULT].As<sup::dto::uint32>(), 3);
}

PvAccessRPCUtilsTests::PvAccessRPCUtilsTests() = default;

PvAccessRPCUtilsTests::~PvAccessRPCUtilsTests() = default;
