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

#include "sup/epics/utils.h"

#include "AnyValue.h"
#include <pvxs/data.h>
#include <pvxs/nt.h>

#include <gtest/gtest.h>

using namespace ::sup::epics;

class UtilsTest : public ::testing::Test
{
};

//! Testing Insert method.
TEST_F(UtilsTest, GetHelloWorld)
{
  EXPECT_EQ(GetHelloWorldString(), std::string("Hello World!"));

  sup::dto::AnyValue anyvalue{sup::dto::Boolean};
  anyvalue = true;
}
