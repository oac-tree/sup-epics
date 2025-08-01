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

#include <gtest/gtest.h>

#include <sup/epics/epics_protocol_factory.h>

class PvAccessLoggingClientServerTests : public ::testing::Test
{
public:

  PvAccessLoggingClientServerTests() = default;
  ~PvAccessLoggingClientServerTests() override = default;
};

//! Initial state of PvAccessClientPV when no server exists.

TEST_F(PvAccessLoggingClientServerTests, ScalarPayload)
{
  EXPECT_TRUE(true);
}

