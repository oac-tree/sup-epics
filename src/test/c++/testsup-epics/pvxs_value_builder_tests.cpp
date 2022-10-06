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

#include <sup/epics/utils/pvxs_utils.h>

using namespace ::sup::epics;

class PvxsValueBuilderTests : public ::testing::Test
{
};

//! Investigating PVXS value itself (exercise to understand PVXS better).

TEST_F(PvxsValueBuilderTests, PVXSValueBasics)
{
  // default constructed
  pvxs::Value pvxs_default1;
  EXPECT_FALSE(pvxs_default1.valid());
  EXPECT_EQ(pvxs_default1.type(), ::pvxs::TypeCode::Null);

  pvxs::Value pvxs_default2;
  EXPECT_TRUE(pvxs_default1.equalType(pvxs_default2));
  EXPECT_TRUE(pvxs_default1.equalInst(pvxs_default2));  // Shouldn't be false?

  // from int32
  pvxs::Value pvxs_int1 = pvxs::TypeDef(pvxs::TypeCode::Int32).create();
  pvxs_int1 = 42;
  EXPECT_TRUE(pvxs_int1.valid());
  EXPECT_EQ(pvxs_int1.as<int>(), 42);

  pvxs::Value pvxs_int2 = pvxs::TypeDef(pvxs::TypeCode::Int32).create();
  EXPECT_TRUE(pvxs_int1.equalType(pvxs_int2));
  EXPECT_FALSE(pvxs_int1.equalInst(pvxs_int2));
}

//! Studying how to assign to PVXS value (exercise to understand PVXS better).

TEST_F(PvxsValueBuilderTests, PVXSValueBasicsAssignToScalar)
{
  pvxs::Value pvxs_int1 = pvxs::TypeDef(pvxs::TypeCode::Int32).create();
  pvxs_int1 = 42;
  EXPECT_TRUE(pvxs_int1.valid());
  EXPECT_EQ(pvxs_int1.as<int>(), 42);

  pvxs::Value pvxs_int2 = pvxs_int1;
  EXPECT_EQ(pvxs_int2.as<int>(), 42);

  pvxs::Value pvxs_int3;
  pvxs_int3 = pvxs_int1;
  EXPECT_EQ(pvxs_int3.as<int>(), 42);

  pvxs_int3 = 45;
  EXPECT_EQ(pvxs_int1.as<int>(), 45);
}

//! Studying how to assign to PVXS struct (exercise to understand PVXS better).

TEST_F(PvxsValueBuilderTests, PVXSValueBasicsAssignToStruct)
{
  pvxs::TypeDef type_def(pvxs::TypeCode::Struct, "simple_t",
                         {pvxs::Member(pvxs::TypeCode::Int32, "field")});

  auto value = type_def.create();
  value["field"] = 42;

  EXPECT_EQ(value["field"].as<int32_t>(), 42);

  auto field_value = value["field"];  // copy
  field_value = 43;
  EXPECT_EQ(value["field"].as<int32_t>(), 43);  // seems there is an implicit sharing inside
}

//! Studying what happen if we assign scalar to struct (exercise to understand PVXS better).

TEST_F(PvxsValueBuilderTests, PVXSValueBasicsAttemptToAssign)
{
  auto struct_value = pvxs::TypeDef(pvxs::TypeCode::Struct, "simple_t",
                         {pvxs::Member(pvxs::TypeCode::Int32, "field")}).create();

  struct_value["field"] = 42;

  auto scalar_value = pvxs::TypeDef(pvxs::TypeCode::Int32).create();
  scalar_value = 43;

  struct_value = scalar_value;

  // so, assignment simply changes the type
  EXPECT_EQ(struct_value.type(), ::pvxs::TypeCode::Int32);
  EXPECT_EQ(struct_value.as<int>(), 43);
}

//! Build PVXS value from empty AnyValue.

TEST_F(PvxsValueBuilderTests, FromEmpty)
{
  // investigating default constructed PVXS
  pvxs::Value pvxs_default;

  // constructing from empty AnyValue
  sup::dto::AnyValue any_value;
  auto pvxs_value = BuildPVXSValue(any_value);
  EXPECT_FALSE(pvxs_value.valid());
  EXPECT_TRUE(pvxs_value.equalType(pvxs_default));
}

//! Build PVXS value from scalar like AnyValue.

TEST_F(PvxsValueBuilderTests, SignedInteger32)
{
  sup::dto::AnyValue any_value{sup::dto::SignedInteger32Type};
  any_value = 42;

  auto pvxs_value = BuildPVXSValue(any_value);
  EXPECT_TRUE(pvxs_value.valid());
  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value.as<int32_t>(), 42);

  // other basic types are performed via DtoConversionUtilsTest::GetPVXSValueFromScalar testing
}

//! Build PVXS value from AnyValue representing a struct with single field.

TEST_F(PvxsValueBuilderTests, StructWithSingleField)
{
  sup::dto::AnyValue any_value = {{{"signed", {sup::dto::SignedInteger32Type, 42}}}};

  auto pvxs_value = BuildPVXSValue(any_value);
  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Struct);
  EXPECT_EQ(pvxs_value.nmembers(), 1);

  auto names = GetMemberNames(pvxs_value);
  EXPECT_EQ(names, std::vector<std::string>({"signed"}));
  EXPECT_EQ(pvxs_value["signed"].type(), ::pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value["signed"].as<int32_t>(), 42);
}

//! Build PVXS value from AnyValue representing a struct with two fields.

TEST_F(PvxsValueBuilderTests, StructWithTwoFields)
{
  sup::dto::AnyValue any_value = {{"signed", {sup::dto::SignedInteger32Type, 42}},
                                  {"bool", {sup::dto::BooleanType, true}}};

  auto pvxs_value = BuildPVXSValue(any_value);

  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Struct);
  EXPECT_EQ(pvxs_value.nmembers(), 2);

  auto names = GetMemberNames(pvxs_value);
  EXPECT_EQ(names, std::vector<std::string>({"signed", "bool"}));

  EXPECT_EQ(pvxs_value["signed"].type(), ::pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value["signed"].as<int32_t>(), 42);
  EXPECT_EQ(pvxs_value["bool"].type(), ::pvxs::TypeCode::Bool);
  EXPECT_EQ(pvxs_value["bool"].as<bool>(), true);
}

//! Build PVXS value from AnyValue representing a struct with two fields nested in parent struct.

TEST_F(PvxsValueBuilderTests, NestedStruct)
{
  sup::dto::AnyValue two_scalars = {{"signed", {sup::dto::SignedInteger32Type, 42}},
                                    {"bool", {sup::dto::BooleanType, true}}};
  sup::dto::AnyValue any_value = {{"scalars", two_scalars}};

  auto pvxs_value = BuildPVXSValue(any_value);

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

//! Build PVXS value from AnyValue representing a struct with two nested structs.

TEST_F(PvxsValueBuilderTests, TwoNestedStruct)
{
  const std::string struct_name = "struct_name";
  sup::dto::AnyValue two_scalars = {{"signed", {sup::dto::SignedInteger32Type, 42}},
                                    {"bool", {sup::dto::BooleanType, true}}};

  sup::dto::AnyValue any_value{{{"struct1", two_scalars},
                                {"struct2",
                                 {{"first", {sup::dto::SignedInteger8Type, 1}},
                                  {"second", {sup::dto::SignedInteger8Type, 2}}}}},
                               struct_name};

  auto pvxs_value = BuildPVXSValue(any_value);

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
  EXPECT_EQ(pvxs_value["struct2.second"].type(), ::pvxs::TypeCode::Int8);
  EXPECT_EQ(pvxs_value["struct2.first"].as<int32_t>(), 1);
  EXPECT_EQ(pvxs_value["struct2.second"].as<int32_t>(), 2);
}

//! Build PVXS value from AnyValue representing an array of integers.

TEST_F(PvxsValueBuilderTests, ArrayOfIntegers)
{
  const int n_elements = 2;
  sup::dto::AnyValue any_value(n_elements, sup::dto::SignedInteger32Type);
  any_value[0] = 42;

  auto pvxs_value = BuildPVXSValue(any_value);

  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Int32A);
  auto data = pvxs_value.as<::pvxs::shared_array<const int32_t>>();
  EXPECT_EQ(data.size(), 2);
  EXPECT_EQ(data[0], 42);
  EXPECT_EQ(data[1], 0);
}

//! Build PVXS value from AnyValue representing an array of integers inside the struct.

TEST_F(PvxsValueBuilderTests, ArrayInStruct)
{
  const std::string struct_name{"struct_name"};

  const int n_elements = 2;
  sup::dto::AnyValue any_array(n_elements, sup::dto::SignedInteger32Type);
  any_array[0] = 42;
  sup::dto::AnyValue any_value = {{{"array", any_array}}, struct_name};

  auto pvxs_value = BuildPVXSValue(any_value);
  EXPECT_EQ(pvxs_value.id(), struct_name);

  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Struct);
  EXPECT_EQ(pvxs_value["array"].type(), pvxs::TypeCode::Int32A);
  auto data = pvxs_value["array"].as<::pvxs::shared_array<const int32_t>>();
  EXPECT_EQ(data.size(), 2);
  EXPECT_EQ(data[0], 42);
  EXPECT_EQ(data[1], 0);
}

//! Building PVXS value from AnyValue mimicking NTEnum.

TEST_F(PvxsValueBuilderTests, NTEnum)
{
  // preparing AnyValue with all fields as in NTEnum
  sup::dto::AnyValue choices =
      sup::dto::ArrayValue({{sup::dto::StringType, std::string("abc")}, std::string("qwerty")});

  sup::dto::AnyValue enum_value = {
      {{"index", {sup::dto::SignedInteger32Type, 42}}, {"choices", choices}}, "enum_t"};

  sup::dto::AnyValue alarm_value = {{{"severity", {sup::dto::SignedInteger32Type, 1}},
                                     {"status", {sup::dto::SignedInteger32Type, 2}},
                                     {"message", {sup::dto::StringType, std::string("abc")}}},
                                    "alarm_t"};

  sup::dto::AnyValue timestamp_value = {{{"secondsPastEpoch", {sup::dto::SignedInteger64Type, -1}},
                                         {"nanoseconds", {sup::dto::SignedInteger32Type, -2}},
                                         {"userTag", {sup::dto::SignedInteger32Type, -3}}},
                                        "time_t"};

  sup::dto::AnyValue any_value{
      {{"value", enum_value}, {"alarm", alarm_value}, {"timeStamp", timestamp_value}},
      "epics:nt/NTEnum:1.0"};

  auto pvxs_value = BuildPVXSValue(any_value);

  EXPECT_EQ(pvxs_value.id(), std::string("epics:nt/NTEnum:1.0"));
  EXPECT_EQ(pvxs_value["value"].type(), pvxs::TypeCode::Struct);
  EXPECT_EQ(pvxs_value["value"].id(), std::string("enum_t"));
  EXPECT_EQ(pvxs_value["value.index"].type(), pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value["value.index"].as<int32_t>(), 42);
  EXPECT_EQ(pvxs_value["value.choices"].type(), pvxs::TypeCode::StringA);
  auto enum_choices = pvxs_value["value.choices"].as<::pvxs::shared_array<const std::string>>();
  EXPECT_EQ(enum_choices.size(), 2);
  EXPECT_EQ(enum_choices[0], std::string("abc"));
  EXPECT_EQ(enum_choices[1], std::string("qwerty"));

  EXPECT_EQ(pvxs_value["alarm"].type(), pvxs::TypeCode::Struct);
  EXPECT_EQ(pvxs_value["alarm.severity"].type(), pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value["alarm.severity"].as<int32_t>(), 1);
  EXPECT_EQ(pvxs_value["alarm.status"].type(), pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value["alarm.status"].as<int32_t>(), 2);
  EXPECT_EQ(pvxs_value["alarm.message"].type(), pvxs::TypeCode::String);
  EXPECT_EQ(pvxs_value["alarm.message"].as<std::string>(), std::string("abc"));

  EXPECT_EQ(pvxs_value["timeStamp"].type(), pvxs::TypeCode::Struct);
  EXPECT_EQ(pvxs_value["timeStamp.secondsPastEpoch"].type(), pvxs::TypeCode::Int64);
  EXPECT_EQ(pvxs_value["timeStamp.secondsPastEpoch"].as<int64_t>(), -1);
  EXPECT_EQ(pvxs_value["timeStamp.nanoseconds"].type(), pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value["timeStamp.nanoseconds"].as<int32_t>(), -2);
  EXPECT_EQ(pvxs_value["timeStamp.userTag"].type(), pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value["timeStamp.userTag"].as<int32_t>(), -3);
}

//! Build PVXS value from AnyValue representing an array of two structures.

TEST_F(PvxsValueBuilderTests, ArrayWithTwoStructureElements)
{
  // building any value
  sup::dto::AnyValue struct_value1 = {{{"field_name", {sup::dto::SignedInteger32Type, 42}}},
                                      "struct_name"};
  sup::dto::AnyValue struct_value2 = {{{"field_name", {sup::dto::SignedInteger32Type, 43}}},
                                      "struct_name"};
  auto anyvalue = sup::dto::ArrayValue({{struct_value1}, struct_value2});

//  auto pvxs_value = BuildPVXSValue(anyvalue);

}
