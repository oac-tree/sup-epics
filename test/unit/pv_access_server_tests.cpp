/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Gennady Pospelov (IO)
 *
 * Copyright (c) : 2010-2025 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 * SPDX-License-Identifier: MIT
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file LICENSE located in the top level directory
 * of the distribution package.
 *****************************************************************************/

#include "mock_utils.h"

#include <gtest/gtest.h>
#include <pvxs/server.h>
#include <sup/dto/anyvalue.h>
#include <sup/epics/pv_access_server.h>

#include <sup/epics-test/softioc_utils.h>
#include <sup/epics-test/unit_test_helper.h>

using sup::epics::test::BusyWaitFor;
using sup::epics::test::GetPvGetOutput;
using sup::epics::test::PvPut;
using ::testing::_;

using namespace sup::epics;

class PVAccessServerTests : public ::testing::Test
{
};

//! Initial state of the server.

TEST_F(PVAccessServerTests, InitialState)
{
  PvAccessServer server(PvAccessServer::Isolated);

  EXPECT_TRUE(server.GetVariableNames().empty());

  // getters and setters should throw for non-existing variables
  EXPECT_THROW(server.GetValue("non-existing-channel"), std::runtime_error);
  sup::dto::AnyValue any_value;
  EXPECT_THROW(server.SetValue("non-existing-channel", any_value), std::runtime_error);
}

//! Add variable and check get/set without starting the server.

TEST_F(PVAccessServerTests, AddVariableAndGetSetWithoutRunning)
{
  PvAccessServer server(PvAccessServer::Isolated);

  // adding the channel and checking the value
  sup::dto::AnyValue any_value0({{"value", {sup::dto::SignedInteger32Type, 42}}});
  server.AddVariable("channel0", any_value0);
  EXPECT_EQ(server.GetValue("channel0"), any_value0);

  // adding second channel and checking the value
  sup::dto::AnyValue any_value1 = {{"signed", {sup::dto::SignedInteger32Type, 42}},
                                   {"bool", {sup::dto::BooleanType, true}}};
  server.AddVariable("channel1", any_value1);
  EXPECT_EQ(server.GetValue("channel1"), any_value1);

  EXPECT_EQ(server.GetVariableNames(), std::vector<std::string>({"channel0", "channel1"}));

  // it is not possible to add channel with the same name
  EXPECT_THROW(server.AddVariable("channel1", any_value1), std::runtime_error);

  // changing first channel
  sup::dto::AnyValue new_any_value({{"value", {sup::dto::SignedInteger32Type, 45}}});
  EXPECT_TRUE(server.SetValue("channel0", new_any_value));
  EXPECT_EQ(server.GetValue("channel0"), new_any_value);
}

//! Standard scenario. Add single variable and start server.
//! Check value via `pvget`, change value via `pvput` and check on server side.

TEST_F(PVAccessServerTests, GetAfterPvPut)
{
  const std::string variable_name{"PVAccessServerTests:GetAfterPvPut"};

  // creating from the environment config to be able to use `pvget` and `pvput`
  PvAccessServer server;

  sup::dto::AnyValue any_value({{"value", {sup::dto::SignedInteger32Type, 42}}});
  server.AddVariable(variable_name, any_value);

  server.Start();

  // validating variable using `pvget`
  EXPECT_TRUE(BusyWaitFor(1.0,
                          [&]()
                          {
                            auto pvget_output = GetPvGetOutput(variable_name);
                            auto varname_found =
                                pvget_output.find(variable_name) != std::string::npos;
                            auto value_found =
                                pvget_output.find("int value 42") != std::string::npos;
                            return varname_found && value_found;
                          }));

  // changing the value via `pvput`
  auto pvput_output = PvPut(variable_name, R"RAW("value"=4321)RAW");

  // validating variable cache
  sup::dto::AnyValue expected_any_value({{"value", {sup::dto::SignedInteger32Type, 4321}}});
  EXPECT_TRUE(
      BusyWaitFor(1.0, [&]() { return server.GetValue(variable_name) == expected_any_value; }));
}

//! Standard scenario. Add single variable and start server.
//! Check value via `pvget`, change value via `pvput` and check on server side.

TEST_F(PVAccessServerTests, GetAfterPvPutWithCallbacks)
{
  MockListener listener;

  const std::string variable_name{"PVAccessServerTests:GetAfterPvPut"};

  // creating from the environment config to be able to use `pvget` and `pvput`
  PvAccessServer server(listener.GetServerCallBack());

  sup::dto::AnyValue any_value({{"value", {sup::dto::SignedInteger32Type, 42}}});
  server.AddVariable(variable_name, any_value);
  server.AddVariable("another-variable-name", any_value);

  server.Start();

  // validating variable using `pvget`
  EXPECT_TRUE(BusyWaitFor(1.0,
                          [&]()
                          {
                            auto pvget_output = GetPvGetOutput(variable_name);
                            auto varname_found =
                                pvget_output.find(variable_name) != std::string::npos;
                            auto value_found =
                                pvget_output.find("int value 42") != std::string::npos;
                            return varname_found && value_found;
                          }));

  // setting up callback expectations
  sup::dto::AnyValue expected_any_value({{"value", {sup::dto::SignedInteger32Type, 4321}}});
  EXPECT_CALL(listener, OnServerValueChanged(variable_name, expected_any_value)).Times(1);

  // changing the value via `pvput`
  auto pvput_output = PvPut(variable_name, R"RAW("value"=4321)RAW");

  // validating variable cache
  EXPECT_TRUE(
      BusyWaitFor(1.0, [&]() { return server.GetValue(variable_name) == expected_any_value; }));
}
