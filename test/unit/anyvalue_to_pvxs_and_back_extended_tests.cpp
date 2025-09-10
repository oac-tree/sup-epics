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
