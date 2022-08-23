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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <pvxs/client.h>
#include <pvxs/data.h>
#include <pvxs/nt.h>
#include <pvxs/server.h>
#include <pvxs/sharedpv.h>
#include <sup/dto/anytype.h>
#include <sup/dto/anyvalue.h>
#include <sup/epics/dto_conversion_utils.h>
#include <sup/epics/pv_access_client.h>

#include <memory>
#include <stdexcept>
#include <thread>

using msec = std::chrono::milliseconds;
using ::testing::_;

namespace
{
const int kInitialIntChannelValue = 42;
const int kInitialIntChannelStatus = 1;
const std::string kIntChannelName = "PVXS-TESTS:INTSCALAR";

const std::string kInitialStringChannelValue = "abc";
const std::string kStringChannelName = "PVXS-TESTS:STRING";
}  // namespace

//! Mock class to listen for callbacks.
class MockClientListener
{
public:
  sup::epics::PVAccessClient::callback_t GetCallBack()
  {
    return [this](const std::string& name, const sup::dto::AnyValue& value)
    { OnValueChanged(name, value); };
  }

  MOCK_METHOD2(OnValueChanged, void(const std::string& name, const sup::dto::AnyValue& value));
};

class PVAccessClientTest : public ::testing::Test
{
public:
  using shared_context_t = std::shared_ptr<pvxs::client::Context>;

  PVAccessClientTest()
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

  //! Create PVXS context intended for sharing among multiple PVAccessClientVariable variables.
  shared_context_t CreateSharedContext()
  {
    return std::make_shared<pvxs::client::Context>(m_server.clientConfig().build());
  }

  pvxs::Value m_pvxs_ntscalar_value;            //!< NTScalar containing int as a value
  pvxs::server::SharedPV m_shared_ntscalar_pv;  //!< Shared PV based on NTScalar

  pvxs::Value m_pvxs_string_value;  //!< Struct with a string as a value.
  pvxs::server::SharedPV
      m_shared_string_pv;  //!< Shared PV based on a struct with string value field

  pvxs::server::Server m_server;
};

//! Check initial state of PVAccessClient when no server is running, and no variables have been
//! added. Server was created (but not started) before the client.

TEST_F(PVAccessClientTest, InitialState)
{
  auto context = CreateSharedContext();

  sup::epics::PVAccessClient client(context);

  EXPECT_TRUE(client.GetVariableNames().empty());
  EXPECT_FALSE(client.IsConnected("non-existing-channel"));

  // getters and setters should throw for non-existing variables
  EXPECT_THROW(client.GetValue("non-existing-channel"), std::runtime_error);
  sup::dto::AnyValue any_value;
  EXPECT_THROW(client.SetValue("non-existing-channel", any_value), std::runtime_error);
}

//! Check AddVariable when no server is running. Server was created before the client.

TEST_F(PVAccessClientTest, AddVariableAndSetValueWhenUnconnected)
{
  auto context = CreateSharedContext();

  sup::epics::PVAccessClient client(context);

  // adding variables
  client.AddVariable("channel0");
  client.AddVariable("channel1");

  // it is not possible to add variable with the same name
  EXPECT_THROW(client.AddVariable("channel1"), std::runtime_error);

  EXPECT_EQ(client.GetVariableNames(), std::vector<std::string>({"channel0", "channel1"}));

  // values should be empty
  EXPECT_TRUE(::sup::dto::IsEmptyValue(client.GetValue("channel0")));
  EXPECT_TRUE(::sup::dto::IsEmptyValue(client.GetValue("channel1")));

  // it is possible to set values even for unconnected variables
  sup::dto::AnyValue any_value{sup::dto::SignedInteger32Type};
  any_value = 42;
  EXPECT_TRUE(client.SetValue("channel0", any_value));

  // one of the values is updated
  EXPECT_EQ(client.GetValue("channel0"), any_value);
  EXPECT_TRUE(::sup::dto::IsEmptyValue(client.GetValue("channel1")));
}

//! Standard scenario. Server with two different variables was created and started before the
//! client. Two variables have been added to the client. Check connection status and GetValue,
//! SetValue functionality.

TEST_F(PVAccessClientTest, TwoDifferentChannels)
{
  // starting a server with two variables
  m_server.start();
  m_shared_ntscalar_pv.open(m_pvxs_ntscalar_value);
  m_shared_string_pv.open(m_pvxs_string_value);

  // creating a client with two variables
  auto context = CreateSharedContext();
  sup::epics::PVAccessClient client(context);
  client.AddVariable(kIntChannelName);
  client.AddVariable(kStringChannelName);

  // checking connection status
  std::this_thread::sleep_for(msec(20));
  EXPECT_TRUE(client.IsConnected(kIntChannelName));
  EXPECT_TRUE(client.IsConnected(kStringChannelName));

  // checking updated values
  auto any_value0 = client.GetValue(kIntChannelName);
  EXPECT_EQ(any_value0["value"], kInitialIntChannelValue);
  auto any_value1 = client.GetValue(kStringChannelName);
  EXPECT_EQ(any_value1["value"], kInitialStringChannelValue);
}
