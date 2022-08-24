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
#include <pvxs/server.h>
#include <pvxs/sharedpv.h>
#include <sup/dto/anyvalue.h>
#include <sup/epics/pvxs/pv_access_server_variable.h>

using msec = std::chrono::milliseconds;
using ::testing::_;

class PVAccessServerVariableTests : public ::testing::Test
{
public:
  PVAccessServerVariableTests() : m_server(pvxs::server::Config::isolated().build()) {}

  pvxs::server::Server m_server;
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
  EXPECT_THROW(variable.SetValue(struct_value), std::runtime_error);
}

//! Adding variable to a running server.

TEST_F(PVAccessServerVariableTests, AddToServer)
{
  const std::string variable_name{"variable_name"};

  sup::dto::AnyValue any_value{sup::dto::SignedInteger32Type, 42};
  sup::epics::PVAccessServerVariable variable(variable_name, any_value, {});

  m_server.start();

  variable.AddToServer(m_server);
}
