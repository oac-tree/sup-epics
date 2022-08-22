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
const int kInitialStatus = 1;
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
    m_pvxs_value["alarm.status"] = kInitialStatus;
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

//! Sets the value through the unconnected client. The value of the cache should be changed.

TEST_F(PVAccessClientVariableTest, SetValueWhenUnconnected)
{
  auto shared_context = CreateSharedContext();
  sup::epics::PVAccessClientVariable variable(kChannelName, shared_context);

  // seting the value
  sup::dto::AnyValue any_value{sup::dto::SignedInteger32Type};
  any_value = 42;
  EXPECT_TRUE(variable.SetValue(any_value));

  // expecting to get same value
  auto result = variable.GetValue();
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
//! Check the client's initial value of the variable after the connection.

TEST_F(PVAccessClientVariableTest, GetValueAfterConnection)
{
  m_server.start();
  m_shared_pv.open(m_pvxs_value);
  auto shared_context = CreateSharedContext();

  sup::epics::PVAccessClientVariable variable(kChannelName, shared_context);
  std::this_thread::sleep_for(msec(20));

  EXPECT_TRUE(variable.IsConnected());

  auto result = variable.GetValue();
  EXPECT_EQ(result["value"], kInitialValue);
}

//! Server with variable and initial value created before the client.
//! The client gets the structure from the server, modifies one field, and sets the value back.
//! Test check that the server value has changed.

TEST_F(PVAccessClientVariableTest, SetFromClient)
{
  m_server.start();
  m_shared_pv.open(m_pvxs_value);
  auto shared_context = CreateSharedContext();

  sup::epics::PVAccessClientVariable variable(kChannelName, shared_context);
  std::this_thread::sleep_for(msec(20));

  EXPECT_TRUE(variable.IsConnected());

  // retrieving value
  auto any_value = variable.GetValue();
  EXPECT_EQ(any_value["value"], kInitialValue);

  // modifying the field in retrieved value
  any_value["value"] = kInitialValue + 1;
  EXPECT_TRUE(variable.SetValue(any_value));
  std::this_thread::sleep_for(msec(20));

  auto shared_value = m_shared_pv.fetch();
  EXPECT_EQ(shared_value["value"].as<int>(), kInitialValue + 1);
}

//! Server with variable and initial value created before the client.
//! The client gets the structure from the server and sets the value of one field three times in a
//! row without any extra delays. This led to the situation, where every next operation, destroys
//! the one being executed. The test checks the last value on the server side.
//!
//! This test often hangs and it is disabled for the moment. FIXME.

TEST_F(PVAccessClientVariableTest, DISABLE_MultipleSetFromClient)
{
  m_server.start();
  m_shared_pv.open(m_pvxs_value);
  auto shared_context = CreateSharedContext();

  sup::epics::PVAccessClientVariable variable(kChannelName, shared_context);
  std::this_thread::sleep_for(msec(20));

  EXPECT_TRUE(variable.IsConnected());

  // retrieving value
  auto any_value = variable.GetValue();
  EXPECT_EQ(any_value["value"], kInitialValue);

  //  std::cerr.setstate(std::ios_base::failbit);

  // modifying the field in retrieved value
  for (int i = 1; i <= 3; ++i)
  {
    any_value["value"] = kInitialValue + i;
    EXPECT_TRUE(variable.SetValue(any_value));
  }
  //  shared_context->hurryUp();
  //  std::cerr.clear();
  std::this_thread::sleep_for(msec(20));

  // It will generate a log message on the screen
  // "ERR pvxs.client.io Server 127.0.0.1:5075 uses non-existent IOID 268443649.  Ignoring..."
  // This is because the client destroys pvxs::client::Operation with a new SetValue request and the
  // server is not able to finalize the taking of the previous value.

  auto shared_value = m_shared_pv.fetch();
  EXPECT_EQ(shared_value["value"].as<int>(), kInitialValue + 3);
}

//! Server with variable and initial value created before two clients.
//! One client set the value, second checks updated value.

TEST_F(PVAccessClientVariableTest, TwoClients)
{
  m_server.start();
  m_shared_pv.open(m_pvxs_value);
  auto shared_context = CreateSharedContext();

  sup::epics::PVAccessClientVariable variable1(kChannelName, shared_context);
  sup::epics::PVAccessClientVariable variable2(kChannelName, shared_context);

  std::this_thread::sleep_for(msec(20));

  EXPECT_TRUE(variable1.IsConnected());
  EXPECT_TRUE(variable2.IsConnected());

  // retrieving value through first variable
  auto any_value1 = variable1.GetValue();
  EXPECT_EQ(any_value1["value"], kInitialValue);

  // retrieving value through the second variable
  auto any_value2 = variable2.GetValue();
  EXPECT_EQ(any_value2["value"], kInitialValue);

  // setting the value through the first variable
  any_value1["value"] = 45;
  EXPECT_TRUE(variable1.SetValue(any_value1));

  std::this_thread::sleep_for(msec(20));

  // checking the value through the second variable
  auto any_value3 = variable2.GetValue();
  EXPECT_EQ(any_value3["value"], 45);

  // checking the value on server side
  auto shared_value = m_shared_pv.fetch();
  EXPECT_EQ(shared_value["value"].as<int>(), 45);
}

//! Server with variable and initial value created before the client.
//! The client sets the value on the server using a custom structure, with value filed.
//! The test proves, that `pvxs::server::Server` allows updating server variable with pvxs::Value
//! that doesn't much the remote one. It will use only matching fields.

TEST_F(PVAccessClientVariableTest, SetFromSimilarStructure)
{
  m_server.start();
  m_shared_pv.open(m_pvxs_value);
  auto shared_context = CreateSharedContext();

  sup::epics::PVAccessClientVariable variable(kChannelName, shared_context);
  std::this_thread::sleep_for(msec(20));

  EXPECT_TRUE(variable.IsConnected());

  // constructing a structure that doesn't coincide with variable on server side
  // however it has `value` field
  sup::dto::AnyValue any_value = {{{"value", {sup::dto::SignedInteger32Type, 22}}}};

  // setting custom AnyValue via client
  EXPECT_TRUE(variable.SetValue(any_value));
  std::this_thread::sleep_for(msec(20));

  // We see that the `value` field has bin updated on server side.
  // The field `alarm.status` on server side remained untouched.
  auto shared_value = m_shared_pv.fetch();
  EXPECT_EQ(shared_value["value"].as<int>(), 22);
  EXPECT_EQ(shared_value["alarm.status"].as<int>(), kInitialStatus);
}
