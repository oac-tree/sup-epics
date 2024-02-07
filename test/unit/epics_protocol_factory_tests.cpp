/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Walter Van Herck (IO)
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

#include <sup/epics/epics_protocol_factory.h>

#include <sup/dto/anyvalue.h>

#include <gtest/gtest.h>

using namespace sup::epics;

class EPICSProtocolFactoryTest : public ::testing::Test
{
protected:
  EPICSProtocolFactoryTest() = default;
  ~EPICSProtocolFactoryTest() = default;

  EPICSProtocolFactory m_factory;
};

TEST_F(EPICSProtocolFactoryTest, ChannelAccessPVWrapper)
{
  sup::dto::AnyValue config = {{
    { kProcessVariableClass, kChannelAccessClientClass },
    { kChannelName, "CA-TESTS:BOOL" },
    { kVariableType, R"RAW({"type":"bool"})RAW" }
  }};
  auto var = m_factory.CreateProcessVariable(config);
  EXPECT_TRUE(var->WaitForAvailable(2.0));
  auto info = var->GetValue(0.0);
  EXPECT_TRUE(info.first);
  EXPECT_EQ(info.second.GetType(), sup::dto::BooleanType);
}

TEST_F(EPICSProtocolFactoryTest, TwoChannelAccessPVWrappers)
{
  sup::dto::AnyValue config = {{
    { kProcessVariableClass, kChannelAccessClientClass },
    { kChannelName, "CA-TESTS:FLOAT" },
    { kVariableType, R"RAW({"type":"float64"})RAW" }
  }};
  auto var_1 = m_factory.CreateProcessVariable(config);
  auto var_2 = m_factory.CreateProcessVariable(config);
  EXPECT_TRUE(var_1->WaitForAvailable(2.0));
  EXPECT_TRUE(var_2->WaitForAvailable(2.0));
  sup::dto::AnyValue update{ sup::dto::Float64Type, 1.1 };
  EXPECT_TRUE(SetVariableValue(*var_1, update));
  EXPECT_TRUE(WaitForVariableValue(*var_1, update, 1.0));
  EXPECT_TRUE(WaitForVariableValue(*var_2, update, 1.0));

  // Add callback to var_2
  double cache = 0.0;
  auto callback = [&cache](const sup::dto::AnyValue& val, bool connected){
    if (connected && val.GetType() == sup::dto::Float64Type)
    {
      cache = val.As<sup::dto::float64>();
    }
  };
  EXPECT_TRUE(var_2->SetMonitorCallback(callback));
  sup::dto::float64 float64_update = -2.0;
  EXPECT_TRUE(SetVariableValue(*var_1, float64_update));
  EXPECT_TRUE(WaitForVariableValue(*var_1, float64_update, 1.0));
  EXPECT_TRUE(WaitForVariableValue(*var_2, float64_update, 1.0));
  EXPECT_EQ(cache, float64_update);
}
