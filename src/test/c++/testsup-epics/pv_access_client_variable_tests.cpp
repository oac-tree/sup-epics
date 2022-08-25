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
#include <pvxs/data.h>
#include <pvxs/nt.h>
#include <pvxs/server.h>
#include <pvxs/sharedpv.h>
#include <sup/dto/anytype.h>
#include <sup/dto/anyvalue.h>
#include <sup/epics/dto_conversion_utils.h>
#include <sup/epics/pvxs/pv_access_client_variable.h>

#include <memory>
#include <thread>

using msec = std::chrono::milliseconds;
using ::testing::_;

namespace
{
const int kInitialValue = 42;
const int kInitialStatus = 1;
const std::string kChannelName = "PVXS-TESTS:NTSCALAR";
}  // namespace

class PVAccessClientVariableTests : public ::testing::Test
{
public:
  using shared_context_t = std::shared_ptr<pvxs::client::Context>;

  PVAccessClientVariableTests()
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

TEST_F(PVAccessClientVariableTests, InitialStateWhenNoServer)
{
  auto shared_context = CreateSharedContext();

  const std::string expected_name("NON_EXISTING:INT");
  sup::epics::PVAccessClientVariable variable(expected_name, shared_context);

  EXPECT_EQ(variable.GetVariableName(), expected_name);
  EXPECT_FALSE(variable.IsConnected());

  auto any_value = variable.GetValue();
  EXPECT_TRUE(::sup::dto::IsEmptyValue(any_value));
}

//! Sets the value through the unconnected client. The value of the cache should be changed.

TEST_F(PVAccessClientVariableTests, SetValueWhenUnconnected)
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

//! Checking the method WaitForConnected.

TEST_F(PVAccessClientVariableTests, WaitForConnected)
{
  auto shared_context = CreateSharedContext();
  sup::epics::PVAccessClientVariable variable(kChannelName, shared_context);

  // Server is not started, waiting will fail.
  EXPECT_FALSE(variable.WaitForConnected(0.01));

  // Server started, waiting will succeed.
  m_server.start();
  EXPECT_TRUE(variable.WaitForConnected(0.1));
}

//! A server with a single variable is created before the client.
//! Checks client connected/disconnected status.

TEST_F(PVAccessClientVariableTests, DisconnectionOnServerStop)
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

//! A server with a single variable is created before the client, but started after the client was
//! created. Checking that the client gets connected after the server start.

TEST_F(PVAccessClientVariableTests, ConnectionOnServerStart)
{
  m_shared_pv.open(m_pvxs_value);
  auto shared_context = CreateSharedContext();

  sup::epics::PVAccessClientVariable variable(kChannelName, shared_context);
  EXPECT_FALSE(variable.IsConnected());

  // starting the server
  m_server.start();
  std::this_thread::sleep_for(msec(20));

  // checking that the variable is connected
  EXPECT_TRUE(variable.IsConnected());
}

//! A server with a single variable is created and started before the client.
//! Check the client's initial value of the variable after the connection.

TEST_F(PVAccessClientVariableTests, GetValueAfterConnection)
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

//! A server with a single variable is created and started before the client.
//! The client is constructed with callback provided.
//! Check the callback value of the variable after the connection.

TEST_F(PVAccessClientVariableTests, CallbackAfterConnection)
{
  MockListener listener;

  m_server.start();
  m_shared_pv.open(m_pvxs_value);
  auto shared_context = CreateSharedContext();

  // setting up callback expectations
  const auto expected_any_value = sup::epics::BuildAnyValue(m_pvxs_value);
  EXPECT_CALL(listener, OnValueChanged(expected_any_value)).Times(1);

  sup::epics::PVAccessClientVariable variable(kChannelName, shared_context, listener.GetCallBack());
  std::this_thread::sleep_for(msec(20));

  EXPECT_TRUE(variable.IsConnected());

  auto result = variable.GetValue();
  EXPECT_EQ(result["value"], kInitialValue);
  EXPECT_EQ(result, expected_any_value);
}

//! Server with variable and initial value created before the client.
//! The client gets the structure from the server, modifies one field, and sets the value back.
//! Test check that the server value has changed.

TEST_F(PVAccessClientVariableTests, SetFromClient)
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

TEST_F(PVAccessClientVariableTests, DISABLED_MultipleSetFromClient)
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

TEST_F(PVAccessClientVariableTests, TwoClients)
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

//! Server with variable and initial value created before two clients.
//! One client set the value, second checks updated value.
//! Both clients are initialised via callbacks.

TEST_F(PVAccessClientVariableTests, TwoClientsCallbacks)
{
  MockListener listener1;
  MockListener listener2;

  m_server.start();
  m_shared_pv.open(m_pvxs_value);
  auto shared_context = CreateSharedContext();

  // callback expectation on variable connection
  EXPECT_CALL(listener1, OnValueChanged(_)).Times(1);
  EXPECT_CALL(listener2, OnValueChanged(_)).Times(1);

  sup::epics::PVAccessClientVariable variable1(kChannelName, shared_context,
                                               listener1.GetCallBack());
  sup::epics::PVAccessClientVariable variable2(kChannelName, shared_context,
                                               listener2.GetCallBack());

  std::this_thread::sleep_for(msec(20));

  EXPECT_TRUE(variable1.IsConnected());
  EXPECT_TRUE(variable2.IsConnected());

  testing::Mock::VerifyAndClearExpectations(&listener1);
  testing::Mock::VerifyAndClearExpectations(&listener2);

  // retrieving value through first variable
  auto any_value1 = variable1.GetValue();
  EXPECT_EQ(any_value1["value"], kInitialValue);

  // retrieving value through the second variable
  auto any_value2 = variable2.GetValue();
  EXPECT_EQ(any_value2["value"], kInitialValue);

  // setting the value through the first variable
  any_value1["value"] = 45;

  // callback expectation on setting the value through one of the client
  EXPECT_CALL(listener1, OnValueChanged(any_value1)).Times(1);
  EXPECT_CALL(listener2, OnValueChanged(any_value1)).Times(1);

  EXPECT_TRUE(variable1.SetValue(any_value1));

  std::this_thread::sleep_for(msec(20));

  // checking the value through the second variable
  auto any_value3 = variable2.GetValue();
  EXPECT_EQ(any_value3["value"], 45);

  // checking the value on server side
  auto shared_value = m_shared_pv.fetch();
  EXPECT_EQ(shared_value["value"].as<int>(), 45);
}

//! Validating implicit struct-scalar conversion. A `struct-scalar` is a special structure
//! with a single `value` field holding a scalar. The current implementation is that scalars on
//! AnyValue side becomes a `struct-scalar` on the PVXS side. Similarly, `struct-scalar` on PVXS
//! side should become a bare scalar on AnyValue side.
//!
//! Server with variable and initial value created before the client. Variable contains PVXS value
//! with a struct with a single `value` field.

TEST_F(PVAccessClientVariableTests, GetSetFromClientForScalarAwareCase)
{
  auto pvxs_struct_scalar_value =
      ::pvxs::TypeDef(::pvxs::TypeCode::Struct, {pvxs::members::Int32("value")}).create();
  pvxs_struct_scalar_value["value"] = 42;

  m_server.start();
  m_shared_pv.open(pvxs_struct_scalar_value);
  auto shared_context = CreateSharedContext();

  sup::epics::PVAccessClientVariable variable(kChannelName, shared_context);
  std::this_thread::sleep_for(msec(20));

  EXPECT_TRUE(variable.IsConnected());

  // retrieving value
  auto any_value = variable.GetValue();
  EXPECT_EQ(any_value.GetType(), sup::dto::SignedInteger32Type);
  EXPECT_EQ(any_value, kInitialValue);

  // modifying the field in retrieved value
  any_value = kInitialValue + 1;
  EXPECT_TRUE(variable.SetValue(any_value));
  std::this_thread::sleep_for(msec(20));

  auto shared_value = m_shared_pv.fetch();
  EXPECT_EQ(shared_value["value"].as<int>(), kInitialValue + 1);
}
