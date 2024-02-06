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
#include <sup/epics/pv_access_client.h>
#include <sup/epics/pvxs/pv_access_client_impl.h>
#include <sup/epics/utils/dto_conversion_utils.h>

#include <memory>
#include <stdexcept>
#include <thread>

#include <sup/epics-test/unit_test_helper.h>

using sup::epics::test::BusyWaitFor;
using ::testing::_;

namespace
{
const int kInitialIntChannelValue = 42;
const int kInitialIntChannelStatus = 1;
const std::string kIntChannelName = "PVXS-TESTS:INTSCALAR";

const std::string kInitialStringChannelValue = "abc";
const std::string kStringChannelName = "PVXS-TESTS:STRING";
}  // namespace

class PvAccessClientTest : public ::testing::Test
{
public:
  using shared_context_t = std::shared_ptr<pvxs::client::Context>;

  PvAccessClientTest()
      : m_shared_ntscalar_pv(pvxs::server::SharedPV::buildMailbox())
      , m_shared_string_pv(pvxs::server::SharedPV::buildMailbox())
  {
    // PVXS value representing integer based NTScalar
    m_pvxs_ntscalar_value = pvxs::nt::NTScalar{pvxs::TypeCode::Int32}.create();
    m_pvxs_ntscalar_value["value"] = kInitialIntChannelValue;
    m_pvxs_ntscalar_value["alarm.status"] = kInitialIntChannelStatus;

    // PVXS value representing a struct with string value field
    m_pvxs_string_value =
        ::pvxs::TypeDef(::pvxs::TypeCode::Struct, {pvxs::members::String("value")}).create();
    m_pvxs_string_value["value"] = kInitialStringChannelValue;

    m_server = pvxs::server::Config::isolated()
                   .build()
                   .addPV(kIntChannelName, m_shared_ntscalar_pv)
                   .addPV(kStringChannelName, m_shared_string_pv);
  }

  //! Create PVXS client implementation from server.
  std::unique_ptr<sup::epics::PvAccessClientImpl> CreateClientImpl(
      sup::epics::PvAccessClient::VariableChangedCallback cb = {})
  {
    std::shared_ptr<pvxs::client::Context> context =
        std::make_shared<pvxs::client::Context>(m_server.clientConfig().build());
    std::unique_ptr<sup::epics::PvAccessClientImpl> result{
        new sup::epics::PvAccessClientImpl(context, cb)};
    return result;
  }

  pvxs::Value m_pvxs_ntscalar_value;            //!< NTScalar containing int as a value
  pvxs::server::SharedPV m_shared_ntscalar_pv;  //!< Shared PV based on NTScalar

  pvxs::Value m_pvxs_string_value;  //!< Struct with a string as a value.
  pvxs::server::SharedPV
      m_shared_string_pv;  //!< Shared PV based on a struct with string value field

  pvxs::server::Server m_server;
};

//! Check initial state of PvAccessClient when no server is running, and no variables have been
//! added. Server was created (but not started) before the client.

TEST_F(PvAccessClientTest, InitialState)
{
  sup::epics::PvAccessClient client(CreateClientImpl());

  EXPECT_TRUE(client.GetVariableNames().empty());
  EXPECT_FALSE(client.IsConnected("non-existing-channel"));

  // getters and setters should throw for non-existing variables
  EXPECT_THROW(client.GetValue("non-existing-channel"), std::runtime_error);
  sup::dto::AnyValue any_value;
  EXPECT_THROW(client.SetValue("non-existing-channel", any_value), std::runtime_error);
}

//! Check AddVariable when no server is running. Server was created before the client.

TEST_F(PvAccessClientTest, AddVariableAndSetValueWhenUnconnected)
{
  sup::epics::PvAccessClient client(CreateClientImpl());

  // adding variables
  client.AddVariable("channel0");
  client.AddVariable("channel1");

  // it is not possible to add variable with the same name
  EXPECT_THROW(client.AddVariable("channel1"), std::runtime_error);

  EXPECT_EQ(client.GetVariableNames(), std::vector<std::string>({"channel0", "channel1"}));

  // values should be empty
  EXPECT_TRUE(::sup::dto::IsEmptyValue(client.GetValue("channel0")));
  EXPECT_TRUE(::sup::dto::IsEmptyValue(client.GetValue("channel1")));

  // it is not possible to set values for unconnected variables
  sup::dto::AnyValue any_value{sup::dto::SignedInteger32Type, 42};
  EXPECT_FALSE(client.SetValue("channel0", any_value));

  // we expect old values
  EXPECT_TRUE(::sup::dto::IsEmptyValue(client.GetValue("channel0")));
  EXPECT_TRUE(::sup::dto::IsEmptyValue(client.GetValue("channel1")));
}

//! Standard scenario. Server with two different variables was created and started before the
//! client. Two variables have been added to the client. Check connection status and GetValue,
//! SetValue functionality.

// FIXME

TEST_F(PvAccessClientTest, TwoDifferentChannels)
{
  // starting a server with two variables
  m_server.start();
  m_shared_ntscalar_pv.open(m_pvxs_ntscalar_value);
  m_shared_string_pv.open(m_pvxs_string_value);

  // creating a client with two variables
  sup::epics::PvAccessClient client(CreateClientImpl());
  client.AddVariable(kIntChannelName);
  client.AddVariable(kStringChannelName);

  // checking connection status
  EXPECT_TRUE(client.WaitForValidValue(kIntChannelName, 1.0));
  EXPECT_TRUE(client.WaitForValidValue(kStringChannelName, 1.0));

  // checking updated values
  auto any_value0 = client.GetValue(kIntChannelName);
  ASSERT_TRUE(any_value0.HasField("value"));
  EXPECT_EQ(any_value0["value"], kInitialIntChannelValue);

  // m_shared_string_pv is a `struct-scalar`, i.e. a struct with a single `value` field
  // PvAccessClient sees it as a simple scalar
  auto any_value1 = client.GetValue(kStringChannelName);
  ASSERT_TRUE(any_value1.HasField("value"));
  EXPECT_EQ(any_value1["value"], kInitialStringChannelValue);

  // setting values
  any_value0["value"] = kInitialIntChannelValue + 1;
  EXPECT_TRUE(client.SetValue(kIntChannelName, any_value0));
  any_value1["value"] = std::string("abc2");
  EXPECT_TRUE(client.SetValue(kStringChannelName, any_value1));

  // checking values on server side
  EXPECT_TRUE(BusyWaitFor(1.0,
                          [this]()
                          {
                            auto shared_int_value = m_shared_ntscalar_pv.fetch();
                            return shared_int_value["value"].as<int>()
                                   == kInitialIntChannelValue + 1;
                          }));
  EXPECT_TRUE(BusyWaitFor(1.0,
                          [this]()
                          {
                            auto shared_string_value = m_shared_string_pv.fetch();
                            return shared_string_value["value"].as<std::string>()
                                   == std::string("abc2");
                          }));
}

//! Standard scenario. Server with two different variables was created and started before
//! clients. Two clients with single variable each are started after. One client sets the value,
//! checking updated value from second client. Both clients have callbacks.

TEST_F(PvAccessClientTest, TwoClients)
{
  MockListener listener1;
  MockListener listener2;

  // starting a server with two variables
  m_server.start();
  m_shared_ntscalar_pv.open(m_pvxs_ntscalar_value);
  m_shared_string_pv.open(m_pvxs_string_value);

  // callback expectation on variable connection, first value and value change
  // expectations are cleared before destruction to ensure that the disconnect callback is
  // not triggered
  EXPECT_CALL(listener1, OnClientValueChanged(kIntChannelName, _)).Times(3);
  EXPECT_CALL(listener2, OnClientValueChanged(kIntChannelName, _)).Times(3);

  sup::epics::PvAccessClient client0(CreateClientImpl(listener1.GetClientCallBack()));
  client0.AddVariable(kIntChannelName);

  sup::epics::PvAccessClient client1(CreateClientImpl(listener2.GetClientCallBack()));
  client1.AddVariable(kIntChannelName);

  // checking connection status
  EXPECT_TRUE(client0.WaitForValidValue(kIntChannelName, 1.0));
  EXPECT_TRUE(client1.WaitForValidValue(kIntChannelName, 1.0));

  // retrieving value through first client
  auto any_value = client0.GetValue(kIntChannelName);
  EXPECT_EQ(any_value["value"], kInitialIntChannelValue);

  // setting the value through the first client
  any_value["value"] = 45;

  EXPECT_TRUE(client0.SetValue(kIntChannelName, any_value));

  // checking the value through the second variable
  EXPECT_TRUE(BusyWaitFor(1.0,
                          [&client1]()
                          {
                            auto any_value_from_client1 = client1.GetValue(kIntChannelName);
                            return any_value_from_client1["value"] == 45;
                          }));

  // validating callbacks and clearing listeners
  testing::Mock::VerifyAndClearExpectations(&listener1);
  testing::Mock::VerifyAndClearExpectations(&listener2);
}
