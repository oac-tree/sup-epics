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
