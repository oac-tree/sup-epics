/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2026 ITER Organization,
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

#include <sup/epics/utils/dto_conversion_utils.h>

#include <sup/dto/anyvalue.h>

#include <pvxs/data.h>

#include <gtest/gtest.h>

class PVXSPartialAssignTests : public ::testing::Test
{
};

TEST_F(PVXSPartialAssignTests, AssignScalarStructWithPartialStruct)
{
  // Target: struct with 3 scalar members
  auto target =
      ::pvxs::TypeDef(::pvxs::TypeCode::Struct, "root_t",
                      {pvxs::members::Float64("a"),
                       pvxs::members::UInt16("b"),
                       pvxs::members::Int32("c")})
          .create();

  // Update: struct with only middle 'b' member
  sup::dto::AnyValue update = {{
    { "b", { sup::dto::UnsignedInteger16Type, 42U} }
  }};
  auto pvxs_update = sup::epics::BuildPVXSValue(update);

  // Verify all nodes in the target are unmarked before assignment:
  EXPECT_FALSE(target.isMarked(false));
  EXPECT_FALSE(target["a"].isMarked(false));
  EXPECT_FALSE(target["b"].isMarked(false));
  EXPECT_FALSE(target["c"].isMarked(false));

  // Assign pvxs_update to target
  target.assign(pvxs_update);

  // The top structure is still not marked
  EXPECT_FALSE(target.isMarked(false));

  // The integer field should be copied and marked
  EXPECT_TRUE(target["b"].isMarked(false));
  EXPECT_EQ(target["b"].as<u_int16_t>(), 42);

  // The other scalar fields are unmarked
  EXPECT_FALSE(target["a"].isMarked(false));
  EXPECT_FALSE(target["c"].isMarked(false));
}

TEST_F(PVXSPartialAssignTests, AssignMixedStructWithPartialStruct)
{
  // Target: struct with int "value" and struct "nested" with 2 scalar members
  auto target =
      ::pvxs::TypeDef(::pvxs::TypeCode::Struct, "root_t",
                      {pvxs::members::Struct("nested", "nested_t",
                                             {pvxs::members::Int32("a"),
                                              pvxs::members::Float64("b")}),
                       pvxs::members::Int32("value")})
          .create();

  sup::dto::AnyValue update = {{
    { "nested", sup::dto::EmptyStruct() },
    { "value", { sup::dto::SignedInteger32Type, 42} }
  }};
  auto pvxs_update = sup::epics::BuildPVXSValue(update);

  // Verify pvxs_update nested has no members
  EXPECT_EQ(pvxs_update["nested"].nmembers(), 0u);

  // Verify all nodes in the target are unmarked before assignment:
  EXPECT_FALSE(target.isMarked(false));
  EXPECT_FALSE(target["nested"].isMarked(false));
  EXPECT_FALSE(target["nested"]["a"].isMarked(false));
  EXPECT_FALSE(target["nested"]["b"].isMarked(false));
  EXPECT_FALSE(target["value"].isMarked(false));

  // Assign pvxs_update to target
  target.assign(pvxs_update);

  // The top structure is not marked
  EXPECT_FALSE(target.isMarked(false));

  // The integer field should be copied and marked
  EXPECT_TRUE(target["value"].isMarked(false));
  EXPECT_EQ(target["value"].as<int32_t>(), 42);

  // The nested struct should be left unmarked since the pvxs_update was an empty struct
  EXPECT_FALSE(target["nested"].isMarked(false));
  EXPECT_FALSE(target["nested"]["a"].isMarked(false));
  EXPECT_FALSE(target["nested"]["b"].isMarked(false));
}

TEST_F(PVXSPartialAssignTests, AssignStructStructWithPartialStruct)
{
  // Target: struct with int "value" and struct "nested" with 2 scalar members
  auto target =
      ::pvxs::TypeDef(::pvxs::TypeCode::Struct, "root_t",
                      {pvxs::members::Struct("sys1", "nested_t",
                                             {pvxs::members::Int32("a"),
                                              pvxs::members::Float64("b")}),
                       pvxs::members::Struct("sys2", "nested_t",
                                             {pvxs::members::Int32("a"),
                                              pvxs::members::Float64("b")})})
          .create();

  sup::dto::AnyValue update = {{
    { "sys2", {{
      { "b", { sup::dto::Float64Type, 10.0 }}
    }} }
  }};
  // sup::dto::AnyValue update = {{
  //   { "sys1", sup::dto::EmptyStruct() },
  //   { "sys2", {{
  //     { "b", { sup::dto::Float64Type, 10.0 }}
  //   }} }
  // }};
  auto pvxs_update = sup::epics::BuildPVXSValue(update);

  // Verify pvxs_update sys1 has no members
  EXPECT_EQ(pvxs_update["sys1"].nmembers(), 0u);

  // Verify all nodes in the target are unmarked before assignment:
  EXPECT_FALSE(target.isMarked(false));
  EXPECT_FALSE(target["sys1"].isMarked(false));
  EXPECT_FALSE(target["sys1"]["a"].isMarked(false));
  EXPECT_FALSE(target["sys1"]["b"].isMarked(false));
  EXPECT_FALSE(target["sys2"].isMarked(false));
  EXPECT_FALSE(target["sys2"]["a"].isMarked(false));
  EXPECT_FALSE(target["sys2"]["b"].isMarked(false));

  // Assign pvxs_update to target
  target.assign(pvxs_update);

  // The top structure is not marked
  EXPECT_FALSE(target.isMarked(false));

  // The integer field should be copied and marked
  EXPECT_TRUE(target["sys2"]["b"].isMarked(false));
  EXPECT_EQ(target["sys2"]["b"].as<double>(), 10.0);

  // All other nodes should be left unmarked
  EXPECT_FALSE(target["sys1"].isMarked(false));
  EXPECT_FALSE(target["sys1"]["a"].isMarked(false));
  EXPECT_FALSE(target["sys1"]["b"].isMarked(false));
  EXPECT_FALSE(target["sys2"].isMarked(false));
  EXPECT_FALSE(target["sys2"]["a"].isMarked(false));
}
