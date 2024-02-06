/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Gennady Pospelov (IO)
 *
 * Copyright (c) : 2010-2024 ITER Organization,
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
#include <sup/epics/pv_access_client_pv.h>
#include <sup/epics/pvxs/pv_access_client_pv_impl.h>
#include <sup/epics/utils/dto_conversion_utils.h>

#include <memory>
#include <thread>

#include <sup/epics-test/unit_test_helper.h>

using sup::epics::test::BusyWaitFor;
using ::testing::_;

namespace
{
const int kInitialValue = 42;
const int kInitialStatus = 1;
const std::string kChannelName = "PVXS-TESTS:NTSCALAR";

}  // namespace

class PvAccessClientPVTests : public ::testing::Test
{
public:
  using shared_context_t = std::shared_ptr<pvxs::client::Context>;

  PvAccessClientPVTests()
      : m_pvxs_value(pvxs::nt::NTScalar{pvxs::TypeCode::Int32}.create())
      , m_shared_pv(pvxs::server::SharedPV::buildMailbox())
      , m_server(pvxs::server::Config::isolated().build().addPV(kChannelName, m_shared_pv))
  {
    m_pvxs_value["value"] = kInitialValue;
    m_pvxs_value["alarm.status"] = kInitialStatus;
  }

  std::unique_ptr<sup::epics::PvAccessClientPVImpl> CreateClientPVImpl(
      const std::string& channel, sup::epics::PvAccessClientPV::VariableChangedCallback cb = {})
  {
    std::shared_ptr<pvxs::client::Context> context =
        std::make_shared<pvxs::client::Context>(m_server.clientConfig().build());
    std::unique_ptr<sup::epics::PvAccessClientPVImpl> result{
        new sup::epics::PvAccessClientPVImpl(channel, context, cb)};
    return result;
  }

  pvxs::Value m_pvxs_value;
  pvxs::server::SharedPV m_shared_pv;
  pvxs::server::Server m_server;
};

//! Initial state of PvAccessClientPV when no server exists.

TEST_F(PvAccessClientPVTests, InitialStateWhenNoServer)
{
  const std::string expected_name("NON_EXISTING:INT");
  sup::epics::PvAccessClientPV variable(CreateClientPVImpl(expected_name));

  EXPECT_EQ(variable.GetChannelName(), expected_name);
  EXPECT_FALSE(variable.IsConnected());

  auto any_value = variable.GetValue();
  EXPECT_TRUE(::sup::dto::IsEmptyValue(any_value));
}

//! Sets the value through the unconnected client. The value of the cache should be changed.

TEST_F(PvAccessClientPVTests, SetValueWhenUnconnected)
{
  sup::epics::PvAccessClientPV variable(CreateClientPVImpl(kChannelName));

  // seting the value
  sup::dto::AnyValue any_value{sup::dto::SignedInteger32Type};
  any_value = 42;
  EXPECT_FALSE(variable.SetValue(any_value));

  // expecting to get old value
  EXPECT_TRUE(::sup::dto::IsEmptyValue(variable.GetValue()));
}

//! Checking the method WaitForConnected.

TEST_F(PvAccessClientPVTests, WaitForConnected)
{
  sup::epics::PvAccessClientPV variable(CreateClientPVImpl(kChannelName));

  // Server is not started, waiting will fail.
  EXPECT_FALSE(variable.WaitForConnected(0.01));

  // Server started, waiting will succeed.
  m_server.start();
  EXPECT_TRUE(variable.WaitForConnected(1.0));
}

//! A server with a single variable is created before the client.
//! Checks client connected/disconnected status.

TEST_F(PvAccessClientPVTests, DisconnectionOnServerStop)
{
  m_server.start();
  m_shared_pv.open(m_pvxs_value);

  sup::epics::PvAccessClientPV variable(CreateClientPVImpl(kChannelName));

  EXPECT_TRUE(variable.WaitForConnected(1.0));

  // stopping server
  m_server.stop();

  EXPECT_TRUE(BusyWaitFor(1.0, [&variable]() { return !variable.IsConnected(); }));
}

//! A server with a single variable is created before the client, but started after the client was
//! created. Checking that the client gets connected after the server start.

TEST_F(PvAccessClientPVTests, ConnectionOnServerStart)
{
  m_shared_pv.open(m_pvxs_value);

  sup::epics::PvAccessClientPV variable(CreateClientPVImpl(kChannelName));
  EXPECT_FALSE(variable.IsConnected());

  // starting the server
  m_server.start();

  // checking that the variable is connected
  EXPECT_TRUE(variable.WaitForConnected(1.0));
}

//! A server with a single variable is created and started before the client.
//! Check the client's initial value of the variable after the connection.

TEST_F(PvAccessClientPVTests, GetValueAfterConnection)
{
  m_server.start();
  m_shared_pv.open(m_pvxs_value);

  sup::epics::PvAccessClientPV variable(CreateClientPVImpl(kChannelName));

  EXPECT_TRUE(variable.WaitForValidValue(1.0));

  auto result = variable.GetValue();
  EXPECT_EQ(result["value"], kInitialValue);
}

//! A server with a single variable is created and started before the client.
//! The client is constructed with callback provided.
//! Check the callback value of the variable after the connection.

TEST_F(PvAccessClientPVTests, CallbackAfterConnection)
{
  MockListener listener;

  m_server.start();
  m_shared_pv.open(m_pvxs_value);

  // setting up callback expectations
  EXPECT_CALL(listener, OnClientPVValueChanged(_)).Times(::testing::AtLeast(1));

  sup::epics::PvAccessClientPV variable(
      CreateClientPVImpl(kChannelName, listener.GetClientPVCallBack()));

  EXPECT_TRUE(variable.WaitForValidValue(1.0));

  auto result = variable.GetValue();
  EXPECT_EQ(result["value"], kInitialValue);
  EXPECT_EQ(result, sup::epics::BuildAnyValue(m_pvxs_value));
}

//! Server with variable and initial value created before the client.
//! The client gets the structure from the server, modifies one field, and sets the value back.
//! Test check that the server value has changed.

TEST_F(PvAccessClientPVTests, SetFromClient)
{
  m_server.start();
  m_shared_pv.open(m_pvxs_value);

  sup::epics::PvAccessClientPV variable(CreateClientPVImpl(kChannelName));

  EXPECT_TRUE(variable.WaitForValidValue(1.0));

  // retrieving value
  auto any_value = variable.GetValue();
  EXPECT_EQ(any_value["value"], kInitialValue);

  // modifying the field in retrieved value
  any_value["value"] = kInitialValue + 1;
  EXPECT_TRUE(variable.SetValue(any_value));

  EXPECT_TRUE(BusyWaitFor(1.0,
                          [this]()
                          {
                            auto shared_value = m_shared_pv.fetch();
                            return shared_value["value"].as<int>() == kInitialValue + 1;
                          }));
}

//! Server with variable and initial value created before the client.
//! The client gets the structure from the server and sets the value of one field three times in a
//! row without any extra delays. This led to the situation, where every next operation, destroys
//! the one being executed. The test checks the last value on the server side.
//!
//! This test often hangs and it is disabled for the moment. FIXME.

TEST_F(PvAccessClientPVTests, MultipleSetFromClient)
{
  m_server.start();
  m_shared_pv.open(m_pvxs_value);

  sup::epics::PvAccessClientPV variable(CreateClientPVImpl(kChannelName));

  EXPECT_TRUE(variable.WaitForValidValue(1.0));

  // retrieving value
  auto any_value = variable.GetValue();
  EXPECT_EQ(any_value["value"], kInitialValue);

  // modifying the field in retrieved value
  for (int i = 1; i <= 3; ++i)
  {
    any_value["value"] = kInitialValue + i;
    EXPECT_TRUE(variable.SetValue(any_value));
  }

  // It will generate a log message on the screen
  // "ERR pvxs.client.io Server 127.0.0.1:5075 uses non-existent IOID 268443649.  Ignoring..."
  // This is because the client destroys pvxs::client::Operation with a new SetValue request and the
  // server is not able to finalize the taking of the previous value.

  EXPECT_TRUE(BusyWaitFor(1.0,
                          [this]()
                          {
                            auto shared_value = m_shared_pv.fetch();
                            return shared_value["value"].as<int>() == kInitialValue + 3;
                          }));
}

//! Server with variable and initial value created before two clients.
//! One client set the value, second checks updated value.

TEST_F(PvAccessClientPVTests, TwoClients)
{
  m_server.start();
  m_shared_pv.open(m_pvxs_value);

  sup::epics::PvAccessClientPV variable1(CreateClientPVImpl(kChannelName));
  sup::epics::PvAccessClientPV variable2(CreateClientPVImpl(kChannelName));

  EXPECT_TRUE(variable1.WaitForValidValue(1.0));
  EXPECT_TRUE(variable2.WaitForValidValue(1.0));

  // retrieving value through first variable
  auto any_value1 = variable1.GetValue();
  EXPECT_EQ(any_value1["value"], kInitialValue);

  // retrieving value through the second variable
  auto any_value2 = variable2.GetValue();
  EXPECT_EQ(any_value2["value"], kInitialValue);

  // setting the value through the first variable
  any_value1["value"] = 45;
  EXPECT_TRUE(variable1.SetValue(any_value1));

  // checking the value through the second variable
  EXPECT_TRUE(BusyWaitFor(1.0,
                          [&variable2]()
                          {
                            auto var2_value = variable2.GetValue();
                            return var2_value["value"] == 45;
                          }));

  // checking the value on server side
  EXPECT_TRUE(BusyWaitFor(1.0,
                          [this]()
                          {
                            auto shared_value = m_shared_pv.fetch();
                            return shared_value["value"].as<int>() == 45;
                          }));
}

//! Server with variable and initial value created before two clients.
//! One client set the value, second checks updated value.
//! Both clients are initialised via callbacks.

TEST_F(PvAccessClientPVTests, TwoClientsCallbacks)
{
  MockListener listener1;
  MockListener listener2;

  m_server.start();
  m_shared_pv.open(m_pvxs_value);

  // callback expectation on variable connection, initial value and value update
  EXPECT_CALL(listener1, OnClientPVValueChanged(_)).Times(3);
  EXPECT_CALL(listener2, OnClientPVValueChanged(_)).Times(3);

  sup::epics::PvAccessClientPV variable1(
      CreateClientPVImpl(kChannelName, listener1.GetClientPVCallBack()));
  sup::epics::PvAccessClientPV variable2(
      CreateClientPVImpl(kChannelName, listener2.GetClientPVCallBack()));

  EXPECT_TRUE(variable1.WaitForValidValue(1.0));
  EXPECT_TRUE(variable2.WaitForValidValue(1.0));

  // retrieving value through first variable
  auto any_value1 = variable1.GetValue();
  EXPECT_EQ(any_value1["value"], kInitialValue);

  // retrieving value through the second variable
  auto any_value2 = variable2.GetValue();
  EXPECT_EQ(any_value2["value"], kInitialValue);

  // setting the value through the first variable
  any_value1["value"] = 45;

  EXPECT_TRUE(variable1.SetValue(any_value1));

  // checking the value through the second variable
  EXPECT_TRUE(BusyWaitFor(1.0,
                          [&variable2]()
                          {
                            auto var2_value = variable2.GetValue();
                            return var2_value["value"] == 45;
                          }));

  // checking the value on server side
  EXPECT_TRUE(BusyWaitFor(1.0,
                          [this]()
                          {
                            auto shared_value = m_shared_pv.fetch();
                            return shared_value["value"].as<int>() == 45;
                          }));

  testing::Mock::VerifyAndClearExpectations(&listener1);
  testing::Mock::VerifyAndClearExpectations(&listener2);
}

//! Validating implicit struct-scalar conversion. A `struct-scalar` is a special structure
//! with a single `value` field holding a scalar. The current implementation is that scalars on
//! AnyValue side becomes a `struct-scalar` on the PVXS side. Similarly, `struct-scalar` on PVXS
//! side should become a bare scalar on AnyValue side.
//!
//! Server with variable and initial value created before the client. Variable contains PVXS value
//! with a struct with a single `value` field.

TEST_F(PvAccessClientPVTests, GetSetFromClientForScalarAwareCase)
{
  auto pvxs_struct_scalar_value =
      ::pvxs::TypeDef(::pvxs::TypeCode::Struct, {pvxs::members::Int32("value")}).create();
  pvxs_struct_scalar_value["value"] = 42;

  m_server.start();
  m_shared_pv.open(pvxs_struct_scalar_value);

  sup::epics::PvAccessClientPV variable(CreateClientPVImpl(kChannelName));

  EXPECT_TRUE(variable.WaitForValidValue(1.0));

  // retrieving value
  auto any_value = variable.GetValue();
  EXPECT_TRUE(sup::dto::IsStructValue(any_value));
  ASSERT_TRUE(any_value.HasField("value"));
  EXPECT_EQ(any_value["value"], kInitialValue);

  // modifying the field in retrieved value
  sup::dto::AnyValue update = kInitialValue + 1;
  EXPECT_THROW(variable.SetValue(update), std::runtime_error);
}
