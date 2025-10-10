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

#include <sup/dto/anyvalue.h>
#include <sup/dto/anyvalue_helper.h>
#include <sup/epics/utils/dto_conversion_utils.h>

#include <gtest/gtest.h>
#include <pvxs/nt.h>

using namespace ::sup::epics;

//! Collection of extended tests where we convert complex AnyValue to PVXS and back.
class AnyValueToPvxsAndBacExtendedTest : public ::testing::Test
{
};

//! Copying array of structs (with array of struct) from AnyValue to PVXS and back.
//! array
//!    0: struct external_struct
//!       array_field: array
//!            0: struct internal_struct
//!                int_field: int32 42
//!            1: struct internal_struct
//!                int_field: int32 43
//!    1: struct external_struct
//!        array_field: array
//!            0: struct internal_struct
//!                int_field: int32 44
//!            1: struct internal_struct
//!                int_field: int32 45
TEST_F(AnyValueToPvxsAndBacExtendedTest, ArrayWithStructContainingArrayOfStruct)
{
  const std::string internal_struct_name("internal_struct");
  const std::string external_struct_name("external_struct");
  // array names do not exist in PVXS, we should use empty name so the equality operator below works
  const std::string deliberately_empty_array_name;

  // two internal structs of the same type
  const sup::dto::AnyValue intern_struct_value1 = {
      {{"int_field", {sup::dto::SignedInteger32Type, 42}}}, internal_struct_name};
  const sup::dto::AnyValue intern_struct_value2 = {
      {{"int_field", {sup::dto::SignedInteger32Type, 43}}}, internal_struct_name};

  // array with two elements
  auto array_value = sup::dto::ArrayValue({intern_struct_value1, intern_struct_value2},
                                          deliberately_empty_array_name);

  // a structure which includes our array
  const sup::dto::AnyValue extern_struct_value1 = {{{"array_field", array_value}},
                                                   external_struct_name};

  // another structure with array and modified values
  sup::dto::AnyValue extern_struct_value2 = {{{"array_field", array_value}}, external_struct_name};
  extern_struct_value2["array_field"][0]["int_field"] = 44;
  extern_struct_value2["array_field"][1]["int_field"] = 45;

  // Resulting array containing two structures (each containing a field with array of structures)
  auto anyvalue = sup::dto::ArrayValue({extern_struct_value1, extern_struct_value2},
                                       deliberately_empty_array_name);

  auto pvxs_value = BuildPVXSValue(anyvalue);

  auto from_pvxs_anyvalue = BuildAnyValue(pvxs_value);

  EXPECT_EQ(anyvalue, from_pvxs_anyvalue);
}

//! struct outside_struct_name
//!  ExternalArrayField: array
//!      0: struct internal_struct2
//!          StructField: struct struct_with_array
//!              ArrayField: array
//!                  0: int32 42
//!                  1: int32 43
//!      1: struct internal_struct2
//!          StructField: struct struct_with_array
//!              ArrayField: array
//!                  0: int32 42
//!                  1: int32 43
TEST_F(AnyValueToPvxsAndBacExtendedTest, StructWithArrayOfStructOfStructArrayOfScalar)
{
  const int n_elements = 2;
  sup::dto::AnyValue array_of_scalars(n_elements, sup::dto::SignedInteger32Type);
  array_of_scalars[0] = 42;
  array_of_scalars[1] = 43;

  const std::string internal_struct_name("struct_with_array");
  const sup::dto::AnyValue struct_array_of_scalar = {{{"ArrayField", array_of_scalars}},
                                                     internal_struct_name};

  const std::string internal_struct_name2("internal_struct2");
  const sup::dto::AnyValue struct_struct_array_of_scalar1 = {
      {{"StructField", struct_array_of_scalar}}, internal_struct_name2};
  const sup::dto::AnyValue struct_struct_array_of_scalar2 = {
      {{"StructField", struct_array_of_scalar}}, internal_struct_name2};

  // array names do not exist in PVXS, we should use empty name so the equality operator below works
  const std::string deliberately_empty_array_name;
  auto external_array =
      sup::dto::ArrayValue({struct_struct_array_of_scalar1, struct_struct_array_of_scalar2},
                           deliberately_empty_array_name);

  const std::string outside_struct_name("outside_struct_name");
  const sup::dto::AnyValue outside_struct = {{{"ExternalArrayField", external_array}},
                                             outside_struct_name};

  auto pvxs_value = BuildPVXSValue(outside_struct);

  auto from_pvxs_anyvalue = BuildAnyValue(pvxs_value);

  EXPECT_EQ(outside_struct, from_pvxs_anyvalue);
}
