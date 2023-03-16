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

#include <sup/epics-di/registered_names.h>

#include <sup/di/error_codes.h>
#include <sup/di/object_manager.h>
#include <sup/dto/anyvalue.h>
#include <sup/dto/any_functor.h>

#include <gtest/gtest.h>

using namespace sup::epics;

class SupEpicsDiTests : public ::testing::Test
{
protected:
  SupEpicsDiTests();
  ~SupEpicsDiTests();
};

class TestFunctor : public sup::dto::AnyFunctor
{
public:
  TestFunctor() = default;
  ~TestFunctor() = default;
  sup::dto::AnyValue operator()(const sup::dto::AnyValue& input) override { return input; }
};

// Register service name, timeout and test functor
const bool ServiceName_Registered = sup::di::GlobalObjectManager().RegisterInstance(
  std::unique_ptr<std::string>{new std::string{"SUP-EPICS-DI:RPC-TEST-SERVICE"}}, "service_name");
const bool Timeout_Registered = sup::di::GlobalObjectManager().RegisterInstance(
  std::unique_ptr<double>{new double(10.0)}, "timeout");
const bool TestFunctor_Registered = sup::di::GlobalObjectManager().RegisterInstance(
  std::unique_ptr<sup::dto::AnyFunctor>{new TestFunctor{}}, "test_functor");

TEST_F(SupEpicsDiTests, PvAccessRPCClient)
{
  // Create PvAccessRPCClientConfig with default timeout
  auto error_code = sup::di::GlobalObjectManager().CreateInstance(
    PV_ACCESS_RPC_CLIENT_CONFIG_DEFAULT, "client_config_1", {"service_name"});
  EXPECT_EQ(error_code, sup::di::ErrorCode::kSuccess);

  // Create PvAccessRPCClientConfig with provided timeout
  error_code = sup::di::GlobalObjectManager().CreateInstance(
    PV_ACCESS_RPC_CLIENT_CONFIG, "client_config_2", {"service_name", "timeout"});
  EXPECT_EQ(error_code, sup::di::ErrorCode::kSuccess);

  // Create PvAccessRPCClient
  error_code = sup::di::GlobalObjectManager().CreateInstance(
    PV_ACCESS_RPC_CLIENT, "rpc_client", {"client_config_1"});
  EXPECT_EQ(error_code, sup::di::ErrorCode::kSuccess);
}

TEST_F(SupEpicsDiTests, PvAccessRPCServer)
{
  // Create PvAccessRPCServerConfig
  auto error_code = sup::di::GlobalObjectManager().CreateInstance(
    PV_ACCESS_RPC_SERVER_CONFIG, "server_config", {"service_name"});
  EXPECT_EQ(error_code, sup::di::ErrorCode::kSuccess);

  // Create PvAccessRPCServer
  error_code = sup::di::GlobalObjectManager().CreateInstance(
    PV_ACCESS_RPC_SERVER, "rpc_server", {"server_config", "test_functor"});
  EXPECT_EQ(error_code, sup::di::ErrorCode::kSuccess);
}

SupEpicsDiTests::SupEpicsDiTests() =default;

SupEpicsDiTests::~SupEpicsDiTests() = default;
