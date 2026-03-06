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

#include <gtest/gtest.h>
#include <pvxs/data.h>

class PVXSValueTests : public ::testing::Test
{
};

// Tests demonstrating raw PVXS assign behavior with empty struct members.

TEST_F(PVXSValueTests, AssignStructWithEmptyStructMember)
{
  // Target: struct with int "value" and struct "nested" with 2 scalar members
  auto target =
      ::pvxs::TypeDef(::pvxs::TypeCode::Struct, "root_t",
                      {pvxs::members::Struct("nested", "nested_t",
                                             {pvxs::members::Int32("a"),
                                              pvxs::members::Float64("b")}),
                       pvxs::members::Int32("value")})
          .create();

  // Source: struct with struct "nested" without members and int "value"
  auto source =
      ::pvxs::TypeDef(::pvxs::TypeCode::Struct, "root_t",
                      {pvxs::members::Struct("nested", "nested_t", {}),
                       pvxs::members::Int32("value")})
          .create();
  source["value"] = 42;
  // "nested" is present but has no members.

  // Verify source nested has no members
  EXPECT_EQ(source["nested"].nmembers(), 0u);

  // Verify all nodes in the target are unmarked before assignment:
  EXPECT_FALSE(target.isMarked(false));
  EXPECT_FALSE(target["nested"].isMarked(false));
  EXPECT_FALSE(target["nested"]["a"].isMarked(false));
  EXPECT_FALSE(target["nested"]["b"].isMarked(false));
  EXPECT_FALSE(target["value"].isMarked(false));

  // Assign source to target
  target.assign(source);

  // The top structure is not marked
  EXPECT_FALSE(target.isMarked(false));

  // The integer field should be copied and marked
  EXPECT_TRUE(target["value"].isMarked(false));
  EXPECT_EQ(target["value"].as<int32_t>(), 42);

  // The nested struct should be left unmarked since the source was an empty struct
  EXPECT_FALSE(target["nested"].isMarked(false));
  EXPECT_FALSE(target["nested"]["a"].isMarked(false));
  EXPECT_FALSE(target["nested"]["b"].isMarked(false));
}

TEST_F(PVXSValueTests, AssignStructWithEmptyArrayMember)
{
  // Target: struct with int "value" and Int32 array "arr" that has elements
  auto target =
      ::pvxs::TypeDef(::pvxs::TypeCode::Struct, "root_t",
                      {pvxs::members::Int32("value"),
                       pvxs::members::Int32A("arr")})
          .create();
  target["value"] = 10;
  target["arr"] = pvxs::shared_array<const int32_t>({1, 2, 3});
  target.unmark();

  // Source: struct with int "value" and Int32 array "arr" that is empty
  auto source =
      ::pvxs::TypeDef(::pvxs::TypeCode::Struct, "root_t",
                      {pvxs::members::Int32("value"),
                       pvxs::members::Int32A("arr")})
          .create();
  source["value"] = 99;
  // "arr" is left unset/unmarked (empty array of the right type)

  // Verify source arr is unmarked
  EXPECT_FALSE(source["arr"].isMarked(false));

  // The array should be unmarked before assignment
  EXPECT_FALSE(target["arr"].isMarked(false));

  // Assign source to target
  target.assign(source);

  // The integer field should be copied and marked
  EXPECT_TRUE(target["value"].isMarked(false));
  EXPECT_EQ(target["value"].as<int32_t>(), 99);

  // The array should be unmarked since source array was unmarked
  EXPECT_FALSE(target["arr"].isMarked(false));
}
