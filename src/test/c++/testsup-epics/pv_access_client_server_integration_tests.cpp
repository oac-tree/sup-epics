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
#include "unit_test_helper.h"

#include <sup/epics/pvxs/pv_access_client_impl.h>

#include <gtest/gtest.h>
#include <pvxs/client.h>
#include <pvxs/server.h>
#include <sup/dto/anyvalue.h>
#include <sup/epics/pv_access_client.h>
#include <sup/epics/pv_access_server.h>
#include <sup/epics/pv_access_context_utils.h>

#include <thread>

using ::testing::_;
using sup::epics::unit_test_helper::BusyWaitFor;

using namespace sup::epics;

//! Testing PvAccessServer and PvAccessClient together.

class PVAccessClientServerIntegrationTests : public ::testing::Test
{
public:
  using client_context_t = std::shared_ptr<pvxs::client::Context>;

  std::unique_ptr<sup::epics::PvAccessClientImpl> CreateClientImpl(
    const pvxs::server::Server& server, sup::epics::PvAccessClient::VariableChangedCallback cb = {})
  {
    std::shared_ptr<pvxs::client::Context> context =
      std::make_shared<pvxs::client::Context>(server.clientConfig().build());
    std::unique_ptr<sup::epics::PvAccessClientImpl> result{
      new sup::epics::PvAccessClientImpl(context, cb)};
    return std::move(result);
  }
};

//! Standard scenario. Server with single variable and single client.
//! Start server, check client, change the value via the server and the client.

TEST_F(PVAccessClientServerIntegrationTests, ServerWithSingleVariableAndSingleClient)
{
  const std::string channel_name("channel0");

  // creating PVXS server and corresponding client context
  auto pvxs_server = CreateIsolatedServer();
  PvAccessClient client(CreateClientImpl(*pvxs_server));

  // creating server with single variable
  PvAccessServer server(std::move(pvxs_server));
  sup::dto::AnyValue any_value{sup::dto::SignedInteger32Type, 42};
  server.AddVariable(channel_name, any_value);

  server.Start();

  // Checking variable on server side
  EXPECT_TRUE(BusyWaitFor(1.0, [&](){ return server.GetValue(channel_name) == any_value; }));

  // creating a client with single variable
  client.AddVariable(channel_name);

  // checking connection and updated values on server and client sides
  EXPECT_TRUE(BusyWaitFor(1.0, [&](){ return server.GetValue(channel_name) == any_value; }));
  EXPECT_TRUE(client.WaitForConnected(channel_name, 1.0));
  EXPECT_TRUE(BusyWaitFor(1.0, [&](){ return client.GetValue(channel_name) == any_value; }));

  // changing the value via the server and checking values on server and client sides
  sup::dto::AnyValue new_any_value1{sup::dto::SignedInteger32Type, 43};
  server.SetValue(channel_name, new_any_value1);

  EXPECT_TRUE(BusyWaitFor(1.0, [&](){ return server.GetValue(channel_name) == new_any_value1; }));
  EXPECT_TRUE(BusyWaitFor(1.0, [&](){ return client.GetValue(channel_name) == new_any_value1; }));

  // changing the value via the client and checking values on server and client sides
  sup::dto::AnyValue new_any_value2{sup::dto::SignedInteger32Type, 44};

  client.SetValue(channel_name, new_any_value2);

  EXPECT_TRUE(BusyWaitFor(1.0, [&](){ return server.GetValue(channel_name) == new_any_value2; }));
  EXPECT_TRUE(BusyWaitFor(1.0, [&](){ return client.GetValue(channel_name) == new_any_value2; }));
}

//! Standard scenario. Server with single variable and single client.
//! Start server, check client, change the value via the server and the client.
//! Here we also control callback signaling.

TEST_F(PVAccessClientServerIntegrationTests, ServerWithSingleVariableAndSingleClientWithCallbacks)
{
  MockListener server_listener;
  MockListener client_listener;

  const std::string channel_name("channel0");

  // setting up callback expectations
  EXPECT_CALL(server_listener, OnNamedValueChanged_old(_, _)).Times(0);
  EXPECT_CALL(client_listener, OnNamedValueChanged(channel_name, _)).Times(::testing::AtLeast(1));

  // creating PVXS server and corresponding client context
  auto pvxs_server = CreateIsolatedServer();
  PvAccessClient client(CreateClientImpl(*pvxs_server, client_listener.GetNamedCallBack()));

  // creating server with single variable
  PvAccessServer server(std::move(pvxs_server), server_listener.GetNamedCallBack_old());
  sup::dto::AnyValue any_value{sup::dto::SignedInteger32Type, 42};
  server.AddVariable(channel_name, any_value);

  server.Start();

  // Checking variable on server side
  EXPECT_TRUE(BusyWaitFor(1.0, [&](){ return server.GetValue(channel_name) == any_value; }));

  // creating a client with single variable
  client.AddVariable(channel_name);

  // checking connection and updated values on server and client sides
  EXPECT_TRUE(BusyWaitFor(1.0, [&](){ return server.GetValue(channel_name) == any_value; }));
  EXPECT_TRUE(client.WaitForConnected(channel_name, 1.0));
  EXPECT_TRUE(BusyWaitFor(1.0, [&](){ return client.GetValue(channel_name) == any_value; }));

  // validating callbacks and clearing listeners
  testing::Mock::VerifyAndClearExpectations(&server_listener);
  testing::Mock::VerifyAndClearExpectations(&client_listener);

  // changing the value via the server and checking values on server and client sides
  sup::dto::AnyValue new_any_value1{sup::dto::SignedInteger32Type, 43};

  // setting up callback expectations
  EXPECT_CALL(server_listener, OnNamedValueChanged_old(channel_name, _)).Times(1);
  EXPECT_CALL(client_listener, OnNamedValueChanged(channel_name, _)).Times(1);

  server.SetValue(channel_name, new_any_value1);

  EXPECT_TRUE(BusyWaitFor(1.0, [&](){ return server.GetValue(channel_name) == new_any_value1; }));
  EXPECT_TRUE(BusyWaitFor(1.0, [&](){ return client.GetValue(channel_name) == new_any_value1; }));

  // validating callbacks and clearing listeners
  testing::Mock::VerifyAndClearExpectations(&server_listener);
  testing::Mock::VerifyAndClearExpectations(&client_listener);

  // changing the value via the client and checking values on server and client sides
  sup::dto::AnyValue new_any_value2{sup::dto::SignedInteger32Type, 44};

  // setting up callback expectations
  EXPECT_CALL(server_listener, OnNamedValueChanged_old(channel_name, _)).Times(1);
  EXPECT_CALL(client_listener, OnNamedValueChanged(channel_name, _)).Times(1);

  client.SetValue(channel_name, new_any_value2);

  EXPECT_TRUE(BusyWaitFor(1.0, [&](){ return server.GetValue(channel_name) == new_any_value2; }));
  EXPECT_TRUE(BusyWaitFor(1.0, [&](){ return client.GetValue(channel_name) == new_any_value2; }));

  // validating callbacks and clearing listeners
  testing::Mock::VerifyAndClearExpectations(&server_listener);
  testing::Mock::VerifyAndClearExpectations(&client_listener);

  // ignore possible disconnect callback
  EXPECT_CALL(client_listener, OnNamedValueChanged(channel_name, _)).Times(::testing::AtLeast(0));
}
