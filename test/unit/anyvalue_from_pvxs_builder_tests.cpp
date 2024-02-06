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

#include <sup/dto/anytype_helper.h>
#include <sup/dto/anyvalue.h>
#include <sup/epics/utils/dto_conversion_utils.h>

#include <gtest/gtest.h>
#include <pvxs/data.h>
#include <pvxs/nt.h>
#include <pvxs/sharedArray.h>

#include <iostream>

using namespace ::sup::epics;

class AnyValueFromPVXSBuilderTests : public ::testing::Test
{
};

//! Construct AnyValue default-constructed PVXS value.

TEST_F(AnyValueFromPVXSBuilderTests, EmptyValue)
{
  pvxs::Value pvxs_value;

  auto anyvalue = BuildAnyValue(pvxs_value);
  EXPECT_TRUE(sup::dto::IsEmptyValue(anyvalue));
}

//! Construct AnyValue from scalar-like PVXS value.

TEST_F(AnyValueFromPVXSBuilderTests, ScalarTypes)
{
  auto pvxs_value = ::pvxs::TypeDef(::pvxs::TypeCode::Int32).create();
  pvxs_value = 42;

  auto anyvalue = BuildAnyValue(pvxs_value);
  EXPECT_EQ(anyvalue.GetType(), ::sup::dto::SignedInteger32Type);
  EXPECT_EQ(anyvalue.As<::sup::dto::int32>(), 42);
}

//! Construct AnyValue from PVXS containing a struct with single fieldd.

TEST_F(AnyValueFromPVXSBuilderTests, StructWithSingleField)
{
  auto pvxs_value =
      ::pvxs::TypeDef(::pvxs::TypeCode::Struct, {pvxs::members::Int32("signed")}).create();
  pvxs_value["signed"] = 42;

  auto anyvalue = BuildAnyValue(pvxs_value);

  sup::dto::AnyType expected_anytype = {{"signed", {sup::dto::SignedInteger32Type}}};

  EXPECT_EQ(anyvalue.GetType(), expected_anytype);
  EXPECT_TRUE(::sup::dto::IsStructValue(anyvalue));
  EXPECT_EQ(anyvalue["signed"].As<sup::dto::int32>(), 42);
}

//! Construct AnyValue from PVXS containing a struct with two fields.

TEST_F(AnyValueFromPVXSBuilderTests, StructWithTwoFields)
{
  auto pvxs_value = ::pvxs::TypeDef(::pvxs::TypeCode::Struct,
                                    {pvxs::members::Int32("signed"), pvxs::members::Bool("bool")})
                        .create();
  pvxs_value["signed"] = 42;
  pvxs_value["bool"] = true;

  auto anyvalue = BuildAnyValue(pvxs_value);

  sup::dto::AnyType expected_anytype = {{"signed", {sup::dto::SignedInteger32Type}},
                                        {"bool", {sup::dto::BooleanType}}};

  EXPECT_EQ(anyvalue.GetType(), expected_anytype);
  EXPECT_TRUE(::sup::dto::IsStructValue(anyvalue));
  EXPECT_EQ(anyvalue["signed"].As<sup::dto::int32>(), 42);
  EXPECT_EQ(anyvalue["bool"].As<sup::dto::boolean>(), true);
}

//! Construct AnyValue from PVXS containing a struct with a struct. Internal struct
//! contains two fields.

TEST_F(AnyValueFromPVXSBuilderTests, StructWithNestedStructWithField)
{
  auto pvxs_value =
      ::pvxs::TypeDef(::pvxs::TypeCode::Struct,
                      {pvxs::members::Struct("scalars", {pvxs::members::Int32("signed"),
                                                         pvxs::members::Bool("bool")})})
          .create();

  pvxs_value["scalars.signed"] = 42;
  pvxs_value["scalars.bool"] = true;

  auto anyvalue = BuildAnyValue(pvxs_value);

  sup::dto::AnyType two_scalars = {{"signed", {sup::dto::SignedInteger32Type}},
                                   {"bool", {sup::dto::BooleanType}}};
  sup::dto::AnyType expected_anytype = {{"scalars", two_scalars}};

  EXPECT_EQ(anyvalue.GetType(), expected_anytype);
  EXPECT_TRUE(::sup::dto::IsStructValue(anyvalue));
  EXPECT_TRUE(::sup::dto::IsStructValue(anyvalue["scalars"]));
  EXPECT_EQ(anyvalue["scalars.signed"].As<sup::dto::int32>(), 42);
  EXPECT_EQ(anyvalue["scalars.bool"].As<sup::dto::boolean>(), true);
}

TEST_F(AnyValueFromPVXSBuilderTests, StructWithTwoNestedStructs)
{
  const std::string struct_name = "struct_name";

  auto member1 = pvxs::members::Struct(
      "struct1", "struct1_name", {pvxs::members::Int32("signed"), pvxs::members::Bool("bool")});
  auto member2 = pvxs::members::Struct(
      "struct2", {pvxs::members::Int8("first"), pvxs::members::UInt8("second")});

  auto pvxs_value =
      ::pvxs::TypeDef(::pvxs::TypeCode::Struct, struct_name, {member1, member2}).create();
  EXPECT_EQ(pvxs_value.id(), struct_name);
  pvxs_value["struct1.signed"] = 42;
  pvxs_value["struct1.bool"] = true;
  pvxs_value["struct2.first"] = -43;
  pvxs_value["struct2.second"] = 44;

  auto anyvalue = BuildAnyValue(pvxs_value);

  sup::dto::AnyType two_scalars = {
      {{"signed", {sup::dto::SignedInteger32Type}}, {"bool", {sup::dto::BooleanType}}},
      "struct1_name"};

  sup::dto::AnyType expected_anytype{
      {{"struct1", two_scalars},
       {"struct2",
        {{"first", {sup::dto::SignedInteger8Type}}, {"second", {sup::dto::UnsignedInteger8Type}}}}},
      struct_name};

  EXPECT_EQ(anyvalue.GetType(), expected_anytype);

  EXPECT_TRUE(::sup::dto::IsStructValue(anyvalue));
  EXPECT_TRUE(::sup::dto::IsStructValue(anyvalue["struct1"]));
  EXPECT_TRUE(::sup::dto::IsStructValue(anyvalue["struct2"]));
  EXPECT_EQ(anyvalue["struct1.signed"].As<sup::dto::int32>(), 42);
  EXPECT_EQ(anyvalue["struct1.bool"].As<sup::dto::boolean>(), true);
  EXPECT_EQ(anyvalue["struct2.first"].As<sup::dto::int8>(), -43);
  EXPECT_EQ(anyvalue["struct2.second"].As<sup::dto::uint8>(), 44);
}

//! Building AnyValue from PVXS value a scalar array.

TEST_F(AnyValueFromPVXSBuilderTests, ArrayOfIntegers)
{
  auto pvxs_value = pvxs::TypeDef(pvxs::TypeCode::Int32A).create();
  ::pvxs::shared_array<int32_t> array({42, 43});
  pvxs_value = array.freeze();

  auto any_value = BuildAnyValue(pvxs_value);

  EXPECT_TRUE(sup::dto::IsArrayValue(any_value));
  EXPECT_EQ(any_value.GetTypeCode(), sup::dto::TypeCode::Array);
  EXPECT_EQ(any_value.NumberOfElements(), 2);
  EXPECT_EQ(any_value[0], 42);
  EXPECT_EQ(any_value[0].GetTypeCode(), sup::dto::TypeCode::Int32);
  EXPECT_EQ(any_value[1], 43);
  EXPECT_EQ(any_value[1].GetTypeCode(), sup::dto::TypeCode::Int32);
}

//! Building AnyValue from PVXS value containing a struct with single field containing a scalar
//! array.

TEST_F(AnyValueFromPVXSBuilderTests, ArrayInStruct)
{
  // long way to create a PVXS struct containing an array
  const std::string struct_name = "struct_name";

  auto pvxs_value =
      ::pvxs::TypeDef(::pvxs::TypeCode::Struct, struct_name, {pvxs::members::Int32A("array")})
          .create();

  ::pvxs::shared_array<int32_t> array({42, 43});
  pvxs_value["array"] = array.freeze();
  EXPECT_EQ(array.size(), 0);  // array dissapears after the assignment

  // validating struct we've got
  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Struct);
  EXPECT_EQ(pvxs_value["array"].type(), pvxs::TypeCode::Int32A);
  auto data = pvxs_value["array"].as<::pvxs::shared_array<const int32_t>>();
  EXPECT_EQ(data.size(), 2);
  EXPECT_EQ(data[0], 42);
  EXPECT_EQ(data[1], 43);

  // building AnyValue from it
  auto any_value = BuildAnyValue(pvxs_value);

  // validating result
  EXPECT_EQ(any_value.GetTypeName(), struct_name);

  sup::dto::AnyType expected_anytype{
      {{"array", ::sup::dto::AnyType(2, ::sup::dto::SignedInteger32Type)}}, struct_name};

  EXPECT_EQ(any_value.GetType(), expected_anytype);
  EXPECT_EQ(any_value["array"][0], 42);
  EXPECT_EQ(any_value["array"][1], 43);
}

//! Building AnyValue from plain default-constructed NTScalar.
//! Plan scalar means that it doesn't contain display, control and alarm meta-data.
//!
//! struct "epics:nt/NTScalar:1.0" {
//!   int32_t value = 0
//!   struct "alarm_t" {
//!     int32_t severity = 0
//!     int32_t status = 0
//!     string message = ""
//!   } alarm
//!   struct "time_t" {
//!     int64_t secondsPastEpoch = 0
//!     int32_t nanoseconds = 0
//!     int32_t userTag = 0
//!   } timeStamp
//! }

TEST_F(AnyValueFromPVXSBuilderTests, PlainDefaultConstructedNTScalar)
{
  // plain NTSCalar without display meta
  auto pvxs_value = pvxs::nt::NTScalar{pvxs::TypeCode::Int32}.create();

  // checking pvxs_value itself
  EXPECT_EQ(pvxs_value.id(), std::string("epics:nt/NTScalar:1.0"));
  EXPECT_EQ(pvxs_value["value"].type(), pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value["value"].as<int32_t>(), 0);
  EXPECT_EQ(pvxs_value["display.limitLow"].type(), pvxs::TypeCode::Null);
  EXPECT_EQ(pvxs_value["display.description"].type(), pvxs::TypeCode::Null);
  EXPECT_EQ(pvxs_value["control.limitLow"].type(), pvxs::TypeCode::Null);

  EXPECT_EQ(pvxs_value["alarm"].type(), pvxs::TypeCode::Struct);
  EXPECT_EQ(pvxs_value["alarm.severity"].type(), pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value["alarm.severity"].as<int32_t>(), 0);
  EXPECT_EQ(pvxs_value["alarm.status"].type(), pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value["alarm.status"].as<int32_t>(), 0);
  EXPECT_EQ(pvxs_value["alarm.message"].type(), pvxs::TypeCode::String);
  EXPECT_EQ(pvxs_value["alarm.message"].as<std::string>(), std::string());

  EXPECT_EQ(pvxs_value["timeStamp"].type(), pvxs::TypeCode::Struct);
  EXPECT_EQ(pvxs_value["timeStamp.secondsPastEpoch"].type(), pvxs::TypeCode::Int64);
  EXPECT_EQ(pvxs_value["timeStamp.secondsPastEpoch"].as<int64_t>(), 0);
  EXPECT_EQ(pvxs_value["timeStamp.nanoseconds"].type(), pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value["timeStamp.nanoseconds"].as<int32_t>(), 0);
  EXPECT_EQ(pvxs_value["timeStamp.userTag"].type(), pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value["timeStamp.userTag"].as<int32_t>(), 0);

  // building any_value
  auto any_value = BuildAnyValue(pvxs_value);

  // constructing expected AnyType
  sup::dto::AnyType alarm_struct = {{{"severity", {sup::dto::SignedInteger32Type}},
                                     {"status", {sup::dto::SignedInteger32Type}},
                                     {"message", {sup::dto::StringType}}},
                                    "alarm_t"};

  sup::dto::AnyType timestamp_struct = {{{"secondsPastEpoch", {sup::dto::SignedInteger64Type}},
                                         {"nanoseconds", {sup::dto::SignedInteger32Type}},
                                         {"userTag", {sup::dto::SignedInteger32Type}}},
                                        "time_t"};

  sup::dto::AnyType expected_anytype{{{"value", {sup::dto::SignedInteger32Type}},
                                      {"alarm", alarm_struct},
                                      {"timeStamp", timestamp_struct}},
                                     "epics:nt/NTScalar:1.0"};

  // validating AnyValue
  EXPECT_EQ(any_value.GetType(), expected_anytype);
  EXPECT_EQ(any_value["value"], 0);
  EXPECT_EQ(any_value["alarm.severity"].GetTypeCode(), sup::dto::TypeCode::Int32);
  EXPECT_EQ(any_value["alarm.severity"], 0);
  EXPECT_EQ(any_value["alarm.status"].GetTypeCode(), sup::dto::TypeCode::Int32);
  EXPECT_EQ(any_value["alarm.status"], 0);
  EXPECT_EQ(any_value["alarm.message"].GetTypeCode(), sup::dto::TypeCode::String);
  EXPECT_EQ(any_value["alarm.message"], std::string());
  EXPECT_EQ(any_value["timeStamp.secondsPastEpoch"].GetTypeCode(), sup::dto::TypeCode::Int64);
  EXPECT_EQ(any_value["timeStamp.secondsPastEpoch"], 0);
  EXPECT_EQ(any_value["timeStamp.nanoseconds"].GetTypeCode(), sup::dto::TypeCode::Int32);
  EXPECT_EQ(any_value["timeStamp.nanoseconds"], 0);
  EXPECT_EQ(any_value["timeStamp.userTag"].GetTypeCode(), sup::dto::TypeCode::Int32);
  EXPECT_EQ(any_value["timeStamp.userTag"], 0);
}

//! Building AnyValue from plain NTScalar.
//! Same as test above, only some values are assigned.

TEST_F(AnyValueFromPVXSBuilderTests, PlainNTScalar)
{
  // plain NTSCalar without display meta
  auto pvxs_value = pvxs::nt::NTScalar{pvxs::TypeCode::Int32}.create();

  // assigning values
  pvxs_value["value"] = 42;
  pvxs_value["alarm.severity"] = 1;
  pvxs_value["alarm.status"] = 2;
  pvxs_value["alarm.message"] = std::string("abc");
  pvxs_value["timeStamp.secondsPastEpoch"] = -1;
  pvxs_value["timeStamp.nanoseconds"] = -2;
  pvxs_value["timeStamp.userTag"] = -3;

  // checking pvxs_value itself
  EXPECT_EQ(pvxs_value["value"].as<int32_t>(), 42);
  EXPECT_EQ(pvxs_value["alarm.severity"].as<int32_t>(), 1);
  EXPECT_EQ(pvxs_value["alarm.status"].as<int32_t>(), 2);
  EXPECT_EQ(pvxs_value["alarm.message"].as<std::string>(), std::string("abc"));

  EXPECT_EQ(pvxs_value["timeStamp.secondsPastEpoch"].as<int64_t>(), -1);
  EXPECT_EQ(pvxs_value["timeStamp.nanoseconds"].as<int32_t>(), -2);
  EXPECT_EQ(pvxs_value["timeStamp.userTag"].as<int32_t>(), -3);

  // building any_value
  auto any_value = BuildAnyValue(pvxs_value);

  // constructing expected AnyType
  sup::dto::AnyType alarm_struct = {{{"severity", {sup::dto::SignedInteger32Type}},
                                     {"status", {sup::dto::SignedInteger32Type}},
                                     {"message", {sup::dto::StringType}}},
                                    "alarm_t"};

  sup::dto::AnyType timestamp_struct = {{{"secondsPastEpoch", {sup::dto::SignedInteger64Type}},
                                         {"nanoseconds", {sup::dto::SignedInteger32Type}},
                                         {"userTag", {sup::dto::SignedInteger32Type}}},
                                        "time_t"};

  sup::dto::AnyType expected_anytype{{{"value", {sup::dto::SignedInteger32Type}},
                                      {"alarm", alarm_struct},
                                      {"timeStamp", timestamp_struct}},
                                     "epics:nt/NTScalar:1.0"};

  // validating AnyValue
  EXPECT_EQ(any_value.GetType(), expected_anytype);
  EXPECT_EQ(any_value["value"], 42);
  EXPECT_EQ(any_value["alarm.severity"].GetTypeCode(), sup::dto::TypeCode::Int32);
  EXPECT_EQ(any_value["alarm.severity"], 1);
  EXPECT_EQ(any_value["alarm.status"].GetTypeCode(), sup::dto::TypeCode::Int32);
  EXPECT_EQ(any_value["alarm.status"], 2);
  EXPECT_EQ(any_value["alarm.message"].GetTypeCode(), sup::dto::TypeCode::String);
  EXPECT_EQ(any_value["alarm.message"], std::string("abc"));
  EXPECT_EQ(any_value["timeStamp.secondsPastEpoch"].GetTypeCode(), sup::dto::TypeCode::Int64);
  EXPECT_EQ(any_value["timeStamp.secondsPastEpoch"], -1);
  EXPECT_EQ(any_value["timeStamp.nanoseconds"].GetTypeCode(), sup::dto::TypeCode::Int32);
  EXPECT_EQ(any_value["timeStamp.nanoseconds"], -2);
  EXPECT_EQ(any_value["timeStamp.userTag"].GetTypeCode(), sup::dto::TypeCode::Int32);
  EXPECT_EQ(any_value["timeStamp.userTag"], -3);
}

//! Build AnyValue from empty NTEnum
//!
//! struct "epics:nt/NTEnum:1.0" {
//!   struct "enum_t" {
//!     int32_t index = 0
//!     string[] choices = {?}[]
//!   } value
//!   struct "alarm_t" {
//!     int32_t severity = 0
//!     int32_t status = 0
//!     string message = ""
//!   } alarm
//!   struct "time_t" {
//!     int64_t secondsPastEpoch = 0
//!     int32_t nanoseconds = 0
//!     int32_t userTag = 0
//!   } timeStamp
//! }

TEST_F(AnyValueFromPVXSBuilderTests, EmptyNTEnum)
{
  auto pvxs_value = pvxs::nt::NTEnum{}.create();

  // checking pvxs_value itself
  EXPECT_EQ(pvxs_value.id(), std::string("epics:nt/NTEnum:1.0"));
  EXPECT_EQ(pvxs_value["value"].type(), pvxs::TypeCode::Struct);
  EXPECT_EQ(pvxs_value["value"].id(), std::string("enum_t"));
  EXPECT_EQ(pvxs_value["value.index"].type(), pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value["value.index"].as<int32_t>(), 0);
  EXPECT_EQ(pvxs_value["value.choices"].type(), pvxs::TypeCode::StringA);
  auto enum_choices = pvxs_value["value.choices"].as<::pvxs::shared_array<const std::string>>();
  EXPECT_EQ(enum_choices.size(), 0);

  EXPECT_EQ(pvxs_value["alarm"].type(), pvxs::TypeCode::Struct);
  EXPECT_EQ(pvxs_value["alarm.severity"].type(), pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value["alarm.severity"].as<int32_t>(), 0);
  EXPECT_EQ(pvxs_value["alarm.status"].type(), pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value["alarm.status"].as<int32_t>(), 0);
  EXPECT_EQ(pvxs_value["alarm.message"].type(), pvxs::TypeCode::String);
  EXPECT_EQ(pvxs_value["alarm.message"].as<std::string>(), std::string());

  EXPECT_EQ(pvxs_value["timeStamp"].type(), pvxs::TypeCode::Struct);
  EXPECT_EQ(pvxs_value["timeStamp.secondsPastEpoch"].type(), pvxs::TypeCode::Int64);
  EXPECT_EQ(pvxs_value["timeStamp.secondsPastEpoch"].as<int64_t>(), 0);
  EXPECT_EQ(pvxs_value["timeStamp.nanoseconds"].type(), pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value["timeStamp.nanoseconds"].as<int32_t>(), 0);
  EXPECT_EQ(pvxs_value["timeStamp.userTag"].type(), pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value["timeStamp.userTag"].as<int32_t>(), 0);

  // building any_value
  auto any_value = BuildAnyValue(pvxs_value);

  // validating AnyValue
  EXPECT_EQ(any_value["value.index"].GetTypeCode(), sup::dto::TypeCode::Int32);
  EXPECT_EQ(any_value["value.index"], 0);
  EXPECT_EQ(any_value["value.choices"].GetTypeCode(), sup::dto::TypeCode::Array);
  EXPECT_EQ(any_value["value.choices"].NumberOfElements(), 0);

  EXPECT_EQ(any_value["alarm.severity"].GetTypeCode(), sup::dto::TypeCode::Int32);
  EXPECT_EQ(any_value["alarm.severity"], 0);
  EXPECT_EQ(any_value["alarm.status"].GetTypeCode(), sup::dto::TypeCode::Int32);
  EXPECT_EQ(any_value["alarm.status"], 0);
  EXPECT_EQ(any_value["alarm.message"].GetTypeCode(), sup::dto::TypeCode::String);
  EXPECT_EQ(any_value["alarm.message"], std::string());
  EXPECT_EQ(any_value["timeStamp.secondsPastEpoch"].GetTypeCode(), sup::dto::TypeCode::Int64);
  EXPECT_EQ(any_value["timeStamp.secondsPastEpoch"], 0);
  EXPECT_EQ(any_value["timeStamp.nanoseconds"].GetTypeCode(), sup::dto::TypeCode::Int32);
  EXPECT_EQ(any_value["timeStamp.nanoseconds"], 0);
  EXPECT_EQ(any_value["timeStamp.userTag"].GetTypeCode(), sup::dto::TypeCode::Int32);
  EXPECT_EQ(any_value["timeStamp.userTag"], 0);
}

//! Build AnyValue from NTEnum with data.
//! Same test as above, only enum is initialised with data.

TEST_F(AnyValueFromPVXSBuilderTests, InitialisedNTEnum)
{
  auto pvxs_value = pvxs::nt::NTEnum{}.create();

  // assigning values
  pvxs_value["value.index"] = 42;
  ::pvxs::shared_array<std::string> array({"abc", "qwerty"});
  pvxs_value["value.choices"] = array.freeze();
  pvxs_value["alarm.severity"] = 1;
  pvxs_value["alarm.status"] = 2;
  pvxs_value["alarm.message"] = std::string("abc");
  pvxs_value["timeStamp.secondsPastEpoch"] = -1;
  pvxs_value["timeStamp.nanoseconds"] = -2;
  pvxs_value["timeStamp.userTag"] = -3;

  // building any_value
  auto any_value = BuildAnyValue(pvxs_value);

  // validating AnyValue
  EXPECT_EQ(any_value["value.index"], 42);
  EXPECT_EQ(any_value["value.choices"][0], std::string("abc"));
  EXPECT_EQ(any_value["value.choices"][1], std::string("qwerty"));
  EXPECT_EQ(any_value["alarm.severity"].GetTypeCode(), sup::dto::TypeCode::Int32);
  EXPECT_EQ(any_value["alarm.severity"], 1);
  EXPECT_EQ(any_value["alarm.status"].GetTypeCode(), sup::dto::TypeCode::Int32);
  EXPECT_EQ(any_value["alarm.status"], 2);
  EXPECT_EQ(any_value["alarm.message"].GetTypeCode(), sup::dto::TypeCode::String);
  EXPECT_EQ(any_value["alarm.message"], std::string("abc"));
  EXPECT_EQ(any_value["timeStamp.secondsPastEpoch"].GetTypeCode(), sup::dto::TypeCode::Int64);
  EXPECT_EQ(any_value["timeStamp.secondsPastEpoch"], -1);
  EXPECT_EQ(any_value["timeStamp.nanoseconds"].GetTypeCode(), sup::dto::TypeCode::Int32);
  EXPECT_EQ(any_value["timeStamp.nanoseconds"], -2);
  EXPECT_EQ(any_value["timeStamp.userTag"].GetTypeCode(), sup::dto::TypeCode::Int32);
  EXPECT_EQ(any_value["timeStamp.userTag"], -3);
}

//! Build AnyValue from the array containing two structures. Each structure has a single
//! `field_name` field with a scalar.

TEST_F(AnyValueFromPVXSBuilderTests, ArrayWithTwoStructureElements)
{
  // building PVXS value representing an array of structs with two elements
  auto pvxs_value = ::pvxs::TypeDef(::pvxs::TypeCode::StructA, "struct_name",
                                    {pvxs::members::Int32("field_name")})
                        .create();

  ::pvxs::Value array_field(pvxs_value);
  ::pvxs::shared_array<::pvxs::Value> arr(2);

  arr[0] = array_field.allocMember();
  arr[0]["field_name"] = 42;
  arr[1] = array_field.allocMember();
  arr[1]["field_name"] = 43;
  array_field = arr.freeze().castTo<const void>();

  // building any_value
  auto any_value = BuildAnyValue(pvxs_value);

  // expected any value
  sup::dto::AnyValue struct_value1 = {{{"field_name", {sup::dto::SignedInteger32Type, 42}}},
                                      "struct_name"};
  sup::dto::AnyValue struct_value2 = {{{"field_name", {sup::dto::SignedInteger32Type, 43}}},
                                      "struct_name"};
  auto expected_array_value = sup::dto::ArrayValue({{struct_value1}, struct_value2});

  EXPECT_EQ(any_value, expected_array_value);
}

//! Build AnyValue from the structure with array with two structures.

TEST_F(AnyValueFromPVXSBuilderTests, StructureWithArrayWithStructure)
{
  // building PVXS struct with array with structs
  // it is not clear how to assign struct_name to external struct and internal structs
  auto pvxs_value =
      ::pvxs::TypeDef(::pvxs::TypeCode::Struct,
                      {pvxs::members::StructA("array_field", {pvxs::members::Int8("first"),
                                                              pvxs::members::UInt8("second")})})
          .create();

  ::pvxs::Value array_field(pvxs_value["array_field"]);
  ::pvxs::shared_array<::pvxs::Value> arr(2);

  arr[0] = array_field.allocMember();
  arr[0]["first"] = -43;
  arr[0]["second"] = 44;
  arr[1] = array_field.allocMember();
  arr[1]["first"] = 42;
  arr[1]["second"] = 43;
  array_field = arr.freeze().castTo<const void>();

  // constructing expected any value
  // Setting empty names for internal structs and for external struct
  sup::dto::AnyValue struct_value1 = {{{"first", {sup::dto::SignedInteger8Type, -43}},
                                       {"second", {sup::dto::UnsignedInteger8Type, 44}}},
                                      ""};
  sup::dto::AnyValue struct_value2 = {{{"first", {sup::dto::SignedInteger8Type, 42}},
                                       {"second", {sup::dto::UnsignedInteger8Type, 43}}},
                                      ""};

  auto array_value = sup::dto::ArrayValue({struct_value1, struct_value2}, "");

  sup::dto::AnyValue expected_struct_value = {{{"array_field", array_value}}, ""};

  // building any_value
  auto any_value = BuildAnyValue(pvxs_value);

  EXPECT_EQ(any_value, expected_struct_value);
}
