/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP common components for EPICS network protocol
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

#include "AnyValue.h"
#include "sup/epics/utils.h"

#include <gtest/gtest.h>
#include <pvxs/data.h>
#include <pvxs/nt.h>

using namespace ::sup::epics;

class UtilsTest : public ::testing::Test
{
};

//! Get PVXS value from empty AnyValue.

TEST_F(UtilsTest, GetPVXSValueFromEmpty)
{
  sup::dto::AnyValue any_value;
  auto pvxs_value = GetPVXSValue(any_value);
  EXPECT_FALSE(pvxs_value.valid());
}
