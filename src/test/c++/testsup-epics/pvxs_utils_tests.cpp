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

#include "sup/epics/utils/pvxs_utils.h"

#include <gtest/gtest.h>
#include <pvxs/data.h>
#include <pvxs/nt.h>

#include <iostream>

using namespace ::sup::epics;

class PvxsUtilsTests : public ::testing::Test
{
};

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
  EXPECT_FALSE(IsScalarArray(TypeDef(TypeCode::Union).create()));
  EXPECT_TRUE(IsScalarArray(TypeDef(TypeCode::UnionA).create()));

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
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::Union).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::UnionA).create()));
}

//! Testing GetChildren() utility function. It is used for testing in other places.

TEST_F(PvxsUtilsTests, GetChildrenForScalar)
{
  auto value = ::pvxs::TypeDef(::pvxs::TypeCode::Int16).create();
  EXPECT_TRUE(GetChildren(value).empty());
}

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
