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
#include "softioc_utils.h"
#include "unit_test_helper.h"

#include <gtest/gtest.h>
#include <pvxs/server.h>
#include <pvxs/sharedpv.h>
#include <sup/dto/anyvalue.h>
#include <sup/epics/pvxs/pv_access_server_pv.h>

using ::testing::_;
using sup::epics::unit_test_helper::BusyWaitFor;

namespace
{
std::unique_ptr<pvxs::server::Server> CreateIsolatedServer();
std::unique_ptr<pvxs::server::Server> CreateServerFromEnv();
}  // unnamed namespace

class PvAccessServerPVTests : public ::testing::Test
{
};

//! Check initial state of PvAccessServerPV.
//! There is no PVXS server running.

TEST_F(PvAccessServerPVTests, InitialState)
{
  const std::string variable_name{"variable_name"};

  // Validate that we can get original AnyValue used during the construction.
  // This checks that implicit scalar->struct conversion is working.

  {  // variable is based on scalar AnyValue
    sup::dto::AnyValue any_value{sup::dto::SignedInteger32Type, 42};
    EXPECT_THROW(sup::epics::PvAccessServerPV variable(variable_name, any_value, {}),
                 std::runtime_error);
  }

  {  // variable is based on struct AnyValue
    sup::dto::AnyValue any_value = {{"signed", {sup::dto::SignedInteger32Type, 42}},
                                    {"bool", {sup::dto::BooleanType, true}}};

    sup::epics::PvAccessServerPV variable(variable_name, any_value, {});
    EXPECT_EQ(variable.GetVariableName(), variable_name);
    EXPECT_EQ(variable.GetValue(), any_value);
  }
}

//! Check GetValue and SetValue. There is no PVXS server running.

TEST_F(PvAccessServerPVTests, GetAndSet)
{
  const std::string variable_name{"variable_name"};

  // creating variable based on scalar
  sup::dto::AnyValue any_value({
    {"value", {sup::dto::SignedInteger32Type, 42}}
  });
  sup::epics::PvAccessServerPV variable(variable_name, any_value, {});
  EXPECT_EQ(variable.GetValue(), any_value);

  // setting new value and checking result
  sup::dto::AnyValue new_any_value({
    {"value", {sup::dto::SignedInteger32Type, 45}}
  });
  EXPECT_TRUE(variable.SetValue(new_any_value));

  // attempt to set value with different type
  sup::dto::AnyValue struct_value = {{"signed", {sup::dto::SignedInteger32Type, 42}},
                                     {"bool", {sup::dto::BooleanType, true}}};
  EXPECT_THROW(variable.SetValue(struct_value), sup::dto::InvalidConversionException);
}

//! Check GetValue and SetValue. Server is running.

TEST_F(PvAccessServerPVTests, GetAndSetForIsolatedServer)
{
  auto server = CreateIsolatedServer();

  const std::string variable_name{"variable_name"};

  // creating variable based on scalar
  sup::dto::AnyValue any_value({
    {"value", {sup::dto::SignedInteger32Type, 42}}
  });
  sup::epics::PvAccessServerPV variable(variable_name, any_value, {});
  EXPECT_EQ(variable.GetValue(), any_value);

  variable.AddToServer(*server);

  // setting new value and checking the result
  sup::dto::AnyValue new_any_value({
    {"value", {sup::dto::SignedInteger32Type, 45}}
  });
  EXPECT_TRUE(variable.SetValue(new_any_value));
  EXPECT_TRUE(BusyWaitFor(1.0, [&](){
    return variable.GetValue() == new_any_value;
  }));
}

//! Check GetValue and SetValue. Server is running.
//! Same as above, callbacks are added.

TEST_F(PvAccessServerPVTests, GetAndSetForIsolatedServerWithCallbacks)
{
  MockListener listener;

  auto server = CreateIsolatedServer();

  const std::string variable_name{"variable_name"};

  // creating variable based on scalar
  sup::dto::AnyValue any_value({
    {"value", {sup::dto::SignedInteger32Type, 42}}
  });
  sup::epics::PvAccessServerPV variable(variable_name, any_value, listener.GetServerPvCallBack());
  EXPECT_EQ(variable.GetValue(), any_value);

  variable.AddToServer(*server);

  // setting new value and checking the result
  sup::dto::AnyValue new_any_value({
    {"value", {sup::dto::SignedInteger32Type, 45}}
  });
  // setting up callback expectations
  EXPECT_CALL(listener, OnServerPVValueChanged(new_any_value)).Times(1);

  EXPECT_TRUE(variable.SetValue(new_any_value));

  EXPECT_TRUE(BusyWaitFor(1.0, [&](){
    return variable.GetValue() == new_any_value;
  }));
}

//! Adding variable to a server. Server is started first.

TEST_F(PvAccessServerPVTests, AddToServerAfterServerStart)
{
  auto server = CreateServerFromEnv(); // to make 'pvget` working
  server->start();

  const std::string variable_name{"variable_name"};

  sup::dto::AnyValue any_value({
    {"value", {sup::dto::SignedInteger32Type, 42}}
  });
  sup::epics::PvAccessServerPV variable(variable_name, any_value, {});

  variable.AddToServer(*server);

  // validating variable using `pvget` command line utility
  EXPECT_TRUE(BusyWaitFor(1.0, [&](){
    auto pvget_output = GetPvGetOutput(variable_name);
    auto varname_found = pvget_output.find(variable_name) != std::string::npos;
    auto value_found = pvget_output.find("int value 42") != std::string::npos;
    return varname_found && value_found;
  }));
}

//! Adding variable to a server, then start the server.

TEST_F(PvAccessServerPVTests, AddToServerBeforeServerStart)
{
  auto server = CreateServerFromEnv(); // to make 'pvget` working

  const std::string variable_name{"variable_name"};

  sup::dto::AnyValue any_value({
    {"value", {sup::dto::SignedInteger32Type, 42}}
  });
  sup::epics::PvAccessServerPV variable(variable_name, any_value, {});

  variable.AddToServer(*server);

  server->start();

  // validating variable using `pvget`
  EXPECT_TRUE(BusyWaitFor(1.0, [&](){
    auto pvget_output = GetPvGetOutput(variable_name);
    auto varname_found = pvget_output.find(variable_name) != std::string::npos;
    auto value_found = pvget_output.find("int value 42") != std::string::npos;
    return varname_found && value_found;
  }));
}

//! Adding variable to a server, then start the server.
//! Validating variable change on external `pvput`.

TEST_F(PvAccessServerPVTests, GetAfterPvPut)
{
  auto server = CreateServerFromEnv(); // to make 'pvget` working

  const std::string variable_name{"variable_name"};

  sup::dto::AnyValue any_value({
    {"value", {sup::dto::SignedInteger32Type, 42}}
  });
  sup::epics::PvAccessServerPV variable(variable_name, any_value, {});

  variable.AddToServer(*server);

  server->start();

  // validating variable using `pvget`
  EXPECT_TRUE(BusyWaitFor(1.0, [&](){
    auto pvget_output = GetPvGetOutput(variable_name);
    auto varname_found = pvget_output.find(variable_name) != std::string::npos;
    auto value_found = pvget_output.find("int value 42") != std::string::npos;
    return varname_found && value_found;
  }));

  // changing the value via `pvput`
  auto pvput_output = PvPut(variable_name, R"RAW("value"=4321)RAW");

  // validating variable cache
  sup::dto::AnyValue expected_any_value({
    {"value", {sup::dto::SignedInteger32Type, 4321}}
  });
  EXPECT_TRUE(BusyWaitFor(1.0, [&](){
    return variable.GetValue() == expected_any_value;
  }));
}

//! Adding variable to a server, then start the server.
//! Validating variable change on external `pvput`.
//! Checking callbacks.

TEST_F(PvAccessServerPVTests, GetAfterPvPutWithCallback)
{
  auto server = CreateServerFromEnv(); // to make 'pvget` working

  MockListener listener;

  const std::string variable_name{"variable_name"};

  sup::dto::AnyValue any_value({
    {"value", {sup::dto::SignedInteger32Type, 42}}
  });
  sup::epics::PvAccessServerPV variable(variable_name, any_value, listener.GetServerPvCallBack());

  variable.AddToServer(*server);

  server->start();

  // validating variable using `pvget`
  EXPECT_TRUE(BusyWaitFor(1.0, [&](){
    auto pvget_output = GetPvGetOutput(variable_name);
    auto varname_found = pvget_output.find(variable_name) != std::string::npos;
    auto value_found = pvget_output.find("int value 42") != std::string::npos;
    return varname_found && value_found;
  }));

  // setting up callback expectations
  sup::dto::AnyValue expected_any_value({
    {"value", {sup::dto::SignedInteger32Type, 4321}}
  });
  EXPECT_CALL(listener, OnServerPVValueChanged(expected_any_value)).Times(1);

  // changing the value via `pvput`
  auto pvput_output = PvPut(variable_name, R"RAW("value"=4321)RAW");

  // validating variable cache
  EXPECT_TRUE(BusyWaitFor(1.0, [&](){
    return variable.GetValue() == expected_any_value;
  }));
}

namespace
{
std::unique_ptr<pvxs::server::Server> CreateIsolatedServer()
{
  return std::unique_ptr<pvxs::server::Server>(
      new pvxs::server::Server(pvxs::server::Config::isolated()));
}

std::unique_ptr<pvxs::server::Server> CreateServerFromEnv()
{
  return std::unique_ptr<pvxs::server::Server>(
        new pvxs::server::Server(pvxs::server::Config::fromEnv()));
}
}  // unnamed namespace
