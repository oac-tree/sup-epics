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
#include <thread>

using msec = std::chrono::milliseconds;
using ::testing::_;

namespace
{
const int kInitialValue = 42;
const int kInitialStatus = 1;
const std::string kChannelName = "PVXS-TESTS:NTSCALAR";
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

//! Initial state of PVAccess.

TEST_F(PVAccessClientTest, InitialState)
{
  auto context = CreateSharedContext();

  sup::epics::PVAccessClient client(context);

  EXPECT_TRUE(client.GetVariableNames().empty());
}
