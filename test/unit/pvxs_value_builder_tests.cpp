/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Gennady Pospelov (IO)
 *
 * Copyright (c) : 2010-2026 ITER Organization,
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

#include "sup/epics/utils/pvxs_value_builder.h"

#include <sup/dto/anyvalue.h>
#include <sup/epics/utils/dto_conversion_utils.h>
#include <sup/epics/utils/pvxs_utils.h>

#include <gtest/gtest.h>
#include <pvxs/data.h>
#include <pvxs/nt.h>

using namespace ::sup::epics;

//! Testing various methods of PVXSValueBuilder.
//! See integration tests in `pvxs_value_builder_extended_tests.cpp`.

class PvxsValueBuilderTests : public ::testing::Test
{
};

//! Build PVXS value from empty AnyValue.

TEST_F(PvxsValueBuilderTests, FromEmptyType)
{
  const pvxs::TypeDef empty_type;

  const PvxsValueBuilder builder(empty_type);
  auto pvxs_value = builder.GetPVXSValue();

  EXPECT_FALSE(pvxs_value.valid());
  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Null);
}

//! Build PVXS value from a scalar AnyValue.

TEST_F(PvxsValueBuilderTests, FromScalar)
{
  // original any_value
  const sup::dto::AnyValue any_value{sup::dto::SignedInteger32Type, 42};

  // initializing builder
  auto pvxs_type = ::pvxs::TypeDef(::pvxs::TypeCode::Int32);
  PvxsValueBuilder builder(pvxs_type);

  // building PVXS value
  builder.ScalarProlog(&any_value);
  builder.ScalarEpilog(&any_value);

  auto pvxs_value = builder.GetPVXSValue();

  // validating pvxs_value
  EXPECT_TRUE(pvxs_value.valid());
  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value.as<int32_t>(), 42);
}

//! Build PVXS value from a structure with single field.

TEST_F(PvxsValueBuilderTests, StructWithSingleField)
{
  // original any_value
  sup::dto::AnyValue any_value = {{{"signed", {sup::dto::SignedInteger32Type, 42}}}};

  // initializing builder
  auto pvxs_type = ::pvxs::TypeDef(::pvxs::TypeCode::Struct, {pvxs::members::Int32("signed")});
  PvxsValueBuilder builder(pvxs_type);

  // building PVXS value
  builder.StructProlog(&any_value);
  builder.MemberProlog(&any_value["signed"], "signed");
  builder.ScalarProlog(&any_value["signed"]);
  builder.ScalarEpilog(&any_value["signed"]);
  builder.MemberEpilog(&any_value["signed"], "signed");
  builder.StructEpilog(&any_value);

  auto pvxs_value = builder.GetPVXSValue();

  // validating pvxs_value
  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Struct);
  EXPECT_EQ(pvxs_value.nmembers(), 1);

  auto names = GetMemberNames(pvxs_value);
  EXPECT_EQ(names, std::vector<std::string>({"signed"}));
  EXPECT_EQ(pvxs_value["signed"].type(), ::pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value["signed"].as<int32_t>(), 42);
}

//! Build PVXS value from a structure with two fields.

TEST_F(PvxsValueBuilderTests, StructWithTwoFields)
{
  // original any_value
  sup::dto::AnyValue any_value = {{"signed", {sup::dto::SignedInteger32Type, 42}},
                                  {"bool", {sup::dto::BooleanType, true}}};

  // initializing builder
  auto pvxs_type = ::pvxs::TypeDef(::pvxs::TypeCode::Struct,
                                   {pvxs::members::Int32("signed"), pvxs::members::Bool("bool")});
  PvxsValueBuilder builder(pvxs_type);

  // building PVXS value
  builder.StructProlog(&any_value);
  builder.MemberProlog(&any_value["signed"], "signed");
  builder.ScalarProlog(&any_value["signed"]);
  builder.ScalarEpilog(&any_value["signed"]);
  builder.MemberEpilog(&any_value["signed"], "signed");
  builder.StructMemberSeparator();
  builder.MemberProlog(&any_value["bool"], "bool");
  builder.ScalarProlog(&any_value["bool"]);
  builder.ScalarEpilog(&any_value["bool"]);
  builder.MemberEpilog(&any_value["bool"], "bool");
  builder.StructEpilog(&any_value);

  auto pvxs_value = builder.GetPVXSValue();

  // validating pvxs_value
  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Struct);
  EXPECT_EQ(pvxs_value.nmembers(), 2);

  auto names = GetMemberNames(pvxs_value);
  EXPECT_EQ(names, std::vector<std::string>({"signed", "bool"}));

  EXPECT_EQ(pvxs_value["signed"].type(), ::pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value["signed"].as<int32_t>(), 42);
  EXPECT_EQ(pvxs_value["bool"].type(), ::pvxs::TypeCode::Bool);
  EXPECT_EQ(pvxs_value["bool"].as<bool>(), true);
}

//! Build PVXS value from a structure with two fields inside parent structure.

TEST_F(PvxsValueBuilderTests, NestedStruct)
{
  // original any_value
  sup::dto::AnyValue two_scalars = {{"signed", {sup::dto::SignedInteger32Type, 42}},
                                    {"bool", {sup::dto::BooleanType, true}}};
  sup::dto::AnyValue any_value = {{"scalars", two_scalars}};

  // initialising builder
  auto pvxs_type = ::pvxs::TypeDef(
      ::pvxs::TypeCode::Struct, {pvxs::members::Struct("scalars", {pvxs::members::Int32("signed"),
                                                                   pvxs::members::Bool("bool")})});
  PvxsValueBuilder builder(pvxs_type);

  // building PVXS value
  builder.StructProlog(&any_value);
  builder.MemberProlog(&any_value["scalars"], "scalars");

  builder.StructProlog(&any_value["scalars"]);
  builder.MemberProlog(&any_value["scalars.signed"], "signed");
  builder.ScalarProlog(&any_value["scalars.signed"]);
  builder.ScalarEpilog(&any_value["scalars.signed"]);
  builder.MemberEpilog(&any_value["scalars.signed"], "signed");
  builder.StructMemberSeparator();
  builder.MemberProlog(&any_value["scalars.bool"], "bool");
  builder.ScalarProlog(&any_value["scalars.bool"]);
  builder.ScalarEpilog(&any_value["scalars.bool"]);
  builder.MemberEpilog(&any_value["scalars.bool"], "bool");
  builder.StructEpilog(&any_value["scalars"]);

  builder.MemberEpilog(&any_value["scalars"], "scalars");
  builder.StructEpilog(&any_value);

  auto pvxs_value = builder.GetPVXSValue();

  // validating pvxs_value
  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Struct);
  EXPECT_EQ(pvxs_value.nmembers(), 1);

  auto names = GetMemberNames(pvxs_value);
  EXPECT_EQ(names, std::vector<std::string>({"scalars"}));

  auto nested_value = pvxs_value["scalars"];
  EXPECT_EQ(nested_value.type(), ::pvxs::TypeCode::Struct);
  EXPECT_EQ(nested_value.nmembers(), 2);

  auto nested_names = GetMemberNames(nested_value);
  EXPECT_EQ(nested_names, std::vector<std::string>({"signed", "bool"}));

  EXPECT_EQ(pvxs_value["scalars.signed"].type(), ::pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value["scalars.bool"].type(), ::pvxs::TypeCode::Bool);

  EXPECT_EQ(pvxs_value["scalars.signed"].as<int32_t>(), 42);
  EXPECT_EQ(pvxs_value["scalars.bool"].as<bool>(), true);
}

//! Build PVXS value from a structure with two fields inside parent structure.

TEST_F(PvxsValueBuilderTests, TwoNestedStructs)
{
  // original any_value
  const std::string struct_name = "struct_name";
  sup::dto::AnyValue two_scalars = {{"signed", {sup::dto::SignedInteger32Type, 42}},
                                    {"bool", {sup::dto::BooleanType, true}}};

  sup::dto::AnyValue any_value{{{"struct1", two_scalars},
                                {"struct2",
                                 {{"first", {sup::dto::SignedInteger8Type, -43}},
                                  {"second", {sup::dto::UnsignedInteger8Type, 44}}}}},
                               struct_name};

  // initializing builder
  auto member1 = pvxs::members::Struct(
      "struct1", "struct1_name", {pvxs::members::Int32("signed"), pvxs::members::Bool("bool")});
  auto member2 = pvxs::members::Struct(
      "struct2", {pvxs::members::Int8("first"), pvxs::members::UInt8("second")});

  auto pvxs_type = ::pvxs::TypeDef(::pvxs::TypeCode::Struct, struct_name, {member1, member2});
  PvxsValueBuilder builder(pvxs_type);

  // building PVXS value
  builder.StructProlog(&any_value);

  builder.MemberProlog(&any_value["struct1"], "struct1");
  builder.StructProlog(&any_value["struct1"]);
  builder.MemberProlog(&any_value["struct1.signed"], "signed");
  builder.ScalarProlog(&any_value["struct1.signed"]);
  builder.ScalarEpilog(&any_value["struct1.signed"]);
  builder.MemberEpilog(&any_value["struct1.signed"], "signed");
  builder.StructMemberSeparator();
  builder.MemberProlog(&any_value["struct1.bool"], "bool");
  builder.ScalarProlog(&any_value["struct1.bool"]);
  builder.ScalarEpilog(&any_value["struct1.bool"]);
  builder.MemberEpilog(&any_value["struct1.bool"], "bool");
  builder.StructEpilog(&any_value["struct1"]);
  builder.MemberEpilog(&any_value["struct1"], "struct1");

  builder.StructMemberSeparator();

  builder.MemberProlog(&any_value["struct2"], "struct2");
  builder.StructProlog(&any_value["struct2"]);
  builder.MemberProlog(&any_value["struct2.first"], "first");
  builder.ScalarProlog(&any_value["struct2.first"]);
  builder.ScalarEpilog(&any_value["struct2.first"]);
  builder.MemberEpilog(&any_value["struct2.first"], "first");
  builder.StructMemberSeparator();
  builder.MemberProlog(&any_value["struct2.second"], "second");
  builder.ScalarProlog(&any_value["struct2.second"]);
  builder.ScalarEpilog(&any_value["struct2.second"]);
  builder.MemberEpilog(&any_value["struct2.second"], "second");
  builder.StructEpilog(&any_value["struct2"]);
  builder.MemberEpilog(&any_value["struct2"], "struct2");

  builder.StructEpilog(&any_value);

  auto pvxs_value = builder.GetPVXSValue();

  // validating PVXS value
  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Struct);
  EXPECT_EQ(pvxs_value.nmembers(), 2);
  EXPECT_EQ(pvxs_value.id(), std::string("struct_name"));

  auto names = GetMemberNames(pvxs_value);
  EXPECT_EQ(names, std::vector<std::string>({"struct1", "struct2"}));

  // first branch
  auto struct1_value = pvxs_value["struct1"];
  EXPECT_EQ(struct1_value.type(), ::pvxs::TypeCode::Struct);
  EXPECT_EQ(struct1_value.nmembers(), 2);

  auto struct1_fields = GetMemberNames(struct1_value);
  EXPECT_EQ(struct1_fields, std::vector<std::string>({"signed", "bool"}));
  EXPECT_EQ(pvxs_value["struct1.signed"].type(), ::pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value["struct1.bool"].type(), ::pvxs::TypeCode::Bool);

  EXPECT_EQ(pvxs_value["struct1.signed"].as<int32_t>(), 42);
  EXPECT_EQ(pvxs_value["struct1.bool"].as<bool>(), true);

  // second branch
  auto struct2_value = pvxs_value["struct2"];
  EXPECT_EQ(struct2_value.type(), ::pvxs::TypeCode::Struct);
  EXPECT_EQ(struct2_value.nmembers(), 2);

  auto struct2_fields = GetMemberNames(struct2_value);
  EXPECT_EQ(struct2_fields, std::vector<std::string>({"first", "second"}));
  EXPECT_EQ(pvxs_value["struct2.first"].type(), ::pvxs::TypeCode::Int8);
  EXPECT_EQ(pvxs_value["struct2.second"].type(), ::pvxs::TypeCode::UInt8);
  EXPECT_EQ(pvxs_value["struct2.first"].as<int32_t>(), -43);
  EXPECT_EQ(pvxs_value["struct2.second"].as<uint32_t>(), 44);
}

//! Build PVXS value from AnyValue representing an array of integers.

TEST_F(PvxsValueBuilderTests, ArrayOfIntegers)
{
  // original anyvalue
  const int n_elements = 2;
  sup::dto::AnyValue any_value(n_elements, sup::dto::SignedInteger32Type);
  any_value[0] = 42;

  // initializing builder
  auto pvxs_type = pvxs::TypeDef(pvxs::TypeCode::Int32A);
  PvxsValueBuilder builder(pvxs_type);

  // building PVXS value
  builder.ArrayProlog(&any_value);
  builder.ScalarProlog(&any_value[0]);
  builder.ScalarEpilog(&any_value[0]);
  builder.ArrayElementSeparator();
  builder.ScalarProlog(&any_value[1]);
  builder.ScalarEpilog(&any_value[1]);
  builder.ArrayEpilog(&any_value);

  auto pvxs_value = builder.GetPVXSValue();

  // validating PVXS value
  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Int32A);
  auto data = pvxs_value.as<::pvxs::shared_array<const int32_t>>();
  EXPECT_EQ(data.size(), 2);
  EXPECT_EQ(data[0], 42);
  EXPECT_EQ(data[1], 0);
}

//! Build PVXS value from AnyValue representing an array of integers inside the struct.

TEST_F(PvxsValueBuilderTests, ArrayInStruct)
{
  // original AnyValue
  const std::string struct_name{"struct_name"};
  const int n_elements = 2;
  sup::dto::AnyValue any_array(n_elements, sup::dto::SignedInteger32Type);
  any_array[0] = 42;
  sup::dto::AnyValue any_value = {{{"array", any_array}}, struct_name};

  // initializing builder
  auto pvxs_type =
      ::pvxs::TypeDef(::pvxs::TypeCode::Struct, struct_name, {pvxs::members::Int32A("array")});
  PvxsValueBuilder builder(pvxs_type);

  // building PVXS value
  builder.StructProlog(&any_value);
  builder.MemberProlog(&any_value["array"], "array");
  builder.ArrayProlog(&any_value["array"]);
  builder.ScalarProlog(&any_value["array"][0]);
  builder.ScalarEpilog(&any_value["array"][0]);
  builder.ArrayElementSeparator();
  builder.ScalarProlog(&any_value["array"][1]);
  builder.ScalarEpilog(&any_value["array"][1]);
  builder.ArrayEpilog(&any_value["array"]);
  builder.MemberEpilog(&any_value["array"], "array");
  builder.StructEpilog(&any_value);

  auto pvxs_value = builder.GetPVXSValue();

  // validating PVXS value
  EXPECT_EQ(pvxs_value.id(), struct_name);

  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Struct);
  EXPECT_EQ(pvxs_value["array"].type(), pvxs::TypeCode::Int32A);
  auto data = pvxs_value["array"].as<::pvxs::shared_array<const int32_t>>();
  EXPECT_EQ(data.size(), 2);
  EXPECT_EQ(data[0], 42);
  EXPECT_EQ(data[1], 0);
}

//! Build PVXS value from AnyValue representing an array of two structures.

TEST_F(PvxsValueBuilderTests, ArrayWithTwoStructureElements)
{
  const std::string expected_struct_name("struct_name");

  // original AnyValue
  const sup::dto::AnyValue struct_value1 = {{{"field_name", {sup::dto::SignedInteger32Type, 42}}},
                                            expected_struct_name};
  const sup::dto::AnyValue struct_value2 = {{{"field_name", {sup::dto::SignedInteger32Type, 43}}},
                                            expected_struct_name};
  auto anyvalue = sup::dto::ArrayValue({struct_value1, struct_value2});

  // initializing builder
  auto pvxs_type = ::pvxs::TypeDef(::pvxs::TypeCode::StructA, expected_struct_name,
                                   {pvxs::members::Int32("field_name")});
  PvxsValueBuilder builder(pvxs_type);

  // building PVXS value
  builder.ArrayProlog(&anyvalue);
  builder.StructProlog(&anyvalue[0]);
  builder.MemberProlog(&anyvalue[0]["field_name"], "field_name");
  builder.ScalarProlog(&anyvalue[0]["field_name"]);
  builder.ScalarEpilog(&anyvalue[0]["field_name"]);
  builder.MemberEpilog(&anyvalue[0]["field_name"], "field_name");
  builder.StructEpilog(&anyvalue[0]);

  builder.ArrayElementSeparator();

  builder.StructProlog(&anyvalue[1]);
  builder.MemberProlog(&anyvalue[1]["field_name"], "field_name");
  builder.ScalarProlog(&anyvalue[1]["field_name"]);
  builder.ScalarEpilog(&anyvalue[1]["field_name"]);
  builder.MemberEpilog(&anyvalue[1]["field_name"], "field_name");
  builder.StructEpilog(&anyvalue[1]);

  builder.ArrayEpilog(&anyvalue);

  auto pvxs_value = builder.GetPVXSValue();

  // validating PVXS value
  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::StructA);
  auto array_data = pvxs_value.as<pvxs::shared_array<const pvxs::Value>>();
  EXPECT_EQ(array_data.size(), 2);
  EXPECT_EQ(array_data[0]["field_name"].as<int32_t>(), 42);
  EXPECT_EQ(array_data[1]["field_name"].as<int32_t>(), 43);

  EXPECT_EQ(array_data[0].id(), expected_struct_name);
  EXPECT_EQ(array_data[1].id(), expected_struct_name);
}
