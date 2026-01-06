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

#include <sup/dto/anytype_helper.h>
#include <sup/dto/anyvalue.h>
#include <sup/dto/anyvalue_helper.h>
#include <sup/epics/utils/dto_conversion_utils.h>
#include <sup/epics/utils/pvxs_type_builder.h>
#include <sup/epics/utils/pvxs_utils.h>

#include <gtest/gtest.h>
#include <pvxs/data.h>
#include <pvxs/nt.h>

using namespace ::sup::epics;

/**
 * @brief Tests for PVXSTypeBuilder class and BuildPVXSType() helper method.
 */
class PvxsTypeBuilderTests : public ::testing::Test
{
public:
  /**
   * @brief Returns a string representing PVXS type, using PVXS debug string.
   */
  static std::string GetPVXSTypeString(const pvxs::TypeDef& type_def)
  {
    std::ostringstream ostr;
    ostr << type_def;
    return ostr.str();
  }
};

TEST_F(PvxsTypeBuilderTests, EmptyType)
{
  const sup::dto::AnyType any_type;
  auto pvxs_type = BuildPVXSType(any_type);

  // There is no good way to check if type is empty. Where are no any getters, and it is not
  // allowed to construct pvxs::Value from it.

  const std::string empty_type_string("<Empty>\n");
  std::ostringstream ostr;
  ostr << pvxs_type;

  EXPECT_EQ(ostr.str(), empty_type_string);
}

TEST_F(PvxsTypeBuilderTests, ScalarType)
{
  const sup::dto::AnyType any_type(sup::dto::SignedInteger32Type);

  // The only way to check if pvxs::TypeDef is correctly created is to create
  // a pvxs::Value from it, and check it's type code.
  auto pvxs_value = BuildPVXSType(any_type).create();

  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value.nmembers(), 0);

  // tests for other basic scalars are done in DtoConversionUtilsTest::GetPVXSTypeCode
}

TEST_F(PvxsTypeBuilderTests, StructWithSingleField)
{
  const sup::dto::AnyType any_type = {{"signed", {sup::dto::SignedInteger32Type}}};
  auto pvxs_value = BuildPVXSType(any_type).create();

  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Struct);
  EXPECT_EQ(pvxs_value.nmembers(), 1);

  auto names = GetMemberNames(pvxs_value);
  EXPECT_EQ(names, std::vector<std::string>({"signed"}));
  EXPECT_EQ(pvxs_value["signed"].type(), ::pvxs::TypeCode::Int32);
}

//! A struct with two scalar fields.
TEST_F(PvxsTypeBuilderTests, StructWithTwoFields)
{
  const sup::dto::AnyType any_type = {{"signed", {sup::dto::SignedInteger32Type}},
                                      {"bool", {sup::dto::BooleanType}}};
  auto pvxs_value = BuildPVXSType(any_type).create();

  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Struct);
  EXPECT_EQ(pvxs_value.nmembers(), 2);

  auto names = GetMemberNames(pvxs_value);
  EXPECT_EQ(names, std::vector<std::string>({"signed", "bool"}));

  EXPECT_EQ(pvxs_value["signed"].type(), ::pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value["bool"].type(), ::pvxs::TypeCode::Bool);
}

//! A struct with the nested struct (with two scalar fields).
TEST_F(PvxsTypeBuilderTests, NestedStruct)
{
  const sup::dto::AnyType two_scalars = {{"signed", {sup::dto::SignedInteger32Type}},
                                         {"bool", {sup::dto::BooleanType}}};
  const sup::dto::AnyType any_type = {{"scalars", two_scalars}};

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

//! A struct with two nested structs.
TEST_F(PvxsTypeBuilderTests, TwoNestedStruct)
{
  const std::string struct_name = "struct_name";
  sup::dto::AnyType two_scalars = {{"signed", {sup::dto::SignedInteger32Type}},
                                   {"bool", {sup::dto::BooleanType}}};

  const sup::dto::AnyType any_type{
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

//! Array of integers.
TEST_F(PvxsTypeBuilderTests, ArrayOfIntegers)
{
  const int n_elements = 42;
  const sup::dto::AnyType any_type(n_elements, sup::dto::SignedInteger32Type);

  auto pvxs_value = BuildPVXSType(any_type).create();

  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Int32A);
  auto data = pvxs_value.as<::pvxs::shared_array<const int32_t>>();
  EXPECT_EQ(data.size(), 0);

  // we can't check much here, since n_elements is not a part of pvxs::TypeDef
}

//! Building array of PVXS values  (exercise to understand PVXS better).

TEST_F(PvxsTypeBuilderTests, PVXSTypeArrayInStruct)
{
  const pvxs::TypeDef type_def(pvxs::TypeCode::Struct, "simple_t",
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

/*
AnyType>
struct struct_name
    array: array
        size: 2
        element: int32

PVXS Type>
struct "struct_name" {
    int32_t[] array
}
*/
//! Structs contains an array of integers.
TEST_F(PvxsTypeBuilderTests, ArrayInStruct)
{
  const int n_elements = 2;
  const sup::dto::AnyType any_array(n_elements, sup::dto::SignedInteger32Type);
  const sup::dto::AnyType any_type = {{{"array", any_array}}, "struct_name"};

  auto pvxs_type_result = BuildPVXSType(any_type);
  auto pvxs_value = pvxs_type_result.create();
  EXPECT_EQ(pvxs_value.id(), std::string("struct_name"));

  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Struct);
  EXPECT_EQ(pvxs_value["array"].type(), pvxs::TypeCode::Int32A);
  auto data = pvxs_value["array"].as<::pvxs::shared_array<const int32_t>>();
  EXPECT_EQ(data.size(), 0);

  // Ugly way to check internal structure of PVXS Type.
  const std::string expected_pvxs_type_string = R"RAW(struct "struct_name" {
    int32_t[] array
}
)RAW";
  EXPECT_EQ(GetPVXSTypeString(pvxs_type_result), expected_pvxs_type_string);
}

/*
AnyType>
array array_name
    size: 2
    element: struct struct_name
        field_name: int32

PVXS Type>
struct[] "struct_name" {
    int32_t field_name
}
*/
//! Array contains a structure, where structure has a single field.
TEST_F(PvxsTypeBuilderTests, ArrayWithTwoStructureElements)
{
  const std::string expected_struct_name("struct_name");
  const std::string expected_array_name("array_name");

  // building AnyType representing array of structs
  const sup::dto::AnyType struct_type = {{{"field_name", sup::dto::SignedInteger32Type}},
                                         expected_struct_name};
  const sup::dto::AnyType array(2, struct_type, expected_array_name);

  auto pvxs_type_result = BuildPVXSType(array);

  // We can't check a type because of the lack of getters. We will check the value.
  auto pvxs_value = pvxs_type_result.create();

  // arrays doesn't have names in PVXS
  EXPECT_TRUE(pvxs_value.id().empty());
  EXPECT_EQ(pvxs_value.type(), pvxs::TypeCode::StructA);

  // This is the only known way to fill the array
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

  EXPECT_EQ(array_data[0].id(), expected_struct_name);
  EXPECT_EQ(array_data[1].id(), expected_struct_name);

  EXPECT_EQ(array_data[0]["field_name"].as<int32_t>(), 42);
  EXPECT_EQ(array_data[1]["field_name"].as<int32_t>(), 43);

  // Ugly way to check internal structure of PVXS Type.
  const std::string expected_pvxs_type_string = R"RAW(struct[] "struct_name" {
    int32_t field_name
}
)RAW";
  EXPECT_EQ(GetPVXSTypeString(pvxs_type_result), expected_pvxs_type_string);
}

/*
AnyType>
struct outside_struct_name
 ArrayField: array array_of_structs_name
     size: 2
     element: struct internal_struct_name
         int_field: int32

PVXS Type>
struct "outside_struct_name" {
 struct[] "internal_struct_name" {
     int32_t int_field
 } ArrayField
}
 */
//! Struct contains an array-of-structs, where a struct has single scalar field
TEST_F(PvxsTypeBuilderTests, StructWithArrayOfStructsWithSingleField)
{
  const std::string internal_struct_name("internal_struct_name");
  const sup::dto::AnyType internal_struct_type = {{{"int_field", sup::dto::SignedInteger32Type}},
                                                  internal_struct_name};

  const std::string array_of_structs_name("array_of_structs_name");
  const sup::dto::AnyType array_type(2, internal_struct_type, array_of_structs_name);

  const std::string outside_struct_name("outside_struct_name");
  const sup::dto::AnyType outside_struct_type = {{{"ArrayField", array_type}}, outside_struct_name};

  auto pvxs_type_result = BuildPVXSType(outside_struct_type);
  // We can't check a type because of the lack of getters. We will check the value.
  auto pvxs_value = pvxs_type_result.create();

  EXPECT_EQ(pvxs_value.id(), outside_struct_name);
  EXPECT_EQ(pvxs_value.type(), pvxs::TypeCode::Struct);

  EXPECT_EQ(pvxs_value["ArrayField"].type(), pvxs::TypeCode::StructA);

  // Ugly way to check internal structure of PVXS Type.
  const std::string expected_pvxs_type_string = R"RAW(struct "outside_struct_name" {
    struct[] "internal_struct_name" {
        int32_t int_field
    } ArrayField
}
)RAW";
  EXPECT_EQ(GetPVXSTypeString(pvxs_type_result), expected_pvxs_type_string);
}

/*
AnyType>
struct outside_struct_name
    ArrayField: array array_of_structs_name
        size: 2
        element: struct struct_with_scalar_array_name
            ScalarArrayField: array
                size: 2
                element: int32

PVXS Type>
struct "outside_struct_name" {
    struct[] "struct_with_scalar_array_name" {
        int32_t[] ScalarArrayField
    } ArrayField
}
*/
//! Struct contains an array-of-structs, where a struct has a field with array of scalars.
TEST_F(PvxsTypeBuilderTests, StructWithArrayOfStructsWithArrayOfScalar)
{
  const int n_elements = 2;
  const sup::dto::AnyType internal_scalar_array(n_elements, sup::dto::SignedInteger32Type);

  const std::string struct_with_scalar_array_name("struct_with_scalar_array_name");
  const sup::dto::AnyType struct_with_scalar_array = {{{"ScalarArrayField", internal_scalar_array}},
                                                      struct_with_scalar_array_name};

  const std::string array_of_structs_name("array_of_structs_name");
  const sup::dto::AnyType array_type(2, struct_with_scalar_array, array_of_structs_name);

  const std::string outside_struct_name("outside_struct_name");
  const sup::dto::AnyType outside_struct_type = {{{"ArrayField", array_type}}, outside_struct_name};

  auto pvxs_type_result = BuildPVXSType(outside_struct_type);
  // We can't check a type because of the lack of getters. We will check the value.
  auto pvxs_value = pvxs_type_result.create();

  EXPECT_EQ(pvxs_value.id(), outside_struct_name);
  EXPECT_EQ(pvxs_value.type(), pvxs::TypeCode::Struct);

  EXPECT_EQ(pvxs_value["ArrayField"].type(), pvxs::TypeCode::StructA);

  // Ugly way to check internal structure of PVXS Type.
  const std::string expected_pvxs_type_string = R"RAW(struct "outside_struct_name" {
    struct[] "struct_with_scalar_array_name" {
        int32_t[] ScalarArrayField
    } ArrayField
}
)RAW";
  EXPECT_EQ(GetPVXSTypeString(pvxs_type_result), expected_pvxs_type_string);
}

/*
struct outside_struct_name
    ArrayOfStructField: array array_of_structs_name
        size: 2
        element: struct envelop_struct_name
            StructField: struct struct_with_scalar_array_name
                ScalarArrayField: array
                    size: 2
                    element: int32

PVXS Type>
struct "outside_struct_name" {
    struct[] "envelop_struct_name" {
        struct "struct_with_scalar_array_name" {
            int32_t[] ScalarArrayField
        } StructField
    } ArrayOfStructField
}
*/
//! Struct contains an array-of-structs, where struct element has the following composition: it
//! contains another struct with scalar array field.
TEST_F(PvxsTypeBuilderTests, StructWithArrayOfStructsOfStructWithArrayOfScalar)
{
  const int n_elements = 2;
  const sup::dto::AnyType internal_scalar_array(n_elements, sup::dto::SignedInteger32Type);

  const std::string struct_with_scalar_array_name("struct_with_scalar_array_name");
  const sup::dto::AnyType struct_with_scalar_array = {{{"ScalarArrayField", internal_scalar_array}},
                                                      struct_with_scalar_array_name};

  const std::string envelop_struct_name("envelop_struct_name");
  const sup::dto::AnyType envelop_struct = {{{"StructField", struct_with_scalar_array}},
                                            envelop_struct_name};

  const std::string array_of_structs_name("array_of_structs_name");
  const sup::dto::AnyType array_type(2, envelop_struct, array_of_structs_name);

  const std::string outside_struct_name("outside_struct_name");
  const sup::dto::AnyType outside_struct_type = {{{"ArrayOfStructField", array_type}},
                                                 outside_struct_name};

  auto pvxs_type_result = BuildPVXSType(outside_struct_type);
  // We can't check a type because of the lack of getters. We will check the value.
  auto pvxs_value = pvxs_type_result.create();

  // Ugly way to check internal structure of PVXS Type.
  const std::string expected_pvxs_type_string = R"RAW(struct "outside_struct_name" {
    struct[] "envelop_struct_name" {
        struct "struct_with_scalar_array_name" {
            int32_t[] ScalarArrayField
        } StructField
    } ArrayOfStructField
}
)RAW";
  EXPECT_EQ(GetPVXSTypeString(pvxs_type_result), expected_pvxs_type_string);
}
