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

#include "sup/epics/utils/pvxs_utils.h"

#include <gtest/gtest.h>
#include <pvxs/data.h>
#include <pvxs/nt.h>

#include <iostream>

using namespace ::sup::epics;

class PvxsUtilsTests : public ::testing::Test
{
};

TEST_F(PvxsUtilsTests, IsEmptyValue)
{
  pvxs::Value value;
  EXPECT_TRUE(IsEmptyValue(value));

  auto scalar_value = ::pvxs::TypeDef(::pvxs::TypeCode::Int16).create();
  EXPECT_FALSE(IsEmptyValue(scalar_value));
}

TEST_F(PvxsUtilsTests, IsScalar)
{
  using pvxs::TypeCode;
  using pvxs::TypeDef;

  EXPECT_TRUE(IsScalar(TypeDef(TypeCode::Bool).create()));
  EXPECT_TRUE(IsScalar(TypeDef(TypeCode::Int8).create()));
  EXPECT_TRUE(IsScalar(TypeDef(TypeCode::UInt8).create()));
  EXPECT_TRUE(IsScalar(TypeDef(TypeCode::Int16).create()));
  EXPECT_TRUE(IsScalar(TypeDef(TypeCode::UInt16).create()));
  EXPECT_TRUE(IsScalar(TypeDef(TypeCode::Int32).create()));
  EXPECT_TRUE(IsScalar(TypeDef(TypeCode::UInt32).create()));
  EXPECT_TRUE(IsScalar(TypeDef(TypeCode::Int64).create()));
  EXPECT_TRUE(IsScalar(TypeDef(TypeCode::UInt64).create()));
  EXPECT_TRUE(IsScalar(TypeDef(TypeCode::Float32).create()));
  EXPECT_TRUE(IsScalar(TypeDef(TypeCode::Float64).create()));
  EXPECT_TRUE(IsScalar(TypeDef(TypeCode::String).create()));

  EXPECT_FALSE(IsScalar(TypeDef(TypeCode::BoolA).create()));
  EXPECT_FALSE(IsScalar(TypeDef(TypeCode::Int8A).create()));
  EXPECT_FALSE(IsScalar(TypeDef(TypeCode::UInt8A).create()));
  EXPECT_FALSE(IsScalar(TypeDef(TypeCode::Int16A).create()));
  EXPECT_FALSE(IsScalar(TypeDef(TypeCode::UInt16A).create()));
  EXPECT_FALSE(IsScalar(TypeDef(TypeCode::Int32A).create()));
  EXPECT_FALSE(IsScalar(TypeDef(TypeCode::UInt32A).create()));
  EXPECT_FALSE(IsScalar(TypeDef(TypeCode::Int64A).create()));
  EXPECT_FALSE(IsScalar(TypeDef(TypeCode::UInt64A).create()));
  EXPECT_FALSE(IsScalar(TypeDef(TypeCode::Float32A).create()));
  EXPECT_FALSE(IsScalar(TypeDef(TypeCode::Float64A).create()));
  EXPECT_FALSE(IsScalar(TypeDef(TypeCode::StringA).create()));

  EXPECT_FALSE(IsScalar(TypeDef(TypeCode::Struct).create()));
  EXPECT_FALSE(IsScalar(TypeDef(TypeCode::StructA).create()));
  EXPECT_FALSE(IsScalar(TypeDef(TypeCode::Union).create()));
  EXPECT_FALSE(IsScalar(TypeDef(TypeCode::UnionA).create()));
}

TEST_F(PvxsUtilsTests, IsScalarArray)
{
  using pvxs::TypeCode;
  using pvxs::TypeDef;

  EXPECT_FALSE(IsScalarArray(TypeDef(TypeCode::Bool).create()));
  EXPECT_FALSE(IsScalarArray(TypeDef(TypeCode::Int8).create()));
  EXPECT_FALSE(IsScalarArray(TypeDef(TypeCode::UInt8).create()));
  EXPECT_FALSE(IsScalarArray(TypeDef(TypeCode::Int16).create()));
  EXPECT_FALSE(IsScalarArray(TypeDef(TypeCode::UInt16).create()));
  EXPECT_FALSE(IsScalarArray(TypeDef(TypeCode::Int32).create()));
  EXPECT_FALSE(IsScalarArray(TypeDef(TypeCode::UInt32).create()));
  EXPECT_FALSE(IsScalarArray(TypeDef(TypeCode::Int64).create()));
  EXPECT_FALSE(IsScalarArray(TypeDef(TypeCode::UInt64).create()));
  EXPECT_FALSE(IsScalarArray(TypeDef(TypeCode::Float32).create()));
  EXPECT_FALSE(IsScalarArray(TypeDef(TypeCode::Float64).create()));
  EXPECT_FALSE(IsScalarArray(TypeDef(TypeCode::String).create()));

  EXPECT_TRUE(IsScalarArray(TypeDef(TypeCode::BoolA).create()));
  EXPECT_TRUE(IsScalarArray(TypeDef(TypeCode::Int8A).create()));
  EXPECT_TRUE(IsScalarArray(TypeDef(TypeCode::UInt8A).create()));
  EXPECT_TRUE(IsScalarArray(TypeDef(TypeCode::Int16A).create()));
  EXPECT_TRUE(IsScalarArray(TypeDef(TypeCode::UInt16A).create()));
  EXPECT_TRUE(IsScalarArray(TypeDef(TypeCode::Int32A).create()));
  EXPECT_TRUE(IsScalarArray(TypeDef(TypeCode::UInt32A).create()));
  EXPECT_TRUE(IsScalarArray(TypeDef(TypeCode::Int64A).create()));
  EXPECT_TRUE(IsScalarArray(TypeDef(TypeCode::UInt64A).create()));
  EXPECT_TRUE(IsScalarArray(TypeDef(TypeCode::Float32A).create()));
  EXPECT_TRUE(IsScalarArray(TypeDef(TypeCode::Float64A).create()));
  EXPECT_TRUE(IsScalarArray(TypeDef(TypeCode::StringA).create()));

  EXPECT_FALSE(IsScalarArray(TypeDef(TypeCode::Struct).create()));
  EXPECT_FALSE(IsScalarArray(TypeDef(TypeCode::StructA).create()));
  EXPECT_FALSE(IsScalarArray(TypeDef(TypeCode::Union).create()));
  EXPECT_TRUE(IsScalarArray(TypeDef(TypeCode::UnionA).create()));
  EXPECT_FALSE(IsScalarArray(TypeDef(TypeCode::StructA).create()));

  // checking Null type separately, since it is not possible to create value from it
  EXPECT_FALSE(IsScalarArray(TypeCode::Null));
}

TEST_F(PvxsUtilsTests, IsStruct)
{
  using pvxs::TypeCode;
  using pvxs::TypeDef;

  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::Bool).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::Int8).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::UInt8).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::Int16).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::UInt16).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::Int32).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::UInt32).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::Int64).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::UInt64).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::Float32).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::Float64).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::String).create()));

  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::BoolA).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::Int8A).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::UInt8A).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::Int16A).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::UInt16A).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::Int32A).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::UInt32A).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::Int64A).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::UInt64A).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::Float32A).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::Float64A).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::StringA).create()));

  EXPECT_TRUE(IsStruct(TypeDef(TypeCode::Struct).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::StructA).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::Union).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::UnionA).create()));
}

TEST_F(PvxsUtilsTests, IsStructArray)
{
  using pvxs::TypeCode;
  using pvxs::TypeDef;

  EXPECT_FALSE(IsStructArray(TypeDef(TypeCode::Bool).create()));
  EXPECT_FALSE(IsStructArray(TypeDef(TypeCode::Int8).create()));
  EXPECT_FALSE(IsStructArray(TypeDef(TypeCode::UInt8).create()));
  EXPECT_FALSE(IsStructArray(TypeDef(TypeCode::Int16).create()));
  EXPECT_FALSE(IsStructArray(TypeDef(TypeCode::UInt16).create()));
  EXPECT_FALSE(IsStructArray(TypeDef(TypeCode::Int32).create()));
  EXPECT_FALSE(IsStructArray(TypeDef(TypeCode::UInt32).create()));
  EXPECT_FALSE(IsStructArray(TypeDef(TypeCode::Int64).create()));
  EXPECT_FALSE(IsStructArray(TypeDef(TypeCode::UInt64).create()));
  EXPECT_FALSE(IsStructArray(TypeDef(TypeCode::Float32).create()));
  EXPECT_FALSE(IsStructArray(TypeDef(TypeCode::Float64).create()));
  EXPECT_FALSE(IsStructArray(TypeDef(TypeCode::String).create()));

  EXPECT_FALSE(IsStructArray(TypeDef(TypeCode::BoolA).create()));
  EXPECT_FALSE(IsStructArray(TypeDef(TypeCode::Int8A).create()));
  EXPECT_FALSE(IsStructArray(TypeDef(TypeCode::UInt8A).create()));
  EXPECT_FALSE(IsStructArray(TypeDef(TypeCode::Int16A).create()));
  EXPECT_FALSE(IsStructArray(TypeDef(TypeCode::UInt16A).create()));
  EXPECT_FALSE(IsStructArray(TypeDef(TypeCode::Int32A).create()));
  EXPECT_FALSE(IsStructArray(TypeDef(TypeCode::UInt32A).create()));
  EXPECT_FALSE(IsStructArray(TypeDef(TypeCode::Int64A).create()));
  EXPECT_FALSE(IsStructArray(TypeDef(TypeCode::UInt64A).create()));
  EXPECT_FALSE(IsStructArray(TypeDef(TypeCode::Float32A).create()));
  EXPECT_FALSE(IsStructArray(TypeDef(TypeCode::Float64A).create()));
  EXPECT_FALSE(IsStructArray(TypeDef(TypeCode::StringA).create()));

  EXPECT_FALSE(IsStructArray(TypeDef(TypeCode::Struct).create()));
  EXPECT_TRUE(IsStructArray(TypeDef(TypeCode::StructA).create()));
  EXPECT_FALSE(IsStructArray(TypeDef(TypeCode::Union).create()));
  EXPECT_FALSE(IsStructArray(TypeDef(TypeCode::UnionA).create()));
}

//! Testing GetChildren() utility function for the case of scalars.

TEST_F(PvxsUtilsTests, GetChildrenForScalar)
{
  auto value = ::pvxs::TypeDef(::pvxs::TypeCode::Int16).create();
  EXPECT_TRUE(GetChildren(value).empty());
}

//! Testing GetChildren() utility function for structs.

TEST_F(PvxsUtilsTests, GetChildrenForSimpleStruct)
{
  auto value = pvxs::TypeDef(pvxs::TypeCode::Struct, "simple_t",
                             {pvxs::Member(pvxs::TypeCode::Int32, "field")})
                   .create();

  // setting through the parent
  value["field"] = 42;
  EXPECT_EQ(value["field"].as<int32_t>(), 42);

  // accessing child
  auto child = value["field"];
  EXPECT_EQ(child.as<int32_t>(), 42);

  // changing through the child
  child = 43;
  EXPECT_EQ(child.as<int32_t>(), 43);

  // through parent we see child has changed
  EXPECT_EQ(value["field"].as<int32_t>(), 43);

  // conclusion: PVXS has implicit sharing, all objects act like pointers

  // finally checking our convenience function to access children
  auto children = GetChildren(value);
  ASSERT_EQ(children.size(), 1);
  EXPECT_EQ(children[0].type(), ::pvxs::TypeCode::Int32);
  EXPECT_EQ(children[0].as<int32_t>(), 43);

  // changing the value
  children[0] = 44;

  // check that old objects have been updated
  EXPECT_EQ(value["field"].as<int32_t>(), 44);
  EXPECT_EQ(child.as<int32_t>(), 44);
}

//! Testing GetChildren() utility function for structs.

TEST_F(PvxsUtilsTests, GetChildrenForArrayOfStructs)
{
  // building PVXS value representing an array of structs with two elements
  auto pvxs_value =
      ::pvxs::TypeDef(::pvxs::TypeCode::StructA, "array_name", {pvxs::members::Int32("field_name")})
          .create();

  ::pvxs::Value array_field(pvxs_value);
  ::pvxs::shared_array<::pvxs::Value> arr(2);

  arr[0] = array_field.allocMember();
  arr[0]["field_name"] = 42;
  arr[1] = array_field.allocMember();
  arr[1]["field_name"] = 43;
  array_field = arr.freeze().castTo<const void>();

  // finally checking our convenience function to access children
  auto children = GetChildren(pvxs_value);
  ASSERT_EQ(children.size(), 2);
  EXPECT_EQ(children[0]["field_name"].as<int32_t>(), 42);
  EXPECT_EQ(children[1]["field_name"].as<int32_t>(), 43);
}

TEST_F(PvxsUtilsTests, GetMemberNames)
{
  {  // empty value
    pvxs::Value value;
    EXPECT_TRUE(GetMemberNames(value).empty());
  }

  // scalar
  {
    auto value = ::pvxs::TypeDef(::pvxs::TypeCode::Int16).create();
    EXPECT_TRUE(GetMemberNames(value).empty());
  }

  // struct with single field
  {
    auto value = pvxs::TypeDef(pvxs::TypeCode::Struct, "simple_t",
                               {pvxs::Member(pvxs::TypeCode::Int32, "field")})
                     .create();
    EXPECT_EQ(GetMemberNames(value), std::vector<std::string>({"field"}));
  }

  // struct with two nested struct
  {
    auto member1 = pvxs::members::Struct(
        "struct1", "struct1_name", {pvxs::members::Int32("signed"), pvxs::members::Bool("bool")});
    auto member2 = pvxs::members::Struct(
        "struct2", {pvxs::members::Int8("first"), pvxs::members::UInt8("second")});

    auto value =
        ::pvxs::TypeDef(::pvxs::TypeCode::Struct, "struct_name", {member1, member2}).create();

    EXPECT_EQ(GetMemberNames(value), std::vector<std::string>({"struct1", "struct2"}));
  }
}

TEST_F(PvxsUtilsTests, GetFieldNameOfChild)
{
  {  // for structs
    auto parent = pvxs::TypeDef(pvxs::TypeCode::Struct, "simple_t",
                                {pvxs::Member(pvxs::TypeCode::Int32, "field")})
                      .create();
    EXPECT_EQ(GetFieldNameOfChild(parent, parent["field"]), std::string("field"));
  }

  {  // for array of structs
    auto pvxs_value = ::pvxs::TypeDef(::pvxs::TypeCode::StructA, "array_name",
                                      {pvxs::members::Int32("field_name")})
                          .create();

    ::pvxs::Value array_field(pvxs_value);
    ::pvxs::shared_array<::pvxs::Value> arr(2);

    arr[0] = array_field.allocMember();
    arr[0]["field_name"] = 42;
    arr[1] = array_field.allocMember();
    arr[1]["field_name"] = 43;
    array_field = arr.freeze().castTo<const void>();

    // finally checking our convenience function to access children
    auto children = GetChildren(pvxs_value);

    // array of structure reports empty name for its children
    EXPECT_EQ(GetFieldNameOfChild(pvxs_value, children[0]), std::string());
  }
}
