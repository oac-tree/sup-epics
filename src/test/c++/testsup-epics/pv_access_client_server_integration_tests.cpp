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
#include <sup/epics/pvxs/context_utils.h>

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

TEST_F(PVAccessClientServerIntegrationTests, ServerWithSingleVariableAndSingleClient)
{
  const std::string channel_name("channel0");

  // creating PVXS server and corresponding client context
  auto pvxs_server = CreateIsolatedServer();
  auto client_context = CreateClientContext(*pvxs_server);

  // creating server with single variable
  PVAccessServer server(std::move(pvxs_server));
  sup::dto::AnyValue any_value0{sup::dto::SignedInteger32Type, 42};
  server.AddVariable(channel_name, any_value0);

  server.Start();
  std::this_thread::sleep_for(msec(20));

  // Checking variable on server side
  EXPECT_EQ(server.GetValue(channel_name), any_value0);

  // creating a client with single variable
  PVAccessClient client(client_context);
  client.AddVariable(channel_name);

  // checking connection and updated values
  std::this_thread::sleep_for(msec(20));
  EXPECT_TRUE(client.IsConnected(channel_name));
  EXPECT_EQ(server.GetValue(channel_name), any_value0);

  auto client_value = client.GetValue(channel_name);
  EXPECT_EQ(client_value, any_value0);
}
