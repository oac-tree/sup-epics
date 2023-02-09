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
#include <sup/epics/pvxs/pv_access_client_impl.h>

#include <thread>

#include <sup/epics-test/unit_test_helper.h>

using sup::epics::test::BusyWaitFor;
using ::testing::_;

using namespace sup::epics;

//! Testing PvAccessServer and PvAccessClient together.

class PvAccessClientServerIntegrationTests : public ::testing::Test
{
public:
};

//! Standard scenario. Server with single variable and single client.
//! Start server, check client, change the value via the server and the client.

TEST_F(PvAccessClientServerIntegrationTests, ServerWithSingleVariableAndSingleClient)
{
  const std::string channel_name("channel0");

  // creating server with single variable
  PvAccessServer server(PvAccessServer::Isolated);
  sup::dto::AnyValue any_value({{"value", {sup::dto::SignedInteger32Type, 42}}});
  server.AddVariable(channel_name, any_value);

  server.Start();

  // Checking variable on server side
  EXPECT_TRUE(BusyWaitFor(1.0, [&]() { return server.GetValue(channel_name) == any_value; }));

  // creating a client with single variable
  PvAccessClient client = server.CreateClient();
  client.AddVariable(channel_name);

  // checking connection and updated values on server and client sides
  EXPECT_TRUE(BusyWaitFor(1.0, [&]() { return server.GetValue(channel_name) == any_value; }));
  EXPECT_TRUE(client.WaitForConnected(channel_name, 1.0));
  EXPECT_TRUE(BusyWaitFor(1.0, [&]() { return client.GetValue(channel_name) == any_value; }));

  // changing the value via the server and checking values on server and client sides
  EXPECT_THROW(server.SetValue(channel_name, 43), std::runtime_error);
  any_value["value"] = 43;
  server.SetValue(channel_name, any_value);

  EXPECT_TRUE(BusyWaitFor(1.0, [&]() { return server.GetValue(channel_name) == any_value; }));
  EXPECT_TRUE(BusyWaitFor(1.0, [&]() { return client.GetValue(channel_name) == any_value; }));

  // changing the value via the client and checking values on server and client sides
  any_value["value"] = 44;

  client.SetValue(channel_name, any_value);

  EXPECT_TRUE(BusyWaitFor(1.0, [&]() { return server.GetValue(channel_name) == any_value; }));
  EXPECT_TRUE(BusyWaitFor(1.0, [&]() { return client.GetValue(channel_name) == any_value; }));
}

//! Standard scenario. Server with single variable and single client.
//! Start server, check client, change the value via the server and the client.
//! Here we also control callback signaling.

TEST_F(PvAccessClientServerIntegrationTests, ServerWithSingleVariableAndSingleClientWithCallbacks)
{
  MockListener server_listener;
  MockListener client_listener;

  const std::string channel_name("channel0");

  // setting up callback expectations
  EXPECT_CALL(server_listener, OnServerValueChanged(_, _)).Times(0);
  EXPECT_CALL(client_listener, OnClientValueChanged(channel_name, _)).Times(::testing::AtLeast(1));

  // creating server with single variable
  PvAccessServer server(PvAccessServer::Isolated, server_listener.GetServerCallBack());
  sup::dto::AnyValue any_value({{"value", {sup::dto::SignedInteger32Type, 42}}});
  server.AddVariable(channel_name, any_value);

  server.Start();

  // Checking variable on server side
  EXPECT_TRUE(BusyWaitFor(1.0, [&]() { return server.GetValue(channel_name) == any_value; }));

  // creating a client with single variable
  PvAccessClient client = server.CreateClient(client_listener.GetClientCallBack());
  client.AddVariable(channel_name);

  // checking connection and updated values on server and client sides
  EXPECT_TRUE(BusyWaitFor(1.0, [&]() { return server.GetValue(channel_name) == any_value; }));
  EXPECT_TRUE(client.WaitForConnected(channel_name, 1.0));
  EXPECT_TRUE(BusyWaitFor(1.0, [&]() { return client.GetValue(channel_name) == any_value; }));

  // validating callbacks and clearing listeners
  testing::Mock::VerifyAndClearExpectations(&server_listener);
  testing::Mock::VerifyAndClearExpectations(&client_listener);

  // changing the value via the server and checking values on server and client sides
  any_value["value"] = 43;

  // setting up callback expectations
  EXPECT_CALL(server_listener, OnServerValueChanged(channel_name, _)).Times(1);
  EXPECT_CALL(client_listener, OnClientValueChanged(channel_name, _)).Times(1);

  server.SetValue(channel_name, any_value);

  EXPECT_TRUE(BusyWaitFor(1.0, [&]() { return server.GetValue(channel_name) == any_value; }));
  EXPECT_TRUE(BusyWaitFor(1.0, [&]() { return client.GetValue(channel_name) == any_value; }));

  // validating callbacks and clearing listeners
  testing::Mock::VerifyAndClearExpectations(&server_listener);
  testing::Mock::VerifyAndClearExpectations(&client_listener);

  // changing the value via the client and checking values on server and client sides
  any_value["value"] = 44;

  // setting up callback expectations
  EXPECT_CALL(server_listener, OnServerValueChanged(channel_name, _)).Times(1);
  EXPECT_CALL(client_listener, OnClientValueChanged(channel_name, _)).Times(1);

  client.SetValue(channel_name, any_value);

  EXPECT_TRUE(BusyWaitFor(1.0, [&]() { return server.GetValue(channel_name) == any_value; }));
  EXPECT_TRUE(BusyWaitFor(1.0, [&]() { return client.GetValue(channel_name) == any_value; }));

  // validating callbacks and clearing listeners
  testing::Mock::VerifyAndClearExpectations(&server_listener);
  testing::Mock::VerifyAndClearExpectations(&client_listener);

  // ignore possible disconnect callback
  EXPECT_CALL(client_listener, OnClientValueChanged(channel_name, _)).Times(::testing::AtLeast(0));
}

//! Test with non-isolated server/client. Server with single variable and single client.
//! Start server, check client, change the value via the server and the client.

TEST_F(PvAccessClientServerIntegrationTests, ClientServerFromEnv)
{
  const std::string channel_name("TEST:PVA-ClientServerFromEnv");

  // creating server with single variable
  PvAccessServer server{};
  sup::dto::AnyValue any_value({{"value", {sup::dto::SignedInteger32Type, 42}}});
  server.AddVariable(channel_name, any_value);

  server.Start();

  // Checking variable on server side
  EXPECT_TRUE(BusyWaitFor(1.0, [&]() { return server.GetValue(channel_name) == any_value; }));

  // creating a client with single variable
  PvAccessClient client = server.CreateClient();
  client.AddVariable(channel_name);

  // checking connection and updated values on server and client sides
  EXPECT_TRUE(BusyWaitFor(1.0, [&]() { return server.GetValue(channel_name) == any_value; }));
  EXPECT_TRUE(client.WaitForConnected(channel_name, 1.0));
  EXPECT_TRUE(BusyWaitFor(1.0, [&]() { return client.GetValue(channel_name) == any_value; }));

  // changing the value via the server and checking values on server and client sides
  EXPECT_THROW(server.SetValue(channel_name, 43), std::runtime_error);
  any_value["value"] = 43;
  server.SetValue(channel_name, any_value);

  EXPECT_TRUE(BusyWaitFor(1.0, [&]() { return server.GetValue(channel_name) == any_value; }));
  EXPECT_TRUE(BusyWaitFor(1.0, [&]() { return client.GetValue(channel_name) == any_value; }));

  // changing the value via the client and checking values on server and client sides
  any_value["value"] = 44;

  client.SetValue(channel_name, any_value);

  EXPECT_TRUE(BusyWaitFor(1.0, [&]() { return server.GetValue(channel_name) == any_value; }));
  EXPECT_TRUE(BusyWaitFor(1.0, [&]() { return client.GetValue(channel_name) == any_value; }));
}
