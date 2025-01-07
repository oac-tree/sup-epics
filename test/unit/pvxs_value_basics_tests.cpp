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
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 *****************************************************************************/

#include <gtest/gtest.h>
#include <pvxs/data.h>
#include <pvxs/nt.h>
#include <sup/epics/utils/pvxs_utils.h>

//! Tests to understand how to construct PVXS values.

class PvxsValueBasicsTests : public ::testing::Test
{
};

//! Basic construction of PVXS values.

TEST_F(PvxsValueBasicsTests, PVXSValueBasics)
{
  // default constructed
  pvxs::Value pvxs_default1;
  EXPECT_FALSE(pvxs_default1.valid());
  EXPECT_EQ(pvxs_default1.type(), ::pvxs::TypeCode::Null);

  pvxs::Value pvxs_default2;
  EXPECT_TRUE(pvxs_default1.equalType(pvxs_default2));
  EXPECT_TRUE(pvxs_default1.equalInst(pvxs_default2));  // Shouldn't be false?

  // from int32
  pvxs::Value pvxs_int1 = pvxs::TypeDef(pvxs::TypeCode::Int32).create();
  pvxs_int1 = 42;
  EXPECT_TRUE(pvxs_int1.valid());
  EXPECT_EQ(pvxs_int1.as<int>(), 42);

  pvxs::Value pvxs_int2 = pvxs::TypeDef(pvxs::TypeCode::Int32).create();
  EXPECT_TRUE(pvxs_int1.equalType(pvxs_int2));
  EXPECT_FALSE(pvxs_int1.equalInst(pvxs_int2));
}

//! Studying how to assign to PVXS value.

TEST_F(PvxsValueBasicsTests, PVXSValueBasicsAssignToScalar)
{
  pvxs::Value pvxs_int1 = pvxs::TypeDef(pvxs::TypeCode::Int32).create();
  pvxs_int1 = 42;
  EXPECT_TRUE(pvxs_int1.valid());
  EXPECT_EQ(pvxs_int1.as<int>(), 42);

  pvxs::Value pvxs_int2 = pvxs_int1;
  EXPECT_EQ(pvxs_int2.as<int>(), 42);

  pvxs::Value pvxs_int3;
  pvxs_int3 = pvxs_int1;
  EXPECT_EQ(pvxs_int3.as<int>(), 42);

  pvxs_int3 = 45;
  EXPECT_EQ(pvxs_int1.as<int>(), 45);
}

//! Studying how to assign to PVXS struct.

TEST_F(PvxsValueBasicsTests, PVXSValueBasicsAssignToStruct)
{
  pvxs::TypeDef type_def(pvxs::TypeCode::Struct, "simple_t",
                         {pvxs::Member(pvxs::TypeCode::Int32, "field")});

  auto value = type_def.create();
  value["field"] = 42;

  EXPECT_EQ(value["field"].as<int32_t>(), 42);

  auto field_value = value["field"];  // copy
  field_value = 43;
  EXPECT_EQ(value["field"].as<int32_t>(), 43);  // seems there is an implicit sharing inside
}

//! Studying what happen if we assign scalar to struct.

TEST_F(PvxsValueBasicsTests, PVXSValueBasicsAttemptToAssign)
{
  auto struct_value = pvxs::TypeDef(pvxs::TypeCode::Struct, "simple_t",
                                    {pvxs::Member(pvxs::TypeCode::Int32, "field")})
                          .create();

  struct_value["field"] = 42;

  auto scalar_value = pvxs::TypeDef(pvxs::TypeCode::Int32).create();
  scalar_value = 43;

  struct_value = scalar_value;

  // so, assignment simply changes the type
  EXPECT_EQ(struct_value.type(), ::pvxs::TypeCode::Int32);
  EXPECT_EQ(struct_value.as<int>(), 43);
}

//! Studying how to construct and populate array of structures.

TEST_F(PvxsValueBasicsTests, ArrayWithTwoStructureElements)
{
  // building PVXS value representing an array of structs with two elements
  auto pvxs_value = ::pvxs::TypeDef(::pvxs::TypeCode::StructA, "struct_name",
                                    {pvxs::members::Int32("field_name")})
                        .create();

  ::pvxs::Value array_field(pvxs_value);
  ::pvxs::shared_array<::pvxs::Value> arr(2);

  arr[0] = array_field.allocMember();
  arr[0]["field_name"] = 42;
  arr[1] = array_field.allocMember();
  arr[1]["field_name"] = 43;
  array_field = arr.freeze().castTo<const void>();

  // learning how read it back
  EXPECT_EQ(pvxs_value.type(), pvxs::TypeCode::StructA);
  // arrays doesn't have names in PVXS
  EXPECT_TRUE(pvxs_value.id().empty());

  auto array_data = pvxs_value.as<pvxs::shared_array<const pvxs::Value>>();
  EXPECT_EQ(array_data.size(), 2);
  EXPECT_EQ(array_data[0]["field_name"].as<int32_t>(), 42);
  EXPECT_EQ(array_data[1]["field_name"].as<int32_t>(), 43);
}

//! Studying how to create TypeDef for structure programmatically.

TEST_F(PvxsValueBasicsTests, CreateTypeDefForStruct)
{
  // creating type_def for structure without specifying fields
  auto pvxs_type_struct = ::pvxs::TypeDef(::pvxs::TypeCode::Struct, "struct_name", {});

  // adding fields
  pvxs::TypeDef field_type1 = ::pvxs::TypeDef(::pvxs::TypeCode::Int8);
  pvxs::TypeDef field_type2 = ::pvxs::TypeDef(::pvxs::TypeCode::UInt8);
  pvxs_type_struct += {field_type1.as("first")};
  pvxs_type_struct += {field_type1.as("second")};

  auto pvxs_value = pvxs_type_struct.create();
  EXPECT_EQ(pvxs_value.type(), pvxs::TypeCode::Struct);
  EXPECT_EQ(sup::epics::GetMemberNames(pvxs_value), std::vector<std::string>({"first", "second"}));
}

//! Studying how to create TypeDef for the array of structures programmatically.

TEST_F(PvxsValueBasicsTests, CreateTypeDefForArrayOfStructs)
{
  auto pvxs_type_struct =
      ::pvxs::TypeDef(::pvxs::TypeCode::Struct, "struct_name",
                      {pvxs::members::Int8("first"), pvxs::members::UInt8("second")});

  auto pvxs_type_array =
      ::pvxs::TypeDef(::pvxs::TypeCode::StructA);

  // for the moment it is not clear how to construct type for StructA in step-wise manner
  // TODO continue
}
