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

#include "AnyValue.h"
#include "sup/epics/dtoconversionutils.h"

#include <gtest/gtest.h>
#include <pvxs/data.h>
#include <pvxs/nt.h>

using namespace ::sup::epics;

class DtoConversionUtilsTest : public ::testing::Test
{
};

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
