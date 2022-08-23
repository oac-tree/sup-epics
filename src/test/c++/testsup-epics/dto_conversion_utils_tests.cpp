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
#include <sup/epics/dto_conversion_utils.h>

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
