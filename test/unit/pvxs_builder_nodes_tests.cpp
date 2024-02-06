/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Gennady Pospelov (IO)
 *
 * Copyright (c) : 2010-2024 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 *****************************************************************************/

#include "sup/epics/utils/pvxs_builder_nodes.h"

#include <gtest/gtest.h>
#include <pvxs/data.h>
#include <pvxs/nt.h>
#include <sup/dto/anyvalue.h>
#include <sup/epics/utils/dto_conversion_utils.h>
#include <sup/epics/utils/pvxs_utils.h>

#include <iostream>

using namespace ::sup::epics;

//! Testing builder nodes defined in `pvxs_builder_nodes.h`

class PvxsBuilderNodesTests : public ::testing::Test
{
};

TEST_F(PvxsBuilderNodesTests, ScalarArrayBuilderNodeInitialState)
{
  sup::dto::AnyValue any_value =
      sup::dto::ArrayValue({{sup::dto::SignedInteger32Type, 1}, 2}, "my_array_t");

  auto pvxs_value = pvxs::TypeDef(pvxs::TypeCode::Int32A).create();

  ScalarArrayBuilderNode node(pvxs_value, &any_value);
  EXPECT_TRUE(node.IsScalarArrayNode());
  EXPECT_FALSE(node.IsStructArrayNode());

  auto result = node.GetPvxsValueRef();
  EXPECT_EQ(result.type(), ::pvxs::TypeCode::Int32A);
  auto data = result.as<::pvxs::shared_array<const int32_t>>();
  EXPECT_EQ(data.size(), 2);
  EXPECT_EQ(data[0], 1);
  EXPECT_EQ(data[1], 2);

  // attempt to create node from wrong type (not an array)
  EXPECT_THROW(
      ScalarArrayBuilderNode(::pvxs::TypeDef(::pvxs::TypeCode::Int32).create(), &any_value),
      std::runtime_error);
}

//! Testing class StructArrayBuilderNode in the

TEST_F(PvxsBuilderNodesTests, StructArrayBuilderNodeAddElement)
{
  // preparing any_value
  sup::dto::AnyValue struct_value1 = {{{"field_name", {sup::dto::SignedInteger32Type, 42}}},
                                      "struct_name"};
  sup::dto::AnyValue struct_value2 = {{{"field_name", {sup::dto::SignedInteger32Type, 43}}},
                                      "struct_name"};
  auto any_value = sup::dto::ArrayValue({{struct_value1}, struct_value2});

  // defining pvxs_value (no values assignedm structs are unallocated)
  auto pvxs_value = ::pvxs::TypeDef(::pvxs::TypeCode::StructA, "struct_name",
                                    {pvxs::members::Int32("field_name")})
                        .create();

  StructArrayBuilderNode node(pvxs_value, &any_value);
  EXPECT_FALSE(node.IsScalarArrayNode());
  EXPECT_TRUE(node.IsStructArrayNode());

  auto& pvxs_value_element0 = node.GetPvxsValueRef();
  pvxs_value_element0["field_name"] = 42;

  node.ArrayElementSeparator();

  auto& pvxs_value_element1 = node.GetPvxsValueRef();
  pvxs_value_element1["field_name"] = 43;

  EXPECT_TRUE(&pvxs_value_element0 != &pvxs_value_element1);

  node.Freeze();

  // retrieving result
  auto pvxs_result = node.GetPvxsValue();
  EXPECT_EQ(pvxs_result.type(), pvxs::TypeCode::StructA);
  auto array_data = pvxs_value.as<pvxs::shared_array<const pvxs::Value>>();
  EXPECT_EQ(array_data.size(), 2);
  EXPECT_EQ(array_data[0]["field_name"].as<int32_t>(), 42);
  EXPECT_EQ(array_data[1]["field_name"].as<int32_t>(), 43);
}
