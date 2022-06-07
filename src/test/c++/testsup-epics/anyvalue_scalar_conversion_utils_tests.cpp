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
#include <sup/epics/anyvalue_scalar_conversion_utils.h>

#include <iostream>

using namespace ::sup::epics;

class AnyValueScalarConversionUtilsTests : public ::testing::Test
{
};

//! Checks GetPVXSValueFromScalar helper method to construct PVXS value from scalar based AnyValue.

TEST_F(AnyValueScalarConversionUtilsTests, GetPVXSValueFromScalar)
{
  {  // from Bool
    sup::dto::AnyValue any_value{sup::dto::BooleanType};
    any_value = true;
    auto result = GetPVXSValueFromScalar(any_value);
    EXPECT_TRUE(result.valid());
    EXPECT_EQ(result.type(), ::pvxs::TypeCode::Bool);
    EXPECT_EQ(result.as<bool>(), true);
  }

  {  // from Char
    sup::dto::AnyValue any_value{sup::dto::Character8Type};
    any_value = 'w';
    auto result = GetPVXSValueFromScalar(any_value);
    EXPECT_TRUE(result.valid());
    EXPECT_EQ(result.type(), ::pvxs::TypeCode::UInt8);
    EXPECT_EQ(result.as<char>(), 'w');
  }

  {  // from Int8
    sup::dto::AnyValue any_value{sup::dto::SignedInteger8Type};
    any_value = 42;
    auto result = GetPVXSValueFromScalar(any_value);
    EXPECT_TRUE(result.valid());
    EXPECT_EQ(result.type(), ::pvxs::TypeCode::Int8);
    EXPECT_EQ(result.as<int8_t>(), 42);
  }

  {  // from UInt8
    sup::dto::AnyValue any_value{sup::dto::UnsignedInteger8Type};
    any_value = 42;
    auto result = GetPVXSValueFromScalar(any_value);
    EXPECT_TRUE(result.valid());
    EXPECT_EQ(result.type(), ::pvxs::TypeCode::UInt8);
    EXPECT_EQ(result.as<uint8_t>(), 42);
  }

  {  // from Int16
    sup::dto::AnyValue any_value{sup::dto::SignedInteger16Type};
    any_value = 42;
    auto result = GetPVXSValueFromScalar(any_value);
    EXPECT_TRUE(result.valid());
    EXPECT_EQ(result.type(), ::pvxs::TypeCode::Int16);
    EXPECT_EQ(result.as<int16_t>(), 42);
  }

  {  // from UInt16
    sup::dto::AnyValue any_value{sup::dto::UnsignedInteger16Type};
    any_value = 42;
    auto result = GetPVXSValueFromScalar(any_value);
    EXPECT_TRUE(result.valid());
    EXPECT_EQ(result.type(), ::pvxs::TypeCode::UInt16);
    EXPECT_EQ(result.as<uint16_t>(), 42);
  }

  {  // from Int32
    sup::dto::AnyValue any_value{sup::dto::SignedInteger32Type};
    any_value = 42;
    auto result = GetPVXSValueFromScalar(any_value);
    EXPECT_TRUE(result.valid());
    EXPECT_EQ(result.type(), ::pvxs::TypeCode::Int32);
    EXPECT_EQ(result.as<int32_t>(), 42);
  }

  {  // from UInt32
    sup::dto::AnyValue any_value{sup::dto::UnsignedInteger32Type};
    any_value = 42;
    auto result = GetPVXSValueFromScalar(any_value);
    EXPECT_TRUE(result.valid());
    EXPECT_EQ(result.type(), ::pvxs::TypeCode::UInt32);
    EXPECT_EQ(result.as<uint32_t>(), 42);
  }

  {  // from Int64
    sup::dto::AnyValue any_value{sup::dto::SignedInteger64Type};
    any_value = 42;
    auto result = GetPVXSValueFromScalar(any_value);
    EXPECT_TRUE(result.valid());
    EXPECT_EQ(result.type(), ::pvxs::TypeCode::Int64);
    EXPECT_EQ(result.as<int64_t>(), 42);
  }

  {  // from UInt64
    sup::dto::AnyValue any_value{sup::dto::UnsignedInteger64Type};
    any_value = 42;
    auto result = GetPVXSValueFromScalar(any_value);
    EXPECT_TRUE(result.valid());
    EXPECT_EQ(result.type(), ::pvxs::TypeCode::UInt64);
    EXPECT_EQ(result.as<uint64_t>(), 42);
  }

  {  // from Float32
    sup::dto::AnyValue any_value{sup::dto::Float32Type};
    any_value = 42.1;
    auto result = GetPVXSValueFromScalar(any_value);
    EXPECT_TRUE(result.valid());
    EXPECT_EQ(result.type(), ::pvxs::TypeCode::Float32);
    EXPECT_FLOAT_EQ(result.as<float>(), 42.1);
  }

  {  // from Float64
    sup::dto::AnyValue any_value{sup::dto::Float64Type};
    any_value = 42.1;
    auto result = GetPVXSValueFromScalar(any_value);
    EXPECT_TRUE(result.valid());
    EXPECT_EQ(result.type(), ::pvxs::TypeCode::Float64);
    EXPECT_DOUBLE_EQ(result.as<double>(), 42.1);
  }

  {  // from string
    sup::dto::AnyValue any_value{sup::dto::StringType};
    any_value = std::string("abc");
    auto result = GetPVXSValueFromScalar(any_value);
    EXPECT_TRUE(result.valid());
    EXPECT_EQ(result.type(), ::pvxs::TypeCode::String);
    EXPECT_EQ(result.as<std::string>(), std::string("abc"));
  }

  {  // from long string
    sup::dto::AnyValue any_value{sup::dto::StringType};
    any_value = std::string(1025, 'a');
    auto result = GetPVXSValueFromScalar(any_value);
    EXPECT_TRUE(result.valid());
    EXPECT_EQ(result.type(), ::pvxs::TypeCode::String);
    EXPECT_EQ(result.as<std::string>(), std::string(1025, 'a'));
  }

  {  // attempt to construct from AnyValue based on structure
    sup::dto::AnyValue any_value = {
        {{"signed", {sup::dto::SignedInteger32Type, 42}}, {"bool", {sup::dto::BooleanType, true}}}};
    EXPECT_THROW(GetPVXSValueFromScalar(any_value), std::runtime_error);
  }
}

//! Checks GetPVXSValueFromScalar helper method to assign scalar-like AnyValue to PVXS
//! value (pre-created with correct type).
TEST_F(AnyValueScalarConversionUtilsTests, AssignPVXSValueFromScalar)
{
  {  // from Bool
    sup::dto::AnyValue any_value{sup::dto::BooleanType};
    any_value = true;
    pvxs::Value pvxs_value = pvxs::TypeDef(pvxs::TypeCode::Bool).create();
    AssignPVXSValueFromScalar(any_value, pvxs_value);
    EXPECT_EQ(pvxs_value.as<bool>(), true);
  }

  {  // from Char
    sup::dto::AnyValue any_value{sup::dto::Character8Type};
    any_value = 'w';
    pvxs::Value pvxs_value = pvxs::TypeDef(pvxs::TypeCode::UInt8).create();
    AssignPVXSValueFromScalar(any_value, pvxs_value);
    EXPECT_EQ(pvxs_value.as<char>(), 'w');
  }

  {  // from Int8
    sup::dto::AnyValue any_value{sup::dto::SignedInteger8Type};
    any_value = 42;
    pvxs::Value pvxs_value = pvxs::TypeDef(pvxs::TypeCode::Int8).create();
    AssignPVXSValueFromScalar(any_value, pvxs_value);
    EXPECT_EQ(pvxs_value.as<int8_t>(), 42);
  }

  {  // from UInt8
    sup::dto::AnyValue any_value{sup::dto::UnsignedInteger8Type};
    any_value = 42;
    pvxs::Value pvxs_value = pvxs::TypeDef(pvxs::TypeCode::UInt8).create();
    AssignPVXSValueFromScalar(any_value, pvxs_value);
    EXPECT_EQ(pvxs_value.as<uint8_t>(), 42);
  }

  {  // from Int16
    sup::dto::AnyValue any_value{sup::dto::SignedInteger16Type};
    any_value = 42;
    pvxs::Value pvxs_value = pvxs::TypeDef(pvxs::TypeCode::Int16).create();
    AssignPVXSValueFromScalar(any_value, pvxs_value);
    EXPECT_EQ(pvxs_value.as<int16_t>(), 42);
  }

  {  // from UInt16
    sup::dto::AnyValue any_value{sup::dto::UnsignedInteger16Type};
    any_value = 42;
    pvxs::Value pvxs_value = pvxs::TypeDef(pvxs::TypeCode::UInt16).create();
    AssignPVXSValueFromScalar(any_value, pvxs_value);
    EXPECT_EQ(pvxs_value.as<uint16_t>(), 42);
  }

  {  // from Int32
    sup::dto::AnyValue any_value{sup::dto::SignedInteger32Type};
    any_value = 42;
    pvxs::Value pvxs_value = pvxs::TypeDef(pvxs::TypeCode::Int32).create();
    AssignPVXSValueFromScalar(any_value, pvxs_value);
    EXPECT_EQ(pvxs_value.as<int32_t>(), 42);
  }

  {  // from UInt32
    sup::dto::AnyValue any_value{sup::dto::UnsignedInteger32Type};
    any_value = 42;
    pvxs::Value pvxs_value = pvxs::TypeDef(pvxs::TypeCode::UInt32).create();
    AssignPVXSValueFromScalar(any_value, pvxs_value);
    EXPECT_EQ(pvxs_value.as<uint32_t>(), 42);
  }

  {  // from Int64
    sup::dto::AnyValue any_value{sup::dto::SignedInteger64Type};
    any_value = 42;
    pvxs::Value pvxs_value = pvxs::TypeDef(pvxs::TypeCode::Int64).create();
    AssignPVXSValueFromScalar(any_value, pvxs_value);
    EXPECT_EQ(pvxs_value.as<int64_t>(), 42);
  }

  {  // from UInt64
    sup::dto::AnyValue any_value{sup::dto::UnsignedInteger64Type};
    any_value = 42;
    pvxs::Value pvxs_value = pvxs::TypeDef(pvxs::TypeCode::UInt64).create();
    AssignPVXSValueFromScalar(any_value, pvxs_value);
    EXPECT_EQ(pvxs_value.as<uint64_t>(), 42);
  }

  {  // from Float32
    sup::dto::AnyValue any_value{sup::dto::Float32Type};
    any_value = 42.1;
    pvxs::Value pvxs_value = pvxs::TypeDef(pvxs::TypeCode::Float32).create();
    AssignPVXSValueFromScalar(any_value, pvxs_value);
    EXPECT_FLOAT_EQ(pvxs_value.as<float>(), 42.1);
  }

  {  // from Float64
    sup::dto::AnyValue any_value{sup::dto::Float64Type};
    any_value = 42.1;
    pvxs::Value pvxs_value = pvxs::TypeDef(pvxs::TypeCode::Float64).create();
    AssignPVXSValueFromScalar(any_value, pvxs_value);
    EXPECT_DOUBLE_EQ(pvxs_value.as<double>(), 42.1);
  }

  {  // from string
    sup::dto::AnyValue any_value{sup::dto::StringType};
    any_value = std::string("abc");
    pvxs::Value pvxs_value = pvxs::TypeDef(pvxs::TypeCode::String).create();
    AssignPVXSValueFromScalar(any_value, pvxs_value);
    EXPECT_EQ(pvxs_value.as<std::string>(), std::string("abc"));
  }

  {  // from long string
    sup::dto::AnyValue any_value{sup::dto::StringType};
    any_value = std::string(1025, 'a');
    pvxs::Value pvxs_value = pvxs::TypeDef(pvxs::TypeCode::String).create();
    AssignPVXSValueFromScalar(any_value, pvxs_value);
    EXPECT_EQ(pvxs_value.as<std::string>(), std::string(1025, 'a'));
  }

  {  // attempt to assign from AnyValue based on structure
    sup::dto::AnyValue any_value = {
        {{"signed", {sup::dto::SignedInteger32Type, 42}}, {"bool", {sup::dto::BooleanType, true}}}};
    pvxs::Value pvxs_value = pvxs::TypeDef(pvxs::TypeCode::String).create();
    EXPECT_THROW(AssignPVXSValueFromScalar(any_value, pvxs_value), std::runtime_error);
  }

  {  // attempt to assign from similar but not coinciding types
    sup::dto::AnyValue any_value{sup::dto::Float32Type};
    any_value = 42.1;
    pvxs::Value pvxs_value =
        pvxs::TypeDef(pvxs::TypeCode::Float64).create();  // deliberately Float64, and not Float32
    EXPECT_THROW(AssignPVXSValueFromScalar(any_value, pvxs_value), std::runtime_error);
  }
}

//! Checks AssignPVXSValueFromScalarArray helper method to assign PVXS arrays from array-like
//! AnyValues. FIXME add tests for all types.

TEST_F(AnyValueScalarConversionUtilsTests, AssignPVXSValueFromScalarArray)
{
  const int n_elements = 2;
  sup::dto::AnyValue any_value(n_elements, sup::dto::SignedInteger32Type);
  any_value[0] = 42;

  pvxs::Value pvxs_value = pvxs::TypeDef(pvxs::TypeCode::Int32A).create();
  AssignPVXSValueFromScalarArray(any_value, pvxs_value);

  auto pvxs_data = pvxs_value.as<::pvxs::shared_array<const int32_t>>();
  EXPECT_EQ(pvxs_data.size(), 2);
  EXPECT_EQ(pvxs_data[0], 42);
  EXPECT_EQ(pvxs_data[1], 0);

  {  // attempt to assign to scalar
    pvxs::Value pvxs_value = pvxs::TypeDef(pvxs::TypeCode::Int32).create();
    EXPECT_THROW(AssignPVXSValueFromScalarArray(any_value, pvxs_value), std::runtime_error);
  }

  {  // attempt to assign to scalar array of wrong type
    pvxs::Value pvxs_value = pvxs::TypeDef(pvxs::TypeCode::Int64).create();
    EXPECT_THROW(AssignPVXSValueFromScalarArray(any_value, pvxs_value), std::runtime_error);
  }
}

//! Checks AssignPVXSValueToAnyValueScalar helper method to assign scalar-like PVXS value to
//! pre-created AnyValue.
TEST_F(AnyValueScalarConversionUtilsTests, AssignPVXSValueToAnyValueScalar)
{
  {  // from Bool
    sup::dto::AnyValue any_value{sup::dto::BooleanType};
    any_value = false;
    pvxs::Value pvxs_value = pvxs::TypeDef(pvxs::TypeCode::Bool).create();
    pvxs_value = true;
    AssignPVXSValueToAnyValueScalar(pvxs_value, any_value);
    EXPECT_EQ(any_value.As<sup::dto::boolean>(), true);
  }

//  {  // from Char
//    sup::dto::AnyValue any_value{sup::dto::Character8Type};
//    any_value = 'w';
//    pvxs::Value pvxs_value = pvxs::TypeDef(pvxs::TypeCode::UInt8).create();
//    AssignPVXSValueFromScalar(any_value, pvxs_value);
//    EXPECT_EQ(pvxs_value.as<char>(), 'w');
//  }

  {  // from Int8
    sup::dto::AnyValue any_value{sup::dto::SignedInteger8Type};
    any_value = 0;
    pvxs::Value pvxs_value = pvxs::TypeDef(pvxs::TypeCode::Int8).create();
    pvxs_value = 42;
    AssignPVXSValueToAnyValueScalar(pvxs_value, any_value);
    EXPECT_EQ(any_value.As<sup::dto::int8>(), 42);
  }

//  {  // from UInt8
//    sup::dto::AnyValue any_value{sup::dto::UnsignedInteger8Type};
//    any_value = 0;
//    pvxs::Value pvxs_value = pvxs::TypeDef(pvxs::TypeCode::UInt8).create();
//    pvxs_value = 42;
//    AssignPVXSValueToAnyValueScalar(pvxs_value, any_value);
//    EXPECT_EQ(any_value.As<sup::dto::uint8>(), 42);
//  }

  {  // from Int16
    sup::dto::AnyValue any_value{sup::dto::SignedInteger16Type};
    any_value = 0;
    pvxs::Value pvxs_value = pvxs::TypeDef(pvxs::TypeCode::Int16).create();
    pvxs_value = 42;
    AssignPVXSValueToAnyValueScalar(pvxs_value, any_value);
    EXPECT_EQ(any_value.As<sup::dto::int16>(), 42);
  }

  {  // from UInt16
    sup::dto::AnyValue any_value{sup::dto::UnsignedInteger16Type};
    any_value = 0;
    pvxs::Value pvxs_value = pvxs::TypeDef(pvxs::TypeCode::UInt16).create();
    pvxs_value = 42;
    AssignPVXSValueToAnyValueScalar(pvxs_value, any_value);
    EXPECT_EQ(any_value.As<sup::dto::uint16>(), 42);
  }

  {  // from Int32
    sup::dto::AnyValue any_value{sup::dto::SignedInteger32Type};
    any_value = 0;
    pvxs::Value pvxs_value = pvxs::TypeDef(pvxs::TypeCode::Int32).create();
    pvxs_value = 42;
    AssignPVXSValueToAnyValueScalar(pvxs_value, any_value);
    EXPECT_EQ(any_value.As<sup::dto::int32>(), 42);
  }

  {  // from UInt32
    sup::dto::AnyValue any_value{sup::dto::UnsignedInteger32Type};
    any_value = 0;
    pvxs::Value pvxs_value = pvxs::TypeDef(pvxs::TypeCode::UInt32).create();
    pvxs_value = 42;
    AssignPVXSValueToAnyValueScalar(pvxs_value, any_value);
    EXPECT_EQ(any_value.As<sup::dto::uint32>(), 42);
  }

  {  // from Int64
    sup::dto::AnyValue any_value{sup::dto::SignedInteger64Type};
    any_value = 0;
    pvxs::Value pvxs_value = pvxs::TypeDef(pvxs::TypeCode::Int64).create();
    pvxs_value = 42;
    AssignPVXSValueToAnyValueScalar(pvxs_value, any_value);
    EXPECT_EQ(any_value.As<sup::dto::int64>(), 42);
  }

  {  // from UInt64
    sup::dto::AnyValue any_value{sup::dto::UnsignedInteger64Type};
    any_value = 0;
    pvxs::Value pvxs_value = pvxs::TypeDef(pvxs::TypeCode::UInt64).create();
    pvxs_value = 42;
    AssignPVXSValueToAnyValueScalar(pvxs_value, any_value);
    EXPECT_EQ(any_value.As<sup::dto::uint64>(), 42);
  }

  {  // from Float32
    sup::dto::AnyValue any_value{sup::dto::Float32Type};
    any_value = 0.0;
    pvxs::Value pvxs_value = pvxs::TypeDef(pvxs::TypeCode::Float32).create();
    pvxs_value = 42;
    AssignPVXSValueToAnyValueScalar(pvxs_value, any_value);
    EXPECT_EQ(any_value.As<sup::dto::float32>(), 42);
  }

  {  // from Float64
    sup::dto::AnyValue any_value{sup::dto::Float64Type};
    any_value = 0.0;
    pvxs::Value pvxs_value = pvxs::TypeDef(pvxs::TypeCode::Float64).create();
    pvxs_value = 42;
    AssignPVXSValueToAnyValueScalar(pvxs_value, any_value);
    EXPECT_EQ(any_value.As<sup::dto::float64>(), 42);
  }

  {  // from string
    sup::dto::AnyValue any_value{sup::dto::StringType};
    any_value = std::string();
    pvxs::Value pvxs_value = pvxs::TypeDef(pvxs::TypeCode::String).create();
    pvxs_value = std::string("abc");
    AssignPVXSValueToAnyValueScalar(pvxs_value, any_value);
    EXPECT_EQ(any_value.As<std::string>(), std::string("abc"));
  }

  {  // from long string
    sup::dto::AnyValue any_value{sup::dto::StringType};
    any_value = std::string();
    pvxs::Value pvxs_value = pvxs::TypeDef(pvxs::TypeCode::String).create();
    pvxs_value = std::string(1025, 'a');
    AssignPVXSValueToAnyValueScalar(pvxs_value, any_value);
    EXPECT_EQ(any_value.As<std::string>(), std::string(1025, 'a'));
  }

  {  // attempt to assign from AnyValue based on structure
    sup::dto::AnyValue any_value = {
        {{"signed", {sup::dto::SignedInteger32Type, 42}}, {"bool", {sup::dto::BooleanType, true}}}};
    pvxs::Value pvxs_value = pvxs::TypeDef(pvxs::TypeCode::String).create();
    EXPECT_THROW(AssignPVXSValueToAnyValueScalar(pvxs_value, any_value), std::runtime_error);
  }

  {  // attempt to assign from similar but not coinciding types
    sup::dto::AnyValue any_value{sup::dto::Float32Type};
    any_value = 0.0;
    pvxs::Value pvxs_value =
        pvxs::TypeDef(pvxs::TypeCode::Float64).create();  // deliberately Float64, and not Float32
    EXPECT_THROW(AssignPVXSValueToAnyValueScalar(pvxs_value, any_value), std::runtime_error);
  }
}
