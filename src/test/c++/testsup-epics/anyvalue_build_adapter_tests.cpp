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

#include "sup/epics/anyvalue_build_adapter.h"

#include <gtest/gtest.h>
#include <sup/dto/anytype_helper.h>
#include <sup/dto/anyvalue.h>
#include <sup/dto/anyvalue_helper.h>

using namespace ::sup::epics;

class AnyValueBuildAdapterTests : public ::testing::Test
{
};

//! Creation of AnyValue containing a struct with single field.

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
  EXPECT_EQ(value["signed"].As<sup::dto::int32>(), 42);
}

//! Creation of AnyValue containing a struct with two fields.

TEST_F(AnyValueBuildAdapterTests, StructWithTwoFields)
{
  sup::dto::AnyType any_type = {{"signed", {sup::dto::SignedInteger32Type}},
                                {"bool", {sup::dto::BooleanType}}};

  AnyValueBuildAdapter builder;

  builder.StartStruct();
  builder.Int32("signed", 42);
  builder.Bool("bool", true);
  builder.EndStruct();

  auto value = builder.MoveAnyValue();
  EXPECT_EQ(value.GetType(), any_type);
  EXPECT_TRUE(::sup::dto::IsStructValue(value));
  EXPECT_EQ(value["signed"].As<sup::dto::int32>(), 42);
  EXPECT_EQ(value["bool"].As<sup::dto::boolean>(), true);
}

//! Creation of AnyValue containing a struct with another struct in it.

TEST_F(AnyValueBuildAdapterTests, StructWithNestedStructWithField)
{
  sup::dto::AnyType two_scalars = {{"signed", {sup::dto::SignedInteger32Type}},
                                   {"bool", {sup::dto::BooleanType}}};
  sup::dto::AnyType any_type = {{"scalars", two_scalars}};

  AnyValueBuildAdapter builder;

  builder.StartStruct();
  builder.StartStruct();
  builder.Int32("signed", 42);
  builder.Bool("bool", true);
  builder.EndStruct("scalars");
  builder.EndStruct();

  auto value = builder.MoveAnyValue();
  EXPECT_EQ(value.GetType(), any_type);
  EXPECT_TRUE(::sup::dto::IsStructValue(value));
  EXPECT_TRUE(::sup::dto::IsStructValue(value["scalars"]));
  EXPECT_EQ(value["scalars.signed"].As<sup::dto::int32>(), 42);
  EXPECT_EQ(value["scalars.bool"].As<sup::dto::boolean>(), true);
}

//! Creation of AnyValue containing two structures.

TEST_F(AnyValueBuildAdapterTests, StructWithTwoNestedStructs)
{
  const std::string struct_name = "struct_name";
  sup::dto::AnyType two_scalars = {{"signed", {sup::dto::SignedInteger32Type}},
                                   {"bool", {sup::dto::BooleanType}}};

  sup::dto::AnyType any_type{
      {{"struct1", two_scalars},
       {"struct2",
        {{"first", {sup::dto::SignedInteger8Type}}, {"second", {sup::dto::UnsignedInteger8Type}}}}},
      struct_name};

  AnyValueBuildAdapter builder;

  builder.StartStruct(struct_name);

  builder.StartStruct();
  builder.Int32("signed", 42);
  builder.Bool("bool", true);
  builder.EndStruct("struct1");

  builder.StartStruct();
  builder.Int8("first", -43);
  builder.UInt8("second", 44);
  builder.EndStruct("struct2");

  builder.EndStruct();

  auto value = builder.MoveAnyValue();
  EXPECT_EQ(value.GetType(), any_type);
  EXPECT_TRUE(::sup::dto::IsStructValue(value));
  EXPECT_TRUE(::sup::dto::IsStructValue(value["struct1"]));
  EXPECT_TRUE(::sup::dto::IsStructValue(value["struct2"]));
  EXPECT_EQ(value["struct1.signed"].As<sup::dto::int32>(), 42);
  EXPECT_EQ(value["struct1.bool"].As<sup::dto::boolean>(), true);
  EXPECT_EQ(value["struct2.first"].As<sup::dto::int8>(), -43);
  EXPECT_EQ(value["struct2.second"].As<sup::dto::uint8>(), 44);
}
