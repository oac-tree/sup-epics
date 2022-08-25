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

#include "softioc_utils.h"
#include "mock_utils.h"

#include <gtest/gtest.h>
#include <pvxs/server.h>
#include <pvxs/sharedpv.h>
#include <sup/dto/anyvalue.h>
#include <sup/epics/pvxs/pv_access_server_variable.h>
#include <sup/epics/pvxs/context_utils.h>

#include <thread>

using msec = std::chrono::milliseconds;
using ::testing::_;

class PVAccessServerVariableTests : public ::testing::Test
{
};

//! Check initial state of PVAccessServerVariable.
//! There is no PVXS server running.

TEST_F(PVAccessServerVariableTests, InitialState)
{
  const std::string variable_name{"variable_name"};

  // Validate that we can get original AnyValue used during the construction.
  // This checks that implicit scalar->struct conversion is working.

  {  // variable is based on scalar AnyValue
    sup::dto::AnyValue any_value{sup::dto::SignedInteger32Type, 42};
    sup::epics::PVAccessServerVariable variable(variable_name, any_value, {});
    EXPECT_EQ(variable.GetVariableName(), variable_name);
    EXPECT_EQ(variable.GetValue(), any_value);
  }

  {  // variable is based on struct AnyValue
    sup::dto::AnyValue any_value = {{"signed", {sup::dto::SignedInteger32Type, 42}},
                                    {"bool", {sup::dto::BooleanType, true}}};

    sup::epics::PVAccessServerVariable variable(variable_name, any_value, {});
    EXPECT_EQ(variable.GetVariableName(), variable_name);
    EXPECT_EQ(variable.GetValue(), any_value);
  }
}

//! Check GetValue and SetValue. There is no PVXS server running.

TEST_F(PVAccessServerVariableTests, GetAndSet)
{
  const std::string variable_name{"variable_name"};

  // creating variable based on scalar
  sup::dto::AnyValue any_value{sup::dto::SignedInteger32Type, 42};
  sup::epics::PVAccessServerVariable variable(variable_name, any_value, {});
  EXPECT_EQ(variable.GetValue(), any_value);

  // setting new value and checking result
  sup::dto::AnyValue new_any_value{sup::dto::SignedInteger32Type, 45};
  EXPECT_TRUE(variable.SetValue(new_any_value));

  // attempt to set value with different type
  sup::dto::AnyValue struct_value = {{"signed", {sup::dto::SignedInteger32Type, 42}},
                                     {"bool", {sup::dto::BooleanType, true}}};
  EXPECT_THROW(variable.SetValue(struct_value), sup::dto::InvalidConversionException);
}

//! Check GetValue and SetValue. Server is running.

TEST_F(PVAccessServerVariableTests, GetAndSetForIsolatedServer)
{
  auto server = sup::epics::CreateIsolatedServer();

  const std::string variable_name{"variable_name"};

  // creating variable based on scalar
  sup::dto::AnyValue any_value{sup::dto::SignedInteger32Type, 42};
  sup::epics::PVAccessServerVariable variable(variable_name, any_value, {});
  EXPECT_EQ(variable.GetValue(), any_value);

  variable.AddToServer(*server);
  std::this_thread::sleep_for(msec(20));

  // setting new value and checking the result
  sup::dto::AnyValue new_any_value{sup::dto::SignedInteger32Type, 45};
  EXPECT_TRUE(variable.SetValue(new_any_value));

  std::this_thread::sleep_for(msec(20));
  EXPECT_EQ(variable.GetValue(), new_any_value);
}

//! Adding variable to a server. Server is started first.

TEST_F(PVAccessServerVariableTests, AddToServerAfterServerStart)
{
  auto server = sup::epics::CreateServerFromEnv(); // to make 'pvget` working
  server->start();

  const std::string variable_name{"variable_name"};

  sup::dto::AnyValue any_value{sup::dto::SignedInteger32Type, 42};
  sup::epics::PVAccessServerVariable variable(variable_name, any_value, {});

  variable.AddToServer(*server);
  std::this_thread::sleep_for(msec(20));

  // validating variable using `pvget` command line utility
  auto pvget_output = GetPvGetOutput(variable_name);
  EXPECT_TRUE(pvget_output.find(variable_name) != std::string::npos);
  EXPECT_TRUE(pvget_output.find("int value 42") != std::string::npos);
}

//! Adding variable to a server, then start the server.

TEST_F(PVAccessServerVariableTests, AddToServerBeforeServerStart)
{
  auto server = sup::epics::CreateServerFromEnv(); // to make 'pvget` working

  const std::string variable_name{"variable_name"};

  sup::dto::AnyValue any_value{sup::dto::SignedInteger32Type, 42};
  sup::epics::PVAccessServerVariable variable(variable_name, any_value, {});

  variable.AddToServer(*server);

  server->start();
  std::this_thread::sleep_for(msec(20));

  // validating variable using `pvget`
  auto pvget_output = GetPvGetOutput(variable_name);
  EXPECT_TRUE(pvget_output.find(variable_name) != std::string::npos);
  EXPECT_TRUE(pvget_output.find("int value 42") != std::string::npos);
}

//! Adding variable to a server, then start the server.
//! Validating variable change on external `pvput`.

TEST_F(PVAccessServerVariableTests, GetAfterPvPut)
{
  auto server = sup::epics::CreateServerFromEnv(); // to make 'pvget` working

  const std::string variable_name{"variable_name"};

  sup::dto::AnyValue any_value{sup::dto::SignedInteger32Type, 42};
  sup::epics::PVAccessServerVariable variable(variable_name, any_value, {});

  variable.AddToServer(*server);

  server->start();
  std::this_thread::sleep_for(msec(20));

  // validating variable using `pvget`
  auto pvget_output = GetPvGetOutput(variable_name);
  EXPECT_TRUE(pvget_output.find(variable_name) != std::string::npos);
  EXPECT_TRUE(pvget_output.find("int value 42") != std::string::npos);

  // changing the value via `pvput`
  auto pvput_output = PvPut(variable_name, R"RAW("value"=4321)RAW");

  std::this_thread::sleep_for(msec(20));

  // validating variable cache
  sup::dto::AnyValue expected_any_value{sup::dto::SignedInteger32Type, 4321};
  EXPECT_EQ(variable.GetValue(), expected_any_value);
}

//! Adding variable to a server, then start the server.
//! Validating variable change on external `pvput`.
//! Checking callbacks.

TEST_F(PVAccessServerVariableTests, GetAfterPvPutWithCallback)
{
  auto server = sup::epics::CreateServerFromEnv(); // to make 'pvget` working

  MockListener listener;

  const std::string variable_name{"variable_name"};

  sup::dto::AnyValue any_value{sup::dto::SignedInteger32Type, 42};
  sup::epics::PVAccessServerVariable variable(variable_name, any_value, listener.GetCallBack());

  variable.AddToServer(*server);

  server->start();
  std::this_thread::sleep_for(msec(20));

  // validating variable using `pvget`
  auto pvget_output = GetPvGetOutput(variable_name);
  EXPECT_TRUE(pvget_output.find(variable_name) != std::string::npos);
  EXPECT_TRUE(pvget_output.find("int value 42") != std::string::npos);

  // setting up callback expectations
  sup::dto::AnyValue expected_any_value{sup::dto::SignedInteger32Type, 4321};
  EXPECT_CALL(listener, OnValueChanged(expected_any_value)).Times(1);

  // changing the value via `pvput`
  auto pvput_output = PvPut(variable_name, R"RAW("value"=4321)RAW");

  std::this_thread::sleep_for(msec(20));

  // validating variable cache
  EXPECT_EQ(variable.GetValue(), expected_any_value);
}
