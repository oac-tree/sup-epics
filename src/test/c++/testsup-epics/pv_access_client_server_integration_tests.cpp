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

#include "mock_utils.h"

#include <gtest/gtest.h>
#include <pvxs/client.h>
#include <pvxs/server.h>
#include <sup/dto/anyvalue.h>
#include <sup/epics/pv_access_client.h>
#include <sup/epics/pv_access_server.h>
#include <sup/epics/pv_access_context_utils.h>

#include <thread>

using msec = std::chrono::milliseconds;
using ::testing::_;

using namespace sup::epics;

//! Testing PVAccessServer and PVAccessClient together.

class PVAccessClientServerIntegrationTests : public ::testing::Test
{
public:
  using client_context_t = std::shared_ptr<pvxs::client::Context>;

  client_context_t CreateClientContext(const pvxs::server::Server& server)
  {
    return std::make_shared<pvxs::client::Context>(server.clientConfig().build());
  }
};

//! Standard scenario. Server with single variable and single client.
//! Start server, check client, change the value via the server and the client.

TEST_F(PVAccessClientServerIntegrationTests, ServerWithSingleVariableAndSingleClient)
{
  const std::string channel_name("channel0");

  // creating PVXS server and corresponding client context
  auto pvxs_server = CreateIsolatedServer();
  auto client_context = CreateClientContext(*pvxs_server);

  // creating server with single variable
  PVAccessServer server(std::move(pvxs_server));
  sup::dto::AnyValue any_value{sup::dto::SignedInteger32Type, 42};
  server.AddVariable(channel_name, any_value);

  server.Start();
  std::this_thread::sleep_for(msec(20));

  // Checking variable on server side
  EXPECT_EQ(server.GetValue(channel_name), any_value);

  // creating a client with single variable
  PVAccessClient client(client_context);
  client.AddVariable(channel_name);

  // checking connection and updated values on server and client sides
  std::this_thread::sleep_for(msec(20));
  EXPECT_EQ(server.GetValue(channel_name), any_value);
  EXPECT_TRUE(client.IsConnected(channel_name));
  EXPECT_EQ(client.GetValue(channel_name), any_value);

  // changing the value via the server and checking values on server and client sides
  sup::dto::AnyValue new_any_value1{sup::dto::SignedInteger32Type, 43};
  server.SetValue(channel_name, new_any_value1);

  std::this_thread::sleep_for(msec(20));
  EXPECT_EQ(server.GetValue(channel_name), new_any_value1);
  std::this_thread::sleep_for(msec(80)); // client requires longer time
  EXPECT_EQ(client.GetValue(channel_name), new_any_value1);

  // changing the value via the client and checking values on server and client sides
  sup::dto::AnyValue new_any_value2{sup::dto::SignedInteger32Type, 44};

  client.SetValue(channel_name, new_any_value2);

  std::this_thread::sleep_for(msec(100));
  EXPECT_EQ(server.GetValue(channel_name), new_any_value2);
  EXPECT_EQ(client.GetValue(channel_name), new_any_value2);
}

//! Standard scenario. Server with single variable and single client.
//! Start server, check client, change the value via the server and the client.
//! Here we also control callback signaling.

TEST_F(PVAccessClientServerIntegrationTests, ServerWithSingleVariableAndSingleClientWithCallbacks)
{
  MockListener server_listener;
  MockListener client_listener;

  const std::string channel_name("channel0");

  // creating PVXS server and corresponding client context
  auto pvxs_server = CreateIsolatedServer();
  auto client_context = CreateClientContext(*pvxs_server);

  // creating server with single variable
  PVAccessServer server(std::move(pvxs_server), server_listener.GetNamedCallBack());
  sup::dto::AnyValue any_value{sup::dto::SignedInteger32Type, 42};
  server.AddVariable(channel_name, any_value);

  // setting up callback expectations
  EXPECT_CALL(server_listener, OnNamedValueChanged(_, _)).Times(0);
  EXPECT_CALL(client_listener, OnNamedValueChanged(channel_name, any_value)).Times(1);

  server.Start();
  std::this_thread::sleep_for(msec(20));

  // Checking variable on server side
  EXPECT_EQ(server.GetValue(channel_name), any_value);

  // creating a client with single variable
  PVAccessClient client(client_context, client_listener.GetNamedCallBack());
  client.AddVariable(channel_name);

  // checking connection and updated values on server and client sides
  std::this_thread::sleep_for(msec(20));
  EXPECT_EQ(server.GetValue(channel_name), any_value);
  EXPECT_TRUE(client.IsConnected(channel_name));
  EXPECT_EQ(client.GetValue(channel_name), any_value);

  // validating callbacks and clearing listeners
  testing::Mock::VerifyAndClearExpectations(&server_listener);
  testing::Mock::VerifyAndClearExpectations(&client_listener);

  // changing the value via the server and checking values on server and client sides
  sup::dto::AnyValue new_any_value1{sup::dto::SignedInteger32Type, 43};

  // setting up callback expectations
  EXPECT_CALL(server_listener, OnNamedValueChanged(channel_name, new_any_value1)).Times(1);
  EXPECT_CALL(client_listener, OnNamedValueChanged(channel_name, new_any_value1)).Times(1);

  server.SetValue(channel_name, new_any_value1);

  std::this_thread::sleep_for(msec(100));
  EXPECT_EQ(server.GetValue(channel_name), new_any_value1);
  EXPECT_EQ(client.GetValue(channel_name), new_any_value1);

  // validating callbacks and clearing listeners
  testing::Mock::VerifyAndClearExpectations(&server_listener);
  testing::Mock::VerifyAndClearExpectations(&client_listener);

  // changing the value via the client and checking values on server and client sides
  sup::dto::AnyValue new_any_value2{sup::dto::SignedInteger32Type, 44};

  // setting up callback expectations
  EXPECT_CALL(server_listener, OnNamedValueChanged(channel_name, new_any_value2)).Times(1);
  EXPECT_CALL(client_listener, OnNamedValueChanged(channel_name, new_any_value2)).Times(1);

  client.SetValue(channel_name, new_any_value2);

  std::this_thread::sleep_for(msec(100));
  EXPECT_EQ(server.GetValue(channel_name), new_any_value2);
  EXPECT_EQ(client.GetValue(channel_name), new_any_value2);

  // validating callbacks and clearing listeners
  testing::Mock::VerifyAndClearExpectations(&server_listener);
  testing::Mock::VerifyAndClearExpectations(&client_listener);
}
