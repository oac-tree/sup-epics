/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP common components for EPICS network protocol
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

#include "sup/epics/dtoconversionutils.h"

#include "AnyValue.h"

#include <gtest/gtest.h>
#include <pvxs/data.h>
#include <pvxs/nt.h>

#include <iostream>

using namespace ::sup::epics;

class DtoConversionUtilsTest : public ::testing::Test
{
};

//! Checks GetPVXSBaseTypeCode method to construct PVXS TypeCode from AnyType (base types).

TEST_F(DtoConversionUtilsTest, GetPVXSBaseTypeCode)
{
  EXPECT_EQ(GetPVXSBaseTypeCode(sup::dto::EmptyType), ::pvxs::TypeCode::Null);
  EXPECT_EQ(GetPVXSBaseTypeCode(sup::dto::Boolean), ::pvxs::TypeCode::Bool);
  EXPECT_EQ(GetPVXSBaseTypeCode(sup::dto::Character8), ::pvxs::TypeCode::UInt8);
  EXPECT_EQ(GetPVXSBaseTypeCode(sup::dto::SignedInteger8), ::pvxs::TypeCode::Int8);
  EXPECT_EQ(GetPVXSBaseTypeCode(sup::dto::UnsignedInteger8), ::pvxs::TypeCode::UInt8);
  EXPECT_EQ(GetPVXSBaseTypeCode(sup::dto::SignedInteger16), ::pvxs::TypeCode::Int16);
  EXPECT_EQ(GetPVXSBaseTypeCode(sup::dto::UnsignedInteger16), ::pvxs::TypeCode::UInt16);
  EXPECT_EQ(GetPVXSBaseTypeCode(sup::dto::SignedInteger32), ::pvxs::TypeCode::Int32);
  EXPECT_EQ(GetPVXSBaseTypeCode(sup::dto::UnsignedInteger32), ::pvxs::TypeCode::UInt32);
  EXPECT_EQ(GetPVXSBaseTypeCode(sup::dto::SignedInteger64), ::pvxs::TypeCode::Int64);
  EXPECT_EQ(GetPVXSBaseTypeCode(sup::dto::UnsignedInteger64), ::pvxs::TypeCode::UInt64);
  EXPECT_EQ(GetPVXSBaseTypeCode(sup::dto::Float32), ::pvxs::TypeCode::Float32);
  EXPECT_EQ(GetPVXSBaseTypeCode(sup::dto::Float64), ::pvxs::TypeCode::Float64);
  EXPECT_EQ(GetPVXSBaseTypeCode(sup::dto::String), ::pvxs::TypeCode::String);

  {  // AnyType based on struct
    ::sup::dto::AnyType any_type{{{"signed", ::sup::dto::SignedInteger8}}};
    EXPECT_EQ(GetPVXSBaseTypeCode(any_type), ::pvxs::TypeCode::Struct);
  }
}

//! Checks GetPVXSElementTypeCode method to construct PVXS TypeCode from AnyType (array elements).

TEST_F(DtoConversionUtilsTest, GetPVXSElementTypeCode)
{
  EXPECT_EQ(GetPVXSElementTypeCode(sup::dto::Boolean), ::pvxs::TypeCode::BoolA);
  EXPECT_EQ(GetPVXSElementTypeCode(sup::dto::Character8), ::pvxs::TypeCode::UInt8A);
  EXPECT_EQ(GetPVXSElementTypeCode(sup::dto::SignedInteger8), ::pvxs::TypeCode::Int8A);
  EXPECT_EQ(GetPVXSElementTypeCode(sup::dto::UnsignedInteger8), ::pvxs::TypeCode::UInt8A);
  EXPECT_EQ(GetPVXSElementTypeCode(sup::dto::SignedInteger16), ::pvxs::TypeCode::Int16A);
  EXPECT_EQ(GetPVXSElementTypeCode(sup::dto::UnsignedInteger16), ::pvxs::TypeCode::UInt16A);
  EXPECT_EQ(GetPVXSElementTypeCode(sup::dto::SignedInteger32), ::pvxs::TypeCode::Int32A);
  EXPECT_EQ(GetPVXSElementTypeCode(sup::dto::UnsignedInteger32), ::pvxs::TypeCode::UInt32A);
  EXPECT_EQ(GetPVXSElementTypeCode(sup::dto::SignedInteger64), ::pvxs::TypeCode::Int64A);
  EXPECT_EQ(GetPVXSElementTypeCode(sup::dto::UnsignedInteger64), ::pvxs::TypeCode::UInt64A);
  EXPECT_EQ(GetPVXSElementTypeCode(sup::dto::Float32), ::pvxs::TypeCode::Float32A);
  EXPECT_EQ(GetPVXSElementTypeCode(sup::dto::Float64), ::pvxs::TypeCode::Float64A);
  EXPECT_EQ(GetPVXSElementTypeCode(sup::dto::String), ::pvxs::TypeCode::StringA);

  {  // AnyType based on struct
    ::sup::dto::AnyType any_type{{{"signed", ::sup::dto::SignedInteger8}}};
    EXPECT_EQ(GetPVXSElementTypeCode(any_type), ::pvxs::TypeCode::StructA);
  }
}

//! Checks GetPVXSTypeCode method to construct PVXS TypeCode from AnyType.
//! Checks scalars and array of scalars.

TEST_F(DtoConversionUtilsTest, GetPVXSTypeCode)
{
  const int n_elements{42};
  using ::sup::dto::AnyType;
  using ::sup::dto::TypeCode;

  EXPECT_EQ(GetPVXSTypeCode(AnyType(TypeCode::Bool)), ::pvxs::TypeCode::Bool);
  EXPECT_EQ(GetPVXSTypeCode(AnyType(n_elements, sup::dto::Boolean)), ::pvxs::TypeCode::BoolA);

  EXPECT_EQ(GetPVXSTypeCode(AnyType(TypeCode::UInt8)), ::pvxs::TypeCode::UInt8);
  EXPECT_EQ(GetPVXSTypeCode(AnyType(n_elements, sup::dto::Character8)), ::pvxs::TypeCode::UInt8A);

  EXPECT_EQ(GetPVXSTypeCode(AnyType(TypeCode::Int8)), ::pvxs::TypeCode::Int8);
  EXPECT_EQ(GetPVXSTypeCode(AnyType(n_elements, sup::dto::SignedInteger8)),
            ::pvxs::TypeCode::Int8A);

  EXPECT_EQ(GetPVXSTypeCode(AnyType(TypeCode::UInt8)), ::pvxs::TypeCode::UInt8);
  EXPECT_EQ(GetPVXSTypeCode(AnyType(n_elements, sup::dto::UnsignedInteger8)),
            ::pvxs::TypeCode::UInt8A);

  EXPECT_EQ(GetPVXSTypeCode(AnyType(TypeCode::Int16)), ::pvxs::TypeCode::Int16);
  EXPECT_EQ(GetPVXSTypeCode(AnyType(n_elements, sup::dto::SignedInteger16)),
            ::pvxs::TypeCode::Int16A);

  EXPECT_EQ(GetPVXSTypeCode(AnyType(TypeCode::UInt16)), ::pvxs::TypeCode::UInt16);
  EXPECT_EQ(GetPVXSTypeCode(AnyType(n_elements, sup::dto::UnsignedInteger16)),
            ::pvxs::TypeCode::UInt16A);

  EXPECT_EQ(GetPVXSTypeCode(AnyType(TypeCode::Int32)), ::pvxs::TypeCode::Int32);
  EXPECT_EQ(GetPVXSTypeCode(AnyType(n_elements, sup::dto::SignedInteger32)),
            ::pvxs::TypeCode::Int32A);

  EXPECT_EQ(GetPVXSTypeCode(AnyType(TypeCode::UInt32)), ::pvxs::TypeCode::UInt32);
  EXPECT_EQ(GetPVXSTypeCode(AnyType(n_elements, sup::dto::UnsignedInteger32)),
            ::pvxs::TypeCode::UInt32A);

  EXPECT_EQ(GetPVXSTypeCode(AnyType(TypeCode::Int64)), ::pvxs::TypeCode::Int64);
  EXPECT_EQ(GetPVXSTypeCode(AnyType(n_elements, sup::dto::SignedInteger64)),
            ::pvxs::TypeCode::Int64A);

  EXPECT_EQ(GetPVXSTypeCode(AnyType(TypeCode::UInt64)), ::pvxs::TypeCode::UInt64);
  EXPECT_EQ(GetPVXSTypeCode(AnyType(n_elements, sup::dto::UnsignedInteger64)),
            ::pvxs::TypeCode::UInt64A);

  EXPECT_EQ(GetPVXSTypeCode(AnyType(TypeCode::Float32)), ::pvxs::TypeCode::Float32);
  EXPECT_EQ(GetPVXSTypeCode(AnyType(n_elements, sup::dto::Float32)), ::pvxs::TypeCode::Float32A);

  EXPECT_EQ(GetPVXSTypeCode(AnyType(TypeCode::Float64)), ::pvxs::TypeCode::Float64);
  EXPECT_EQ(GetPVXSTypeCode(AnyType(n_elements, sup::dto::Float64)), ::pvxs::TypeCode::Float64A);

  EXPECT_EQ(GetPVXSTypeCode(AnyType(TypeCode::String)), ::pvxs::TypeCode::String);
  EXPECT_EQ(GetPVXSTypeCode(AnyType(n_elements, sup::dto::String)), ::pvxs::TypeCode::StringA);

  AnyType my_struct({{"signed", ::sup::dto::SignedInteger8}});
  EXPECT_EQ(GetPVXSTypeCode(my_struct), ::pvxs::TypeCode::Struct);
  EXPECT_EQ(GetPVXSTypeCode(AnyType(n_elements, my_struct)), ::pvxs::TypeCode::StructA);
}

//! Checks GetPVXSValueFromScalar helper method to construct PVXS value from scalar based AnyValue.

TEST_F(DtoConversionUtilsTest, GetPVXSValueFromScalar)
{
  {  // from Bool
    sup::dto::AnyValue any_value{sup::dto::Boolean};
    any_value = true;
    auto result = GetPVXSValueFromScalar(any_value);
    EXPECT_TRUE(result.valid());
    EXPECT_EQ(result.type(), ::pvxs::TypeCode::Bool);
    EXPECT_EQ(result.as<bool>(), true);
  }

  {  // from Char
    sup::dto::AnyValue any_value{sup::dto::Character8};
    any_value = 'w';
    auto result = GetPVXSValueFromScalar(any_value);
    EXPECT_TRUE(result.valid());
    EXPECT_EQ(result.type(), ::pvxs::TypeCode::UInt8);
    EXPECT_EQ(result.as<char>(), 'w');
  }

  {  // from Int8
    sup::dto::AnyValue any_value{sup::dto::SignedInteger8};
    any_value = 42;
    auto result = GetPVXSValueFromScalar(any_value);
    EXPECT_TRUE(result.valid());
    EXPECT_EQ(result.type(), ::pvxs::TypeCode::Int8);
    EXPECT_EQ(result.as<int8_t>(), 42);
  }

  {  // from UInt8
    sup::dto::AnyValue any_value{sup::dto::UnsignedInteger8};
    any_value = 42;
    auto result = GetPVXSValueFromScalar(any_value);
    EXPECT_TRUE(result.valid());
    EXPECT_EQ(result.type(), ::pvxs::TypeCode::UInt8);
    EXPECT_EQ(result.as<uint8_t>(), 42);
  }

  {  // from Int16
    sup::dto::AnyValue any_value{sup::dto::SignedInteger16};
    any_value = 42;
    auto result = GetPVXSValueFromScalar(any_value);
    EXPECT_TRUE(result.valid());
    EXPECT_EQ(result.type(), ::pvxs::TypeCode::Int16);
    EXPECT_EQ(result.as<int16_t>(), 42);
  }

  {  // from UInt16
    sup::dto::AnyValue any_value{sup::dto::UnsignedInteger16};
    any_value = 42;
    auto result = GetPVXSValueFromScalar(any_value);
    EXPECT_TRUE(result.valid());
    EXPECT_EQ(result.type(), ::pvxs::TypeCode::UInt16);
    EXPECT_EQ(result.as<uint16_t>(), 42);
  }

  {  // from Int32
    sup::dto::AnyValue any_value{sup::dto::SignedInteger32};
    any_value = 42;
    auto result = GetPVXSValueFromScalar(any_value);
    EXPECT_TRUE(result.valid());
    EXPECT_EQ(result.type(), ::pvxs::TypeCode::Int32);
    EXPECT_EQ(result.as<int32_t>(), 42);
  }

  {  // from UInt32
    sup::dto::AnyValue any_value{sup::dto::UnsignedInteger32};
    any_value = 42;
    auto result = GetPVXSValueFromScalar(any_value);
    EXPECT_TRUE(result.valid());
    EXPECT_EQ(result.type(), ::pvxs::TypeCode::UInt32);
    EXPECT_EQ(result.as<uint32_t>(), 42);
  }

  {  // from Int64
    sup::dto::AnyValue any_value{sup::dto::SignedInteger64};
    any_value = 42;
    auto result = GetPVXSValueFromScalar(any_value);
    EXPECT_TRUE(result.valid());
    EXPECT_EQ(result.type(), ::pvxs::TypeCode::Int64);
    EXPECT_EQ(result.as<int64_t>(), 42);
  }

  {  // from UInt64
    sup::dto::AnyValue any_value{sup::dto::UnsignedInteger64};
    any_value = 42;
    auto result = GetPVXSValueFromScalar(any_value);
    EXPECT_TRUE(result.valid());
    EXPECT_EQ(result.type(), ::pvxs::TypeCode::UInt64);
    EXPECT_EQ(result.as<uint64_t>(), 42);
  }

  {  // from Float32
    sup::dto::AnyValue any_value{sup::dto::Float32};
    any_value = 42.1;
    auto result = GetPVXSValueFromScalar(any_value);
    EXPECT_TRUE(result.valid());
    EXPECT_EQ(result.type(), ::pvxs::TypeCode::Float32);
    EXPECT_FLOAT_EQ(result.as<float>(), 42.1);
  }

  {  // from Float64
    sup::dto::AnyValue any_value{sup::dto::Float64};
    any_value = 42.1;
    auto result = GetPVXSValueFromScalar(any_value);
    EXPECT_TRUE(result.valid());
    EXPECT_EQ(result.type(), ::pvxs::TypeCode::Float64);
    EXPECT_DOUBLE_EQ(result.as<double>(), 42.1);
  }

  {  // from string
    sup::dto::AnyValue any_value{sup::dto::String};
    any_value = std::string("abc");
    auto result = GetPVXSValueFromScalar(any_value);
    EXPECT_TRUE(result.valid());
    EXPECT_EQ(result.type(), ::pvxs::TypeCode::String);
    EXPECT_EQ(result.as<std::string>(), std::string("abc"));
  }

  {  // from long string
    sup::dto::AnyValue any_value{sup::dto::String};
    any_value = std::string(1025, 'a');
    auto result = GetPVXSValueFromScalar(any_value);
    EXPECT_TRUE(result.valid());
    EXPECT_EQ(result.type(), ::pvxs::TypeCode::String);
    EXPECT_EQ(result.as<std::string>(), std::string(1025, 'a'));
  }

  {  // attempt to construct from AnyValue based on structure
    sup::dto::AnyValue any_value = {
        {{"signed", {sup::dto::SignedInteger32, 42}}, {"bool", {sup::dto::Boolean, true}}}};
    EXPECT_THROW(GetPVXSValueFromScalar(any_value), std::runtime_error);
  }
}
