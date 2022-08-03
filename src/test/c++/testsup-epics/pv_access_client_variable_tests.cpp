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

#include <gtest/gtest.h>
#include <pvxs/client.h>
#include <pvxs/data.h>
#include <pvxs/nt.h>
#include <pvxs/server.h>
#include <pvxs/sharedpv.h>
#include <sup/dto/anytype.h>
#include <sup/dto/anyvalue.h>
#include <sup/epics/pvxs/pv_access_client_variable.h>

#include <memory>
#include <thread>

using msec = std::chrono::milliseconds;

namespace
{
const int kInitialValue = 42;
const std::string kChannelName = "PVXS-TESTS:NTSCALAR";
}  // namespace

class PVAccessClientVariableTest : public ::testing::Test
{
public:
  using shared_context_t = std::shared_ptr<pvxs::client::Context>;

  PVAccessClientVariableTest()
      : m_pvxs_value(pvxs::nt::NTScalar{pvxs::TypeCode::Int32}.create())
      , m_shared_pv(pvxs::server::SharedPV::buildMailbox())
      , m_server(pvxs::server::Config::isolated().build().addPV(kChannelName, m_shared_pv))
  {
    m_pvxs_value["value"] = kInitialValue;
  }

  //! Create PVXS context intended for sharing among multiple PVAccessClientVariable variables.
  shared_context_t CreateSharedContext()
  {
    return std::make_shared<pvxs::client::Context>(m_server.clientConfig().build());
  }

  pvxs::Value m_pvxs_value;
  pvxs::server::SharedPV m_shared_pv;
  pvxs::server::Server m_server;
};

//! Initial state of PVAccessClientVariable when no server exists.

TEST_F(PVAccessClientVariableTest, InitialStateWhenNoServer)
{
  auto shared_context = CreateSharedContext();

  const std::string expected_name("NON_EXISTING:INT");
  sup::epics::PVAccessClientVariable variable(expected_name, shared_context);

  EXPECT_EQ(variable.GetVariableName(), expected_name);
  EXPECT_FALSE(variable.IsConnected());
}

//! Sets the value through unconnected client. The value of the cache should be changed.

TEST_F(PVAccessClientVariableTest, SetValueWhenUnconnected)
{
  auto shared_context = CreateSharedContext();
  sup::epics::PVAccessClientVariable variable(kChannelName, shared_context);

  // seting the value
  sup::dto::AnyValue any_value{sup::dto::SignedInteger32Type};
  any_value = 42;
  EXPECT_TRUE(variable.SetValue(any_value));

  // expecting to get same value
  sup::dto::AnyValue result;
  EXPECT_TRUE(variable.GetValue(result));
  EXPECT_EQ(any_value, result);
}

//! A server with a single variable is created before the client.
//! Checks client connected/disconnected status.

TEST_F(PVAccessClientVariableTest, DisconnectionOnServerStop)
{
  m_server.start();
  m_shared_pv.open(m_pvxs_value);
  auto shared_context = CreateSharedContext();

  sup::epics::PVAccessClientVariable variable(kChannelName, shared_context);
  std::this_thread::sleep_for(msec(20));

  EXPECT_TRUE(variable.IsConnected());

  // stopping server
  m_server.stop();
  std::this_thread::sleep_for(msec(20));

  EXPECT_FALSE(variable.IsConnected());
}

//! A server with a single variable is created before the client.
//! Check client's initial value of the variable after the connection.

TEST_F(PVAccessClientVariableTest, GetValueAfterConnection)
{
  m_server.start();
  m_shared_pv.open(m_pvxs_value);
  auto shared_context = CreateSharedContext();

  sup::epics::PVAccessClientVariable variable(kChannelName, shared_context);
  std::this_thread::sleep_for(msec(20));

  EXPECT_TRUE(variable.IsConnected());

  sup::dto::AnyValue result;
  EXPECT_TRUE(variable.GetValue(result));
  EXPECT_EQ(result["value"], kInitialValue);
}

//! Server with variable and initial value created before the client.
//! Client gets the value from server, modifies one field and set the value back.
//! Test check that server value has changed.

TEST_F(PVAccessClientVariableTest, SetFromClient)
{
  m_server.start();
  m_shared_pv.open(m_pvxs_value);
  auto shared_context = CreateSharedContext();

  sup::epics::PVAccessClientVariable variable(kChannelName, shared_context);
  std::this_thread::sleep_for(msec(20));

  EXPECT_TRUE(variable.IsConnected());

  // retrieving value
  sup::dto::AnyValue any_value;
  EXPECT_TRUE(variable.GetValue(any_value));
  EXPECT_EQ(any_value["value"], kInitialValue);

  // modifying the field in retrieved value
  any_value["value"] = kInitialValue + 1;
  EXPECT_TRUE(variable.SetValue(any_value));
  std::this_thread::sleep_for(msec(20));

  auto shared_value = m_shared_pv.fetch();
  EXPECT_EQ(shared_value["value"].as<int>(), kInitialValue + 1);
}
