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

#include <gtest/gtest.h>
#include <pvxs/data.h>
#include <pvxs/nt.h>
#include <sup/dto/anyvalue.h>
#include <sup/epics/utils/dto_typecode_conversion_utils.h>

using namespace ::sup::epics;

class DtoTypeCodeConversionUtilsTests : public ::testing::Test
{
};

//! Checks GetPVXSBaseTypeCode method to construct PVXS TypeCode from AnyType (base types).

TEST_F(DtoTypeCodeConversionUtilsTests, GetPVXSBaseTypeCode)
{
  EXPECT_EQ(GetPVXSBaseTypeCode(sup::dto::EmptyType), ::pvxs::TypeCode::Null);
  EXPECT_EQ(GetPVXSBaseTypeCode(sup::dto::BooleanType), ::pvxs::TypeCode::Bool);
  EXPECT_EQ(GetPVXSBaseTypeCode(sup::dto::Character8Type), ::pvxs::TypeCode::UInt8);
  EXPECT_EQ(GetPVXSBaseTypeCode(sup::dto::SignedInteger8Type), ::pvxs::TypeCode::Int8);
  EXPECT_EQ(GetPVXSBaseTypeCode(sup::dto::UnsignedInteger8Type), ::pvxs::TypeCode::UInt8);
  EXPECT_EQ(GetPVXSBaseTypeCode(sup::dto::SignedInteger16Type), ::pvxs::TypeCode::Int16);
  EXPECT_EQ(GetPVXSBaseTypeCode(sup::dto::UnsignedInteger16Type), ::pvxs::TypeCode::UInt16);
  EXPECT_EQ(GetPVXSBaseTypeCode(sup::dto::SignedInteger32Type), ::pvxs::TypeCode::Int32);
  EXPECT_EQ(GetPVXSBaseTypeCode(sup::dto::UnsignedInteger32Type), ::pvxs::TypeCode::UInt32);
  EXPECT_EQ(GetPVXSBaseTypeCode(sup::dto::SignedInteger64Type), ::pvxs::TypeCode::Int64);
  EXPECT_EQ(GetPVXSBaseTypeCode(sup::dto::UnsignedInteger64Type), ::pvxs::TypeCode::UInt64);
  EXPECT_EQ(GetPVXSBaseTypeCode(sup::dto::Float32Type), ::pvxs::TypeCode::Float32);
  EXPECT_EQ(GetPVXSBaseTypeCode(sup::dto::Float64Type), ::pvxs::TypeCode::Float64);
  EXPECT_EQ(GetPVXSBaseTypeCode(sup::dto::StringType), ::pvxs::TypeCode::String);

  {  // AnyType based on struct
    const ::sup::dto::AnyType any_type{{{"signed", ::sup::dto::SignedInteger8Type}}};
    EXPECT_EQ(GetPVXSBaseTypeCode(any_type), ::pvxs::TypeCode::Struct);
  }

  {  // attempt to get unknown type code
    const sup::dto::AnyType any_type(64, sup::dto::UnsignedInteger32Type);
    EXPECT_THROW(GetPVXSBaseTypeCode(any_type), std::runtime_error);
  }
}

//! Checks GetPVXSArrayTypeCode method to construct PVXS TypeCode from AnyType (array elements).

TEST_F(DtoTypeCodeConversionUtilsTests, GetPVXSArrayTypeCode)
{
  EXPECT_EQ(GetPVXSArrayTypeCode(sup::dto::BooleanType), ::pvxs::TypeCode::BoolA);
  EXPECT_EQ(GetPVXSArrayTypeCode(sup::dto::Character8Type), ::pvxs::TypeCode::UInt8A);
  EXPECT_EQ(GetPVXSArrayTypeCode(sup::dto::SignedInteger8Type), ::pvxs::TypeCode::Int8A);
  EXPECT_EQ(GetPVXSArrayTypeCode(sup::dto::UnsignedInteger8Type), ::pvxs::TypeCode::UInt8A);
  EXPECT_EQ(GetPVXSArrayTypeCode(sup::dto::SignedInteger16Type), ::pvxs::TypeCode::Int16A);
  EXPECT_EQ(GetPVXSArrayTypeCode(sup::dto::UnsignedInteger16Type), ::pvxs::TypeCode::UInt16A);
  EXPECT_EQ(GetPVXSArrayTypeCode(sup::dto::SignedInteger32Type), ::pvxs::TypeCode::Int32A);
  EXPECT_EQ(GetPVXSArrayTypeCode(sup::dto::UnsignedInteger32Type), ::pvxs::TypeCode::UInt32A);
  EXPECT_EQ(GetPVXSArrayTypeCode(sup::dto::SignedInteger64Type), ::pvxs::TypeCode::Int64A);
  EXPECT_EQ(GetPVXSArrayTypeCode(sup::dto::UnsignedInteger64Type), ::pvxs::TypeCode::UInt64A);
  EXPECT_EQ(GetPVXSArrayTypeCode(sup::dto::Float32Type), ::pvxs::TypeCode::Float32A);
  EXPECT_EQ(GetPVXSArrayTypeCode(sup::dto::Float64Type), ::pvxs::TypeCode::Float64A);
  EXPECT_EQ(GetPVXSArrayTypeCode(sup::dto::StringType), ::pvxs::TypeCode::StringA);

  {  // AnyType based on struct
    const ::sup::dto::AnyType any_type{{{"signed", ::sup::dto::SignedInteger8Type}}};
    EXPECT_EQ(GetPVXSArrayTypeCode(any_type), ::pvxs::TypeCode::StructA);
  }
}

//! Checks GetPVXSTypeCode method to construct PVXS TypeCode from AnyType.
//! Checks scalars and array of scalars.

TEST_F(DtoTypeCodeConversionUtilsTests, GetPVXSTypeCode)
{
  const int n_elements{42};
  using ::sup::dto::AnyType;
  using ::sup::dto::TypeCode;

  EXPECT_EQ(GetPVXSTypeCode(AnyType(TypeCode::Bool)), ::pvxs::TypeCode::Bool);
  EXPECT_EQ(GetPVXSTypeCode(AnyType(n_elements, sup::dto::BooleanType)), ::pvxs::TypeCode::BoolA);

  EXPECT_EQ(GetPVXSTypeCode(AnyType(TypeCode::UInt8)), ::pvxs::TypeCode::UInt8);
  EXPECT_EQ(GetPVXSTypeCode(AnyType(n_elements, sup::dto::Character8Type)),
            ::pvxs::TypeCode::UInt8A);

  EXPECT_EQ(GetPVXSTypeCode(AnyType(TypeCode::Int8)), ::pvxs::TypeCode::Int8);
  EXPECT_EQ(GetPVXSTypeCode(AnyType(n_elements, sup::dto::SignedInteger8Type)),
            ::pvxs::TypeCode::Int8A);

  EXPECT_EQ(GetPVXSTypeCode(AnyType(TypeCode::UInt8)), ::pvxs::TypeCode::UInt8);
  EXPECT_EQ(GetPVXSTypeCode(AnyType(n_elements, sup::dto::UnsignedInteger8Type)),
            ::pvxs::TypeCode::UInt8A);

  EXPECT_EQ(GetPVXSTypeCode(AnyType(TypeCode::Int16)), ::pvxs::TypeCode::Int16);
  EXPECT_EQ(GetPVXSTypeCode(AnyType(n_elements, sup::dto::SignedInteger16Type)),
            ::pvxs::TypeCode::Int16A);

  EXPECT_EQ(GetPVXSTypeCode(AnyType(TypeCode::UInt16)), ::pvxs::TypeCode::UInt16);
  EXPECT_EQ(GetPVXSTypeCode(AnyType(n_elements, sup::dto::UnsignedInteger16Type)),
            ::pvxs::TypeCode::UInt16A);

  EXPECT_EQ(GetPVXSTypeCode(AnyType(TypeCode::Int32)), ::pvxs::TypeCode::Int32);
  EXPECT_EQ(GetPVXSTypeCode(AnyType(n_elements, sup::dto::SignedInteger32Type)),
            ::pvxs::TypeCode::Int32A);

  EXPECT_EQ(GetPVXSTypeCode(AnyType(TypeCode::UInt32)), ::pvxs::TypeCode::UInt32);
  EXPECT_EQ(GetPVXSTypeCode(AnyType(n_elements, sup::dto::UnsignedInteger32Type)),
            ::pvxs::TypeCode::UInt32A);

  EXPECT_EQ(GetPVXSTypeCode(AnyType(TypeCode::Int64)), ::pvxs::TypeCode::Int64);
  EXPECT_EQ(GetPVXSTypeCode(AnyType(n_elements, sup::dto::SignedInteger64Type)),
            ::pvxs::TypeCode::Int64A);

  EXPECT_EQ(GetPVXSTypeCode(AnyType(TypeCode::UInt64)), ::pvxs::TypeCode::UInt64);
  EXPECT_EQ(GetPVXSTypeCode(AnyType(n_elements, sup::dto::UnsignedInteger64Type)),
            ::pvxs::TypeCode::UInt64A);

  EXPECT_EQ(GetPVXSTypeCode(AnyType(TypeCode::Float32)), ::pvxs::TypeCode::Float32);
  EXPECT_EQ(GetPVXSTypeCode(AnyType(n_elements, sup::dto::Float32Type)),
            ::pvxs::TypeCode::Float32A);

  EXPECT_EQ(GetPVXSTypeCode(AnyType(TypeCode::Float64)), ::pvxs::TypeCode::Float64);
  EXPECT_EQ(GetPVXSTypeCode(AnyType(n_elements, sup::dto::Float64Type)),
            ::pvxs::TypeCode::Float64A);

  EXPECT_EQ(GetPVXSTypeCode(AnyType(TypeCode::String)), ::pvxs::TypeCode::String);
  EXPECT_EQ(GetPVXSTypeCode(AnyType(n_elements, sup::dto::StringType)), ::pvxs::TypeCode::StringA);

  const AnyType my_struct({{"signed", ::sup::dto::SignedInteger8Type}});
  EXPECT_EQ(GetPVXSTypeCode(my_struct), ::pvxs::TypeCode::Struct);
  EXPECT_EQ(GetPVXSTypeCode(AnyType(n_elements, my_struct)), ::pvxs::TypeCode::StructA);
}

//! Checks GetPVXSBaseTypeCode method to construct PVXS TypeCode from AnyType (base types).

TEST_F(DtoTypeCodeConversionUtilsTests, GetAnyTypeCode)
{
  // scalar case
  EXPECT_EQ(GetAnyTypeCode(::pvxs::TypeCode::Null), ::sup::dto::TypeCode::Empty);
  EXPECT_EQ(GetAnyTypeCode(::pvxs::TypeCode::Bool), ::sup::dto::TypeCode::Bool);
  EXPECT_EQ(GetAnyTypeCode(::pvxs::TypeCode::Int8), ::sup::dto::TypeCode::Int8);
  EXPECT_EQ(GetAnyTypeCode(::pvxs::TypeCode::UInt8), ::sup::dto::TypeCode::UInt8);

  EXPECT_EQ(GetAnyTypeCode(::pvxs::TypeCode::Int16), ::sup::dto::TypeCode::Int16);
  EXPECT_EQ(GetAnyTypeCode(::pvxs::TypeCode::UInt16), ::sup::dto::TypeCode::UInt16);

  EXPECT_EQ(GetAnyTypeCode(::pvxs::TypeCode::Int32), ::sup::dto::TypeCode::Int32);
  EXPECT_EQ(GetAnyTypeCode(::pvxs::TypeCode::UInt32), ::sup::dto::TypeCode::UInt32);

  EXPECT_EQ(GetAnyTypeCode(::pvxs::TypeCode::Int64), ::sup::dto::TypeCode::Int64);
  EXPECT_EQ(GetAnyTypeCode(::pvxs::TypeCode::UInt64), ::sup::dto::TypeCode::UInt64);

  EXPECT_EQ(GetAnyTypeCode(::pvxs::TypeCode::Float32), ::sup::dto::TypeCode::Float32);
  EXPECT_EQ(GetAnyTypeCode(::pvxs::TypeCode::Float64), ::sup::dto::TypeCode::Float64);

  EXPECT_EQ(GetAnyTypeCode(::pvxs::TypeCode::String), ::sup::dto::TypeCode::String);

  // array case
  EXPECT_EQ(GetAnyTypeCode(::pvxs::TypeCode::BoolA), ::sup::dto::TypeCode::Bool);
  EXPECT_EQ(GetAnyTypeCode(::pvxs::TypeCode::Int8A), ::sup::dto::TypeCode::Int8);
  EXPECT_EQ(GetAnyTypeCode(::pvxs::TypeCode::UInt8A), ::sup::dto::TypeCode::UInt8);

  EXPECT_EQ(GetAnyTypeCode(::pvxs::TypeCode::Int16A), ::sup::dto::TypeCode::Int16);
  EXPECT_EQ(GetAnyTypeCode(::pvxs::TypeCode::UInt16A), ::sup::dto::TypeCode::UInt16);

  EXPECT_EQ(GetAnyTypeCode(::pvxs::TypeCode::Int32A), ::sup::dto::TypeCode::Int32);
  EXPECT_EQ(GetAnyTypeCode(::pvxs::TypeCode::UInt32A), ::sup::dto::TypeCode::UInt32);

  EXPECT_EQ(GetAnyTypeCode(::pvxs::TypeCode::Int64A), ::sup::dto::TypeCode::Int64);
  EXPECT_EQ(GetAnyTypeCode(::pvxs::TypeCode::UInt64A), ::sup::dto::TypeCode::UInt64);

  EXPECT_EQ(GetAnyTypeCode(::pvxs::TypeCode::Float32A), ::sup::dto::TypeCode::Float32);
  EXPECT_EQ(GetAnyTypeCode(::pvxs::TypeCode::Float64A), ::sup::dto::TypeCode::Float64);

  EXPECT_EQ(GetAnyTypeCode(::pvxs::TypeCode::StringA), ::sup::dto::TypeCode::String);
}
