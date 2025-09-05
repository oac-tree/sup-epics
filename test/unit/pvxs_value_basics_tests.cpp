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

#include <sup/epics/utils/pvxs_utils.h>

#include <gtest/gtest.h>
#include <pvxs/data.h>
#include <pvxs/nt.h>

//! Tests to understand how to construct PVXS values.

class PvxsValueBasicsTests : public ::testing::Test
{
};

//! Basic construction of PVXS values.

TEST_F(PvxsValueBasicsTests, PVXSValueBasics)
{
  // default constructed
  const pvxs::Value pvxs_default1;
  EXPECT_FALSE(pvxs_default1.valid());
  EXPECT_EQ(pvxs_default1.type(), ::pvxs::TypeCode::Null);

  const pvxs::Value pvxs_default2;
  EXPECT_TRUE(pvxs_default1.equalType(pvxs_default2));
  EXPECT_TRUE(pvxs_default1.equalInst(pvxs_default2));  // Shouldn't be false?

  // from int32
  pvxs::Value pvxs_int1 = pvxs::TypeDef(pvxs::TypeCode::Int32).create();
  pvxs_int1 = 42;
  EXPECT_TRUE(pvxs_int1.valid());
  EXPECT_EQ(pvxs_int1.as<int>(), 42);

  const pvxs::Value pvxs_int2 = pvxs::TypeDef(pvxs::TypeCode::Int32).create();
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

  const pvxs::Value pvxs_int2 = pvxs_int1;
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
  const pvxs::TypeDef type_def(pvxs::TypeCode::Struct, "simple_t",
                               {pvxs::Member(pvxs::TypeCode::Int32, "field")});

  auto value = type_def.create();
  EXPECT_EQ(value.id(), "simple_t");
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
  const pvxs::TypeDef field_type1 = ::pvxs::TypeDef(::pvxs::TypeCode::Int8);
  const pvxs::TypeDef field_type2 = ::pvxs::TypeDef(::pvxs::TypeCode::UInt8);
  pvxs_type_struct += {field_type1.as("first")};
  pvxs_type_struct += {field_type1.as("second")};

  auto pvxs_value = pvxs_type_struct.create();
  EXPECT_EQ(pvxs_value.type(), pvxs::TypeCode::Struct);
  EXPECT_EQ(sup::epics::GetMemberNames(pvxs_value), std::vector<std::string>({"first", "second"}));
}

//! Studying how to create TypeDef for the array of structures programmatically.
TEST_F(PvxsValueBasicsTests, CreateTypeDefForArrayOfStructs)
{
  auto pvxs_type_array = ::pvxs::TypeDef(::pvxs::TypeCode::StructA);

  // adding fields that will become fields of underlying structure
  const pvxs::TypeDef field_type1 = ::pvxs::TypeDef(::pvxs::TypeCode::Int8);
  const pvxs::TypeDef field_type2 = ::pvxs::TypeDef(::pvxs::TypeCode::UInt8);
  pvxs_type_array += {field_type1.as("first")};
  pvxs_type_array += {field_type2.as("second")};

  // validating via value creation
  auto pvxs_value = pvxs_type_array.create();

  // allocating space, populating with values
  ::pvxs::Value array_field(pvxs_value);
  ::pvxs::shared_array<::pvxs::Value> arr(2);

  arr[0] = array_field.allocMember();
  arr[0]["first"] = -42;
  arr[0]["second"] = 42;
  arr[1] = array_field.allocMember();
  arr[1]["first"] = -43;
  arr[1]["second"] = 43;
  array_field = arr.freeze().castTo<const void>();

  // reading it back back
  EXPECT_EQ(pvxs_value.type(), pvxs::TypeCode::StructA);

  EXPECT_TRUE(pvxs_value.id().empty());

  auto array_data = pvxs_value.as<pvxs::shared_array<const pvxs::Value>>();
  EXPECT_EQ(array_data.size(), 2);

  EXPECT_EQ(array_data[0].id(), std::string());
  EXPECT_EQ(array_data[1].id(), std::string());

  EXPECT_EQ(array_data[0]["second"].as<uint8_t>(), 42);
  EXPECT_EQ(array_data[0]["first"].as<int8_t>(), -42);
  EXPECT_EQ(array_data[0]["second"].as<uint8_t>(), 42);
  EXPECT_EQ(array_data[1]["first"].as<int8_t>(), -43);
  EXPECT_EQ(array_data[1]["second"].as<uint8_t>(), 43);

  EXPECT_EQ(array_data[0]["first"].type(), ::pvxs::TypeCode::Int8);
  EXPECT_EQ(array_data[0]["second"].type(), ::pvxs::TypeCode::UInt8);
  EXPECT_EQ(array_data[1]["first"].type(), ::pvxs::TypeCode::Int8);
  EXPECT_EQ(array_data[1]["second"].type(), ::pvxs::TypeCode::UInt8);
}

//! Test almost like before with the attempt to pass a name. It shows that the name passed to the
//! array appear as the name of struct. Arrays itself doesn't have a name in PVXS.
TEST_F(PvxsValueBasicsTests, CreateTypeDefForArrayOfStructsNamed)
{
  const std::string expected_array_name("array_of_struct");
  auto pvxs_type_array = ::pvxs::TypeDef(::pvxs::TypeCode::StructA, expected_array_name, {});

  // adding fields that will become fields of underlying structure
  const pvxs::TypeDef field_type1 = ::pvxs::TypeDef(::pvxs::TypeCode::Int8);
  const pvxs::TypeDef field_type2 = ::pvxs::TypeDef(::pvxs::TypeCode::UInt8);
  pvxs_type_array += {field_type1.as("first")};
  pvxs_type_array += {field_type1.as("second")};

  // validating via value creation
  auto pvxs_value = pvxs_type_array.create();

  // allocating space, populating with values
  ::pvxs::Value array_field(pvxs_value);
  ::pvxs::shared_array<::pvxs::Value> arr(2);

  arr[0] = array_field.allocMember();
  arr[0]["first"] = -42;
  arr[0]["second"] = 42;
  arr[1] = array_field.allocMember();
  arr[1]["first"] = -43;
  arr[1]["second"] = 43;
  array_field = arr.freeze().castTo<const void>();

  // reading it back back
  EXPECT_EQ(pvxs_value.type(), pvxs::TypeCode::StructA);

  // NOTE arrays doesn't have names in PVXS
  EXPECT_TRUE(pvxs_value.id().empty());

  auto array_data = pvxs_value.as<pvxs::shared_array<const pvxs::Value>>();
  EXPECT_EQ(array_data.size(), 2);

  // NOTE apparently the name of the array appeared as a name of a struct
  EXPECT_EQ(array_data[0].id(), expected_array_name);
  EXPECT_EQ(array_data[1].id(), expected_array_name);

  EXPECT_EQ(array_data[0]["first"].as<int32_t>(), -42);
  EXPECT_EQ(array_data[0]["second"].as<uint32_t>(), 42);
  EXPECT_EQ(array_data[1]["first"].as<int32_t>(), -43);
  EXPECT_EQ(array_data[1]["second"].as<uint32_t>(), 43);
}
