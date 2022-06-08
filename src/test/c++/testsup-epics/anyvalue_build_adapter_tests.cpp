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

#include <gtest/gtest.h>
#include <sup/dto/anytype_helper.h>
#include <sup/dto/anyvalue.h>
#include <sup/dto/anyvalue_helper.h>
#include <sup/epics/anyvalue_build_adapter.h>

using namespace ::sup::epics;

class AnyValueBuildAdapterTests : public ::testing::Test
{
};

TEST_F(AnyValueBuildAdapterTests, StructWithSingleField)
{
  sup::dto::AnyType any_type = {{"signed", {sup::dto::SignedInteger32Type}}};

  AnyValueBuildAdapter builder;

  builder.StartStruct();
  builder.Int32("signed", 42);
  builder.EndStruct();

  auto value = builder.MoveAnyValue();
  EXPECT_EQ(value.GetType(), any_type);
  EXPECT_TRUE(::sup::dto::IsStructValue(value));
}
