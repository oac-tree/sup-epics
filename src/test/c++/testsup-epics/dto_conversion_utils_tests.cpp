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
#include <sup/dto/anyvalue.h>
#include <sup/epics/utils/dto_conversion_utils.h>
#include <sup/epics/utils/pvxs_utils.h>

#include <iostream>

using namespace ::sup::epics;

class DtoConversionUtilsTests : public ::testing::Test
{
};

//! Check ConvertScalarToStruct utility function.

TEST_F(DtoConversionUtilsTests, ConvertScalarToStruct)
{
  // convert from integer scalar
  sup::dto::AnyValue any_value{sup::dto::SignedInteger32Type, 42};
  auto result = ConvertScalarToStruct(any_value);
  EXPECT_TRUE(IsStructValue(result));
  EXPECT_EQ(result.MemberNames(), std::vector<std::string>({"value"}));
  EXPECT_EQ(result["value"], 42);

  // attempt to convert from struct
  sup::dto::AnyValue struct_value = {{{"value", {sup::dto::SignedInteger32Type, 42}}}};
  EXPECT_THROW(ConvertScalarToStruct(struct_value), std::runtime_error);
}

//! Check ConvertScalarToStruct utility function.

TEST_F(DtoConversionUtilsTests, ConvertStructToScalar)
{
  // struct with single `value` field should turn into a scalar
  sup::dto::AnyValue any_value = {{{"value", {sup::dto::SignedInteger32Type, 42}}}};
  auto result = ConvertStructToScalar(any_value);
  EXPECT_EQ(result, any_value["value"]);

  // attempt to convert from scalar
  sup::dto::AnyValue scalar_value{sup::dto::SignedInteger32Type, 42};
  EXPECT_THROW(ConvertStructToScalar(scalar_value), std::runtime_error);

  // attempt to convert from struct that has more than one field
  sup::dto::AnyValue large_struct = {{"value", {sup::dto::SignedInteger32Type, 42}},
                                     {"bool", {sup::dto::BooleanType, true}}};
  EXPECT_THROW(ConvertStructToScalar(large_struct), std::runtime_error);

  // attempt to convert from struct that has wrong field type
  sup::dto::AnyValue two_scalars = {{"signed", {sup::dto::SignedInteger32Type, 42}},
                                    {"bool", {sup::dto::BooleanType, true}}};
  sup::dto::AnyValue nested_struct = {{"value", two_scalars}};
  EXPECT_THROW(ConvertStructToScalar(nested_struct), std::runtime_error);
}

//! Validating BuildScalarAwarePVXSValue utility function. Building PVXS value from scalar AnyValue.
//! Expecting to get a struct with single `value` field.

TEST_F(DtoConversionUtilsTests, BuildScalarAwarePVXSValueFromScalar)
{
  sup::dto::AnyValue any_value{sup::dto::SignedInteger32Type, 42};

  auto pvxs_value = BuildScalarAwarePVXSValue(any_value);

  EXPECT_TRUE(pvxs_value.valid());
  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Struct);
  EXPECT_EQ(GetMemberNames(pvxs_value), std::vector<std::string>({"value"}));
  EXPECT_EQ(pvxs_value["value"].type(), ::pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value["value"].as<int32_t>(), 42);
}

//! Validating BuildScalarAwarePVXSValue utility function. Building PVXS value from AnyValue
//! representing a struct with a single field. Expecting one-to-one conversion.

TEST_F(DtoConversionUtilsTests, BuildScalarAwarePVXSValueFromStructWithSingleField)
{
  sup::dto::AnyValue any_value = {{{"signed", {sup::dto::SignedInteger32Type, 42}}}};

  auto pvxs_value = BuildScalarAwarePVXSValue(any_value);

  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Struct);
  EXPECT_EQ(pvxs_value.nmembers(), 1);

  auto names = GetMemberNames(pvxs_value);
  EXPECT_EQ(names, std::vector<std::string>({"signed"}));
  EXPECT_EQ(pvxs_value["signed"].type(), ::pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value["signed"].as<int32_t>(), 42);
}

//! Validating BuildScalarAwareAnyValue utility function. PVXS value contain a struct with
//! single `value` field.  Expecting to get back a scalar.
TEST_F(DtoConversionUtilsTests, BuildScalarAwareAnyValueFromStructWithSingleField)
{
  auto pvxs_value =
      ::pvxs::TypeDef(::pvxs::TypeCode::Struct, {pvxs::members::Int32("value")}).create();
  pvxs_value["value"] = 42;

  auto anyvalue = BuildScalarAwareAnyValue(pvxs_value);

  sup::dto::AnyType expected_anytype = {sup::dto::SignedInteger32Type};

  EXPECT_EQ(anyvalue.GetType(), expected_anytype);
  EXPECT_FALSE(::sup::dto::IsStructValue(anyvalue));
  EXPECT_EQ(anyvalue, 42);
}

//! Validating BuildScalarAwarePVXSValue utility function. Building PVXS value from AnyValue
//! representing a struct with a single custom field. Expecting one-to-one conversion.

TEST_F(DtoConversionUtilsTests, BuildScalarAwareAnyValueFromStructWithCustomField)
{
  auto pvxs_value =
      ::pvxs::TypeDef(::pvxs::TypeCode::Struct, {pvxs::members::Int32("field")}).create();
  pvxs_value["field"] = 42;

  auto anyvalue = BuildScalarAwareAnyValue(pvxs_value);

  sup::dto::AnyType expected_anytype = {{"field", {sup::dto::SignedInteger32Type}}};

  EXPECT_EQ(anyvalue.GetType(), expected_anytype);
  EXPECT_TRUE(::sup::dto::IsStructValue(anyvalue));
  EXPECT_EQ(anyvalue["field"].As<sup::dto::int32>(), 42);
}
