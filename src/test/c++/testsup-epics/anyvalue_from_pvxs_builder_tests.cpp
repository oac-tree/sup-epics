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
#include <pvxs/data.h>
#include <pvxs/nt.h>
#include <sup/dto/anytype_helper.h>
#include <sup/dto/anyvalue.h>
#include <sup/epics/dto_conversion_utils.h>

#include <iostream>

using namespace ::sup::epics;

class AnyValueFromPVXSBuilderTests : public ::testing::Test
{
};

//! Construct AnyValue from scalar-like PVXS value

TEST_F(AnyValueFromPVXSBuilderTests, ScalarTypes)
{
  auto pvxs_value = ::pvxs::TypeDef(::pvxs::TypeCode::Int32).create();
  pvxs_value = 42;

  auto anyvalue = BuildAnyValue(pvxs_value);
  EXPECT_EQ(anyvalue.GetType(), ::sup::dto::SignedInteger32Type);
  EXPECT_EQ(anyvalue.As<::sup::dto::int32>(), 42);
}

//! Construct AnyValue from PVXS containing a struct with single fields.

TEST_F(AnyValueFromPVXSBuilderTests, StructWithSingleField)
{
  sup::dto::AnyType expected_anytype = {{"signed", {sup::dto::SignedInteger32Type}}};

  auto pvxs_value =
      ::pvxs::TypeDef(::pvxs::TypeCode::Struct, {pvxs::members::Int32("signed")}).create();
  pvxs_value["signed"] = 42;

  auto anyvalue = BuildAnyValue(pvxs_value);
  EXPECT_EQ(anyvalue.GetType(), expected_anytype);
  EXPECT_TRUE(::sup::dto::IsStructValue(anyvalue));
  EXPECT_EQ(anyvalue["signed"].As<sup::dto::int32>(), 42);
}

TEST_F(AnyValueFromPVXSBuilderTests, StructWithTwoFields)
{
  sup::dto::AnyType expected_anytype = {{"signed", {sup::dto::SignedInteger32Type}},
                                        {"bool", {sup::dto::BooleanType}}};

  auto pvxs_value = ::pvxs::TypeDef(::pvxs::TypeCode::Struct,
                                    {pvxs::members::Int32("signed"), pvxs::members::Bool("bool")})
                        .create();
  pvxs_value["signed"] = 42;
  pvxs_value["bool"] = true;

  auto anyvalue = BuildAnyValue(pvxs_value);

  EXPECT_EQ(anyvalue.GetType(), expected_anytype);
  EXPECT_TRUE(::sup::dto::IsStructValue(anyvalue));
  EXPECT_EQ(anyvalue["signed"].As<sup::dto::int32>(), 42);
  EXPECT_EQ(anyvalue["bool"].As<sup::dto::boolean>(), true);
}

TEST_F(AnyValueFromPVXSBuilderTests, StructWithNestedStructWithField)
{
  sup::dto::AnyType two_scalars = {{"signed", {sup::dto::SignedInteger32Type}},
                                   {"bool", {sup::dto::BooleanType}}};
  sup::dto::AnyType expected_anytype = {{"scalars", two_scalars}};

  auto pvxs_value =
      ::pvxs::TypeDef(::pvxs::TypeCode::Struct,
                      {pvxs::members::Struct("scalars", {pvxs::members::Int32("signed"),
                                                         pvxs::members::Bool("bool")})})
          .create();

  pvxs_value["scalars.signed"] = 42;
  pvxs_value["scalars.bool"] = true;

  auto anyvalue = BuildAnyValue(pvxs_value);
  EXPECT_EQ(anyvalue.GetType(), expected_anytype);
  EXPECT_TRUE(::sup::dto::IsStructValue(anyvalue));
  EXPECT_TRUE(::sup::dto::IsStructValue(anyvalue["scalars"]));
  EXPECT_EQ(anyvalue["scalars.signed"].As<sup::dto::int32>(), 42);
  EXPECT_EQ(anyvalue["scalars.bool"].As<sup::dto::boolean>(), true);
}

TEST_F(AnyValueFromPVXSBuilderTests, StructWithTwoNestedStructs)
{
  const std::string struct_name = "struct_name";
  sup::dto::AnyType two_scalars = {{{"signed", {sup::dto::SignedInteger32Type}},
                                   {"bool", {sup::dto::BooleanType}}}, "struct1_name"};

  sup::dto::AnyType expected_anytype{
      {{"struct1", two_scalars},
       {"struct2",
        {{"first", {sup::dto::SignedInteger8Type}}, {"second", {sup::dto::UnsignedInteger8Type}}}}},
      struct_name};

  auto member1 = pvxs::members::Struct(
      "struct1", "struct1_name", {pvxs::members::Int32("signed"), pvxs::members::Bool("bool")});
  auto member2 = pvxs::members::Struct(
      "struct2", {pvxs::members::Int8("first"), pvxs::members::UInt8("second")});

  auto pvxs_value =
      ::pvxs::TypeDef(::pvxs::TypeCode::Struct, struct_name, {member1, member2}).create();
  pvxs_value["struct1.signed"] = 42;
  pvxs_value["struct1.bool"] = true;
  pvxs_value["struct2.first"] = -43;
  pvxs_value["struct2.second"] = 44;

  auto anyvalue = BuildAnyValue(pvxs_value);
  EXPECT_EQ(anyvalue.GetType(), expected_anytype);

  EXPECT_TRUE(::sup::dto::IsStructValue(anyvalue));
  EXPECT_TRUE(::sup::dto::IsStructValue(anyvalue["struct1"]));
  EXPECT_TRUE(::sup::dto::IsStructValue(anyvalue["struct2"]));
  EXPECT_EQ(anyvalue["struct1.signed"].As<sup::dto::int32>(), 42);
  EXPECT_EQ(anyvalue["struct1.bool"].As<sup::dto::boolean>(), true);
  EXPECT_EQ(anyvalue["struct2.first"].As<sup::dto::int8>(), -43);
  EXPECT_EQ(anyvalue["struct2.second"].As<sup::dto::uint8>(), 44);
}
