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
#include <sup/dto/anytype_helper.h>
#include <sup/dto/anyvalue.h>
#include <sup/epics/utils/dto_conversion_utils.h>
#include <sup/epics/utils/pvxs_type_builder.h>
#include <sup/epics/utils/pvxs_utils.h>

#include <iostream>

using namespace ::sup::epics;

//! Testing PvxsTypeBuilder class to build pvxs::TypeDef from AnyType's.

class PvxsTypeBuilderTests : public ::testing::Test
{
};

TEST_F(PvxsTypeBuilderTests, EmptyType)
{
  sup::dto::AnyType any_type;
  auto pvxs_type = BuildPVXSType(any_type);

  // There is no good way to check if type is empty. Where are no any getters, and it is not
  // allowed to construct pvxs::Value from it.

  const std::string empty_type_string("<Empty>\n");
  std::ostringstream ostr;
  ostr << pvxs_type;

  EXPECT_EQ(ostr.str(), empty_type_string);
}

//! Testing GetPVXSType method to build pvxs::TypeDef from scalar-like types.

TEST_F(PvxsTypeBuilderTests, ScalarType)
{
  sup::dto::AnyType any_type(sup::dto::SignedInteger32Type);

  // The only way to check if pvxs::TypeDef is correctly created is to create
  // a pvxs::Value from it, and check it's type code.
  auto pvxs_value = BuildPVXSType(any_type).create();

  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value.nmembers(), 0);

  // tests for other basic scalars are done in DtoConversionUtilsTest::GetPVXSTypeCode
}

//! Build PVXS type from AnyType representing a struct with single field.

TEST_F(PvxsTypeBuilderTests, StructWithSingleField)
{
  sup::dto::AnyType any_type = {{"signed", {sup::dto::SignedInteger32Type}}};
  auto pvxs_value = BuildPVXSType(any_type).create();

  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Struct);
  EXPECT_EQ(pvxs_value.nmembers(), 1);

  auto names = GetMemberNames(pvxs_value);
  EXPECT_EQ(names, std::vector<std::string>({"signed"}));
  EXPECT_EQ(pvxs_value["signed"].type(), ::pvxs::TypeCode::Int32);
}

//! Build PVXS value from AnyValue representing a struct with two fields.

TEST_F(PvxsTypeBuilderTests, StructWithTwoFields)
{
  sup::dto::AnyType any_type = {{"signed", {sup::dto::SignedInteger32Type}},
                                {"bool", {sup::dto::BooleanType}}};
  auto pvxs_value = BuildPVXSType(any_type).create();

  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Struct);
  EXPECT_EQ(pvxs_value.nmembers(), 2);

  auto names = GetMemberNames(pvxs_value);
  EXPECT_EQ(names, std::vector<std::string>({"signed", "bool"}));

  EXPECT_EQ(pvxs_value["signed"].type(), ::pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value["bool"].type(), ::pvxs::TypeCode::Bool);
}

//! Build PVXS type from AnyType representing a struct with two fields nested in parent struct.

TEST_F(PvxsTypeBuilderTests, NestedStruct)
{
  sup::dto::AnyType two_scalars = {{"signed", {sup::dto::SignedInteger32Type}},
                                   {"bool", {sup::dto::BooleanType}}};
  sup::dto::AnyType any_type = {{"scalars", two_scalars}};

  auto pvxs_value = BuildPVXSType(any_type).create();

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
}

//! Build PVXS type from AnyType representing a struct with two nested structs.

TEST_F(PvxsTypeBuilderTests, TwoNestedStruct)
{
  const std::string struct_name = "struct_name";
  sup::dto::AnyType two_scalars = {{"signed", {sup::dto::SignedInteger32Type}},
                                   {"bool", {sup::dto::BooleanType}}};

  sup::dto::AnyType any_type{
      {{"struct1", two_scalars},
       {"struct2",
        {{"first", {sup::dto::SignedInteger8Type}}, {"second", {sup::dto::SignedInteger8Type}}}}},
      struct_name};

  auto pvxs_value = BuildPVXSType(any_type).create();

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

  // second branch
  auto struct2_value = pvxs_value["struct2"];
  EXPECT_EQ(struct2_value.type(), ::pvxs::TypeCode::Struct);
  EXPECT_EQ(struct2_value.nmembers(), 2);

  auto struct2_fields = GetMemberNames(struct2_value);
  EXPECT_EQ(struct2_fields, std::vector<std::string>({"first", "second"}));
  EXPECT_EQ(pvxs_value["struct2.first"].type(), ::pvxs::TypeCode::Int8);
  EXPECT_EQ(pvxs_value["struct2.second"].type(), ::pvxs::TypeCode::Int8);
}

//! Building array of PVXS values (exercise to understand PVXS better).

TEST_F(PvxsTypeBuilderTests, PVXSTypeBasicsArrayOfIntegers)
{
  {  // studying impliced sharing
    auto value = pvxs::TypeDef(pvxs::TypeCode::Int32A).create();
    EXPECT_EQ(value.type(), pvxs::TypeCode::Int32A);

    ::pvxs::shared_array<int32_t> array({42, 43});

    EXPECT_EQ(array.size(), 2);
    value = array.freeze();

    EXPECT_EQ(array.size(), 0);  // array dissapears after the assignment

    EXPECT_EQ(value.type(), pvxs::TypeCode::Int32A);
    // I don't know how to access values directly
    auto data = value.as<::pvxs::shared_array<const int32_t>>();
    EXPECT_EQ(data.size(), 2);
    EXPECT_EQ(data[0], 42);
    EXPECT_EQ(data[1], 43);

    ::pvxs::shared_array<int32_t> array2({42, 43, 45});
    value = array2.freeze();
    auto data2 = value.as<::pvxs::shared_array<const int32_t>>();
    EXPECT_EQ(data2.size(), 3);
  }
}

//! Build PVXS type from AnyType representing an array of integers.

TEST_F(PvxsTypeBuilderTests, ArrayOfIntegers)
{
  const int n_elements = 42;
  sup::dto::AnyType any_type(n_elements, sup::dto::SignedInteger32Type);

  auto pvxs_value = BuildPVXSType(any_type).create();

  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Int32A);
  auto data = pvxs_value.as<::pvxs::shared_array<const int32_t>>();
  EXPECT_EQ(data.size(), 0);

  // we can't check much here, since n_elements is not a part of pvxs::TypeDef
}

//! Building array of PVXS values  (exercise to understand PVXS better).

TEST_F(PvxsTypeBuilderTests, PVXSTypeArrayInStruct)
{
  pvxs::TypeDef type_def(pvxs::TypeCode::Struct, "simple_t",
                         {pvxs::Member(pvxs::TypeCode::Int32A, "field")});

  auto value = type_def.create();
  EXPECT_EQ(value.type(), pvxs::TypeCode::Struct);
  EXPECT_EQ(value["field"].type(), pvxs::TypeCode::Int32A);

  ::pvxs::shared_array<int32_t> array({42, 43});
  value["field"] = array.freeze();
  auto data = value["field"].as<::pvxs::shared_array<const int32_t>>();
  EXPECT_EQ(data.size(), 2);
  EXPECT_EQ(data[0], 42);
  EXPECT_EQ(data[1], 43);
}

//! Build PVXS type from AnyType representing an array of integers inside the struct.

TEST_F(PvxsTypeBuilderTests, ArrayInStruct)
{
  const int n_elements = 42;
  sup::dto::AnyType any_array(n_elements, sup::dto::SignedInteger32Type);
  sup::dto::AnyType any_type = {{{"array", any_array}}, "struct_name"};

  auto pvxs_value = BuildPVXSType(any_type).create();
  EXPECT_EQ(pvxs_value.id(), std::string("struct_name"));

  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Struct);
  EXPECT_EQ(pvxs_value["array"].type(), pvxs::TypeCode::Int32A);
  auto data = pvxs_value["array"].as<::pvxs::shared_array<const int32_t>>();
  EXPECT_EQ(data.size(), 0);
}

//!  Build PVXS type from AnyType representing an array containing two structures. Each structure
//!  has a single `field_name` field with a scalar.

//! Test is failing since we don't know how build StructA types in step-wise manner.

TEST_F(PvxsTypeBuilderTests, DISABLED_ArrayWithTwoStructureElements)
{
  // building AnyType representing array of structs
  sup::dto::AnyType struct_type = {{{"field_name", sup::dto::SignedInteger32Type}}, "struct_name"};
  sup::dto::AnyType array(2, struct_type);

  auto pvxs_type_result = BuildPVXSType(array);

  // We can't check a type because of the lack of getters. We will check the value.
  auto pvxs_value = pvxs_type_result.create();

  // arrays doesn't have names in PVXS
  EXPECT_TRUE(pvxs_value.id().empty());
  EXPECT_EQ(pvxs_value.type(), pvxs::TypeCode::StructA);

  // This is the only known way to file the array
  ::pvxs::Value array_field(pvxs_value);
  ::pvxs::shared_array<::pvxs::Value> arr(2);
  arr[0] = array_field.allocMember();
  arr[0]["field_name"] = 42;
  arr[1] = array_field.allocMember();
  arr[1]["field_name"] = 43;
  array_field = arr.freeze().castTo<const void>();

  // reading the data back
  auto array_data = pvxs_value.as<pvxs::shared_array<const pvxs::Value>>();
  EXPECT_EQ(array_data.size(), 2);
  EXPECT_EQ(array_data[0]["field_name"].as<int32_t>(), 42);
  EXPECT_EQ(array_data[1]["field_name"].as<int32_t>(), 43);
}
