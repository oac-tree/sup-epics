/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : CODAC Core System
 *
 * Description   : EPICS 7 C++ wrapper classes
 *
 * Author        : Bertrand Bauvir (IO)
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

#include <gtest/gtest.h>
#include <sup/dto/anytype.h>
#include <sup/dto/anyvalue.h>
#include <sup/epics/channel_access_pv.h>

#include <cmath>
#include <thread>

#include <sup/epics-test/softioc_runner.h>
#include <sup/epics-test/softioc_utils.h>

static bool WaitForValue(const sup::epics::ChannelAccessPV& variable,
                         const sup::dto::AnyValue& expected_value, double timeout_sec);

class ChannelAccessPVTest : public ::testing::Test
{
protected:
  ChannelAccessPVTest() = default;
  ~ChannelAccessPVTest() = default;
};

inline void FillWithScalars(sup::dto::AnyValue& anyvalue, int val)
{
  for (size_t idx = 0; idx < anyvalue.NumberOfElements(); ++idx)
  {
    anyvalue[idx].ConvertFrom(val);
  }
}

TEST_F(ChannelAccessPVTest, ConnectAndRead)
{
  using namespace sup::epics;

  // preparing variables
  ChannelAccessPV ca_bool_var("CA-TESTS:BOOL", sup::dto::BooleanType);
  ChannelAccessPV ca_float_var("CA-TESTS:FLOAT", sup::dto::Float32Type);
  ChannelAccessPV ca_string_var("CA-TESTS:STRING", sup::dto::StringType);

  sup::dto::AnyType char_array_t(1024, sup::dto::Character8Type, "char8[]");
  ChannelAccessPV ca_chararray_var("CA-TESTS:CHARRAY", char_array_t);

  // waiting for variables to have valid values
  EXPECT_TRUE(ca_bool_var.WaitForValidValue(5.0));
  EXPECT_TRUE(ca_float_var.WaitForValidValue(1.0));
  EXPECT_TRUE(ca_string_var.WaitForValidValue(1.0));
  EXPECT_TRUE(ca_chararray_var.WaitForValidValue(1.0));

  // Check channel name
  EXPECT_EQ(ca_bool_var.GetChannelName(), "CA-TESTS:BOOL");
  EXPECT_EQ(ca_float_var.GetChannelName(), "CA-TESTS:FLOAT");
  EXPECT_EQ(ca_string_var.GetChannelName(), "CA-TESTS:STRING");
  EXPECT_EQ(ca_chararray_var.GetChannelName(), "CA-TESTS:CHARRAY");

  // check bool
  auto bool_val = ca_bool_var.GetValue();
  EXPECT_FALSE(sup::dto::IsEmptyValue(bool_val));
  EXPECT_EQ(bool_val.GetType(), sup::dto::BooleanType);

  // check float
  auto float_val = ca_float_var.GetValue();
  EXPECT_FALSE(sup::dto::IsEmptyValue(float_val));
  EXPECT_EQ(float_val.GetType(), sup::dto::Float32Type);

  // check string
  auto string_val = ca_string_var.GetValue();
  EXPECT_FALSE(sup::dto::IsEmptyValue(string_val));
  EXPECT_EQ(string_val.GetType(), sup::dto::StringType);

  // check array
  auto chararray_val = ca_chararray_var.GetValue();
  EXPECT_FALSE(sup::dto::IsEmptyValue(chararray_val));
  EXPECT_EQ(chararray_val.GetType(), char_array_t);
}

TEST_F(ChannelAccessPVTest, NonExistentChannel)
{
  using namespace sup::epics;

  ChannelAccessPV ca_nonexist_var("NON_EXISTING:INT", sup::dto::SignedInteger32Type);

  EXPECT_FALSE(ca_nonexist_var.IsConnected());
  EXPECT_TRUE(sup::dto::IsEmptyValue(ca_nonexist_var.GetValue()));
  EXPECT_FALSE(ca_nonexist_var.WaitForConnected(0.02));
  EXPECT_FALSE(ca_nonexist_var.WaitForValidValue(0.02));
}

// See COA-1670:
TEST_F(ChannelAccessPVTest, NonInitializedArray)
{
  using namespace sup::epics;

  sup::dto::AnyType array_type(5, sup::dto::UnsignedInteger16Type, "uint16[]");
  ChannelAccessPV non_init_array("CA-TESTS:UNINITARRAY", array_type);

  EXPECT_TRUE(non_init_array.WaitForValidValue(5.0));
  auto val = non_init_array.GetValue();
  EXPECT_EQ(val.NumberOfElements(), 5);
  for (size_t i=0; i<val.NumberOfElements(); ++i)
  {
    ASSERT_EQ(val[i].GetType(), sup::dto::UnsignedInteger16Type);
    EXPECT_EQ(val[i].As<sup::dto::uint16>(), 0);
  }
}

// See COA-1685:
TEST_F(ChannelAccessPVTest, UpdateArrayElementSizeMismatch)
{
  using namespace sup::epics;
  {
    // Write uint16 record with uint8 array
    sup::dto::AnyType array_type(4, sup::dto::UnsignedInteger8Type);
    ChannelAccessPV array_pv("CA-ARRAY-TEST:USHORT", array_type);
    EXPECT_TRUE(array_pv.WaitForConnected(5.0));
    sup::dto::AnyValue update = sup::dto::ArrayValue({
      {sup::dto::UnsignedInteger8Type, 1}, 2, 3, 4});
    EXPECT_TRUE(array_pv.SetValue(update));
    EXPECT_TRUE(WaitForValue(array_pv, update, 5.0));
  }
  {
    // Write uint16 record with uint16 array
    sup::dto::AnyType array_type(4, sup::dto::UnsignedInteger16Type);
    ChannelAccessPV array_pv("CA-ARRAY-TEST:USHORT", array_type);
    EXPECT_TRUE(array_pv.WaitForConnected(5.0));
    sup::dto::AnyValue update = sup::dto::ArrayValue({
      {sup::dto::UnsignedInteger16Type, 1}, 2, 3, 4});
    EXPECT_TRUE(array_pv.SetValue(update));
    EXPECT_TRUE(WaitForValue(array_pv, update, 5.0));
  }
  {
    // Write uint16 record with bool array
    sup::dto::AnyType array_type(4, sup::dto::BooleanType);
    ChannelAccessPV array_pv("CA-ARRAY-TEST:USHORT", array_type);
    EXPECT_TRUE(array_pv.WaitForConnected(5.0));
    sup::dto::AnyValue update = sup::dto::ArrayValue({
      {sup::dto::BooleanType, true}, false, true, false});
    EXPECT_TRUE(array_pv.SetValue(update));
    EXPECT_TRUE(WaitForValue(array_pv, update, 5.0));
  }
}

TEST_F(ChannelAccessPVTest, SingleReadWrite)
{
  using namespace sup::epics;

  // preparing variables
  ChannelAccessPV ca_bool_var("CA-TESTS:BOOL", sup::dto::BooleanType);
  ChannelAccessPV ca_float_var("CA-TESTS:FLOAT", sup::dto::Float32Type);
  ChannelAccessPV ca_string_var("CA-TESTS:STRING", sup::dto::StringType);

  sup::dto::AnyType char_array_t(1024, sup::dto::Character8Type, "char8[]");
  ChannelAccessPV ca_chararray_var("CA-TESTS:CHARRAY", char_array_t);

  // waiting for variables to connect
  EXPECT_TRUE(ca_bool_var.WaitForConnected(5.0));
  EXPECT_TRUE(ca_float_var.WaitForConnected(1.0));
  EXPECT_TRUE(ca_string_var.WaitForConnected(1.0));
  EXPECT_TRUE(ca_chararray_var.WaitForConnected(1.0));

  // set bool
  sup::dto::boolean bool_val = true;
  EXPECT_TRUE(ca_bool_var.SetValue(bool_val));

  // set float
  sup::dto::float32 float_val = 0.25F;
  EXPECT_TRUE(ca_float_var.SetValue(float_val));

  // set string
  std::string string_val = "some value";
  EXPECT_TRUE(ca_string_var.SetValue(string_val));

  // reading bool
  EXPECT_TRUE(WaitForValue(ca_bool_var, bool_val, 5.0));

  // reading float
  EXPECT_TRUE(WaitForValue(ca_float_var, float_val, 5.0));

  // reading string
  EXPECT_TRUE(WaitForValue(ca_string_var, string_val, 5.0));

  // writing long string
  sup::dto::AnyValue chararray_val(1024, sup::dto::Character8Type, "char8[]");
  sup::dto::char8 char_data[1024] =
      "Some very long string which is longer than the maximum length of EPICSv3 string and "
      "should be serialised on a waveform record";
  sup::dto::AssignFromCType(chararray_val, char_data);
  EXPECT_TRUE(ca_chararray_var.SetValue(chararray_val));

  // reading long string
  EXPECT_TRUE(WaitForValue(ca_chararray_var, chararray_val, 5.0));

  // retrieve timestamp
  auto now = std::chrono::system_clock::now();
  auto now_timestamp =
      std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
  ChannelAccessPV::ExtendedValue ext_boolean;
  EXPECT_NO_THROW(ext_boolean = ca_bool_var.GetExtendedValue());

  auto timestamp = ext_boolean.timestamp;
  EXPECT_TRUE(now_timestamp > static_cast<long>(timestamp));
  EXPECT_NE(timestamp, 0);
}

TEST_F(ChannelAccessPVTest, MultipleReadWrite)
{
  using namespace sup::epics;

  // create variables
  ChannelAccessPV ca_float_writer("CA-TESTS:FLOAT", sup::dto::Float32Type);
  ChannelAccessPV ca_float_reader("CA-TESTS:FLOAT", sup::dto::Float32Type);

  // waiting for connected clients
  EXPECT_TRUE(ca_float_writer.WaitForConnected(5.0));
  EXPECT_TRUE(ca_float_reader.WaitForConnected(1.0));

  // set first value
  sup::dto::float32 value1 = 3.5F;
  ASSERT_TRUE(ca_float_writer.SetValue(value1));

  // reading variable through first client
  EXPECT_TRUE(WaitForValue(ca_float_writer, value1, 5.0));

  // reading variable through second client
  EXPECT_TRUE(WaitForValue(ca_float_reader, value1, 5.0));

  // set second value
  sup::dto::float32 value2 = -1.5F;
  ASSERT_TRUE(ca_float_writer.SetValue(value2));

  // reading variable through first client
  EXPECT_TRUE(WaitForValue(ca_float_writer, value2, 5.0));

  // reading variable through second client
  EXPECT_TRUE(WaitForValue(ca_float_reader, value2, 5.0));
}

TEST_F(ChannelAccessPVTest, BoolFormats)
{
  using namespace sup::epics;

  // create variables
  ChannelAccessPV pv_as_bool("CA-TESTS:BOOL", sup::dto::BooleanType);
  ChannelAccessPV pv_as_uint8("CA-TESTS:BOOL", sup::dto::UnsignedInteger8Type);
  ChannelAccessPV pv_as_uint16("CA-TESTS:BOOL", sup::dto::UnsignedInteger16Type);
  ChannelAccessPV pv_as_uint32("CA-TESTS:BOOL", sup::dto::UnsignedInteger32Type);
  ChannelAccessPV pv_as_int32("CA-TESTS:BOOL", sup::dto::SignedInteger32Type);
  ChannelAccessPV pv_as_uint64("CA-TESTS:BOOL", sup::dto::UnsignedInteger64Type);
  ChannelAccessPV pv_as_int64("CA-TESTS:BOOL", sup::dto::SignedInteger64Type);
  ChannelAccessPV pv_as_string("CA-TESTS:BOOL", sup::dto::StringType);

  // waiting for connected clients
  EXPECT_TRUE(pv_as_bool.WaitForConnected(5.0));
  EXPECT_TRUE(pv_as_uint8.WaitForConnected(1.0));
  EXPECT_TRUE(pv_as_uint16.WaitForConnected(1.0));
  EXPECT_TRUE(pv_as_uint32.WaitForConnected(1.0));
  EXPECT_TRUE(pv_as_int32.WaitForConnected(1.0));
  EXPECT_TRUE(pv_as_uint64.WaitForConnected(1.0));
  EXPECT_TRUE(pv_as_string.WaitForConnected(1.0));

  // Boolean records cannot be read out by integer types that require string backends in CA:
  EXPECT_FALSE(pv_as_int64.WaitForValidValue(0.5));
  {
    // set true
    sup::dto::boolean bool_v = true;
    ASSERT_TRUE(pv_as_bool.SetValue(bool_v));

    // reading variables through different clients
    sup::dto::uint8 uint8_v = 1u;
    EXPECT_TRUE(WaitForValue(pv_as_uint8, uint8_v, 5.0));

    sup::dto::uint16 uint16_v = 1u;
    EXPECT_TRUE(WaitForValue(pv_as_uint16, uint16_v, 5.0));

    sup::dto::uint32 uint32_v = 1u;
    EXPECT_TRUE(WaitForValue(pv_as_uint32, uint32_v, 5.0));

    sup::dto::int32 int32_v = 1;
    EXPECT_TRUE(WaitForValue(pv_as_int32, int32_v, 5.0));

    sup::dto::uint64 uint64_v = 1u;
    EXPECT_TRUE(WaitForValue(pv_as_uint64, uint64_v, 5.0));

    std::string string_v = "TRUE";
    EXPECT_TRUE(WaitForValue(pv_as_string, string_v, 5.0));
    EXPECT_EQ(pv_as_string.GetValue().As<std::string>(), string_v);
  }
  {
    // set false through integer using bool channel
    sup::dto::uint16 uint16_v = 0u;
    ASSERT_TRUE(pv_as_bool.SetValue(uint16_v));
    sup::dto::boolean bool_v = false;
    EXPECT_TRUE(WaitForValue(pv_as_bool, bool_v, 5.0));

    // reading variables through different clients
    EXPECT_TRUE(WaitForValue(pv_as_uint16, uint16_v, 5.0));

    sup::dto::uint8 uint8_v = 0u;
    EXPECT_TRUE(WaitForValue(pv_as_uint8, uint8_v, 5.0));

    sup::dto::uint32 uint32_v = 0u;
    EXPECT_TRUE(WaitForValue(pv_as_uint32, uint32_v, 5.0));

    sup::dto::int32 int32_v = 0;
    EXPECT_TRUE(WaitForValue(pv_as_int32, int32_v, 5.0));

    sup::dto::uint64 uint64_v = 0u;
    EXPECT_TRUE(WaitForValue(pv_as_uint64, uint64_v, 5.0));

    std::string string_v = "FALSE";
    EXPECT_TRUE(WaitForValue(pv_as_string, string_v, 5.0));
    EXPECT_EQ(pv_as_string.GetValue().As<std::string>(), string_v);
  }
}

TEST_F(ChannelAccessPVTest, StringFormats)
{
  using namespace sup::epics;

  // create variables
  ChannelAccessPV pv_as_string("CA-TESTS:STRING", sup::dto::StringType);
  ChannelAccessPV pv_as_uint16("CA-TESTS:STRING", sup::dto::UnsignedInteger16Type);

  // waiting for connected clients
  EXPECT_TRUE(pv_as_string.WaitForConnected(5.0));
  EXPECT_TRUE(pv_as_uint16.WaitForConnected(1.0));

  // set first value
  std::string string_v = "some_string";
  ASSERT_TRUE(pv_as_string.SetValue(string_v));
  EXPECT_TRUE(WaitForValue(pv_as_string, string_v, 5.0));

  // Trying to connect a uint16 channel to a string record will never result in a valid value:
  EXPECT_FALSE(pv_as_uint16.WaitForValidValue(1.0));
}

TEST_F(ChannelAccessPVTest, IntFormats)
{
  using namespace sup::epics;

  // create variables
  ChannelAccessPV pv_as_int32("CA-TESTS:LONG", sup::dto::SignedInteger32Type);
  ChannelAccessPV pv_as_int64("CA-TESTS:LONG", sup::dto::SignedInteger64Type);
  ChannelAccessPV pv_as_string("CA-TESTS:LONG", sup::dto::StringType);
  ChannelAccessPV pv_as_bool("CA-TESTS:LONG", sup::dto::BooleanType);

  // waiting for connected clients
  EXPECT_TRUE(pv_as_int32.WaitForConnected(5.0));
  EXPECT_TRUE(pv_as_int64.WaitForConnected(1.0));
  EXPECT_TRUE(pv_as_string.WaitForConnected(1.0));
  EXPECT_TRUE(pv_as_bool.WaitForConnected(1.0));

  {
    // set first value
    sup::dto::int32 int32_v = 42;
    ASSERT_TRUE(pv_as_int32.SetValue(int32_v));
    EXPECT_TRUE(WaitForValue(pv_as_int32, int32_v, 5.0));

    // reading variables through different clients
    sup::dto::int64 int64_v = 42;
    EXPECT_TRUE(WaitForValue(pv_as_int64, int64_v, 5.0));

    std::string string_v = "42";
    EXPECT_TRUE(WaitForValue(pv_as_string, string_v, 5.0));
    EXPECT_EQ(pv_as_string.GetValue().As<std::string>(), string_v);

    sup::dto::boolean bool_v = true;
    EXPECT_TRUE(WaitForValue(pv_as_bool, bool_v, 5.0));
  }
  {
    // set second value using a 64bit int on a 32bit channel
    sup::dto::int64 int64_v = 0;
    sup::dto::int32 int32_v = 0;
    ASSERT_TRUE(pv_as_int32.SetValue(int64_v));
    EXPECT_TRUE(WaitForValue(pv_as_int32, int32_v, 5.0));

    // reading variables through different clients
    EXPECT_TRUE(WaitForValue(pv_as_int64, int64_v, 5.0));

    std::string string_v = "0";
    EXPECT_TRUE(WaitForValue(pv_as_string, string_v, 5.0));
    EXPECT_EQ(pv_as_string.GetValue().As<std::string>(), string_v);

    sup::dto::boolean bool_v = false;
    EXPECT_TRUE(WaitForValue(pv_as_bool, bool_v, 5.0));
  }
}

TEST_F(ChannelAccessPVTest, Int64Formats)
{
  using namespace sup::epics;

  // create variables
  ChannelAccessPV pv_as_int64("CA-TESTS:INT64", sup::dto::SignedInteger64Type);
  ChannelAccessPV pv_as_int32("CA-TESTS:INT64", sup::dto::SignedInteger32Type);
  ChannelAccessPV pv_as_string("CA-TESTS:INT64", sup::dto::StringType);
  ChannelAccessPV pv_as_bool("CA-TESTS:INT64", sup::dto::BooleanType);

  // waiting for connected clients
  EXPECT_TRUE(pv_as_int64.WaitForConnected(1.0));
  EXPECT_TRUE(pv_as_int32.WaitForConnected(5.0));
  EXPECT_TRUE(pv_as_string.WaitForConnected(1.0));
  EXPECT_TRUE(pv_as_bool.WaitForConnected(1.0));

  {
    // set first value
    sup::dto::int64 int64_v = 42;
    ASSERT_TRUE(pv_as_int64.SetValue(int64_v));
    EXPECT_TRUE(WaitForValue(pv_as_int64, int64_v, 5.0));

    // reading variables through different clients
    sup::dto::int32 int32_v = 42;
    EXPECT_TRUE(WaitForValue(pv_as_int32, int32_v, 5.0));

    std::string string_v = "42";
    EXPECT_TRUE(WaitForValue(pv_as_string, string_v, 5.0));
    EXPECT_EQ(pv_as_string.GetValue().As<std::string>(), string_v);

    sup::dto::boolean bool_v = true;
    EXPECT_TRUE(WaitForValue(pv_as_bool, bool_v, 5.0));
  }
  {
    // set second value using a boolean on the 64bit channel
    sup::dto::boolean bool_v = false;
    ASSERT_TRUE(pv_as_int64.SetValue(bool_v));
    sup::dto::int64 int64_v = 0;
    EXPECT_TRUE(WaitForValue(pv_as_int64, int64_v, 5.0));

    // reading variables through different clients
    sup::dto::int32 int32_v = 0;
    EXPECT_TRUE(WaitForValue(pv_as_int32, int32_v, 5.0));

    std::string string_v = "0";
    EXPECT_TRUE(WaitForValue(pv_as_string, string_v, 5.0));
    EXPECT_EQ(pv_as_string.GetValue().As<std::string>(), string_v);

    EXPECT_TRUE(WaitForValue(pv_as_bool, bool_v, 5.0));
  }
}

TEST_F(ChannelAccessPVTest, Int64WithCallback)
{
  using namespace sup::epics;

  // setup callback
  std::mutex mtx;
  std::condition_variable cv;
  sup::dto::AnyValue read_back;
  ChannelAccessPV::VariableChangedCallback callback =
    [&](const ChannelAccessPV::ExtendedValue& ext_val) {
      if (ext_val.connected) {
        std::lock_guard<std::mutex> lk{mtx};
        read_back = ext_val.value;
        cv.notify_one();
      }
    };
  // create variable and wait for connected
  ChannelAccessPV pv_as_int64("CA-TESTS:INT64", sup::dto::SignedInteger64Type, callback);
  EXPECT_TRUE(pv_as_int64.WaitForConnected(1.0));

  {
    // set first value
    sup::dto::int64 int64_v = 42;
    ASSERT_TRUE(pv_as_int64.SetValue(int64_v));
    auto predicate = [&]() {
      return read_back.GetType() == sup::dto::SignedInteger64Type &&
             read_back.As<sup::dto::int64>() == int64_v;
    };
    std::unique_lock<std::mutex> lk{mtx};
    EXPECT_TRUE(cv.wait_for(lk, std::chrono::seconds(2), predicate));
  }
  {
    // set second value
    sup::dto::int64 int64_v = 1729;
    ASSERT_TRUE(pv_as_int64.SetValue(int64_v));
    auto predicate = [&]() {
      return read_back.GetType() == sup::dto::SignedInteger64Type &&
             read_back.As<sup::dto::int64>() == int64_v;
    };
    std::unique_lock<std::mutex> lk{mtx};
    EXPECT_TRUE(cv.wait_for(lk, std::chrono::seconds(2), predicate));
  }
}

TEST_F(ChannelAccessPVTest, EnumFormats)
{
  using namespace sup::epics;

  // create variables
  ChannelAccessPV pv_as_enum("CA-TESTS:ENUM", sup::dto::UnsignedInteger16Type);
  ChannelAccessPV pv_as_string("CA-TESTS:ENUM", sup::dto::StringType);
  ChannelAccessPV pv_as_bool("CA-TESTS:ENUM", sup::dto::BooleanType);
  ChannelAccessPV pv_as_uint8("CA-TESTS:ENUM", sup::dto::UnsignedInteger8Type);
  ChannelAccessPV pv_as_int32("CA-TESTS:ENUM", sup::dto::SignedInteger32Type);
  ChannelAccessPV pv_as_uint32("CA-TESTS:ENUM", sup::dto::UnsignedInteger32Type);
  ChannelAccessPV pv_as_int64("CA-TESTS:ENUM", sup::dto::SignedInteger64Type);
  ChannelAccessPV pv_as_uint64("CA-TESTS:ENUM", sup::dto::UnsignedInteger64Type);

  // waiting for connected clients
  EXPECT_TRUE(pv_as_enum.WaitForConnected(5.0));
  EXPECT_TRUE(pv_as_string.WaitForConnected(1.0));
  EXPECT_TRUE(pv_as_bool.WaitForConnected(1.0));
  EXPECT_TRUE(pv_as_uint8.WaitForConnected(1.0));
  EXPECT_TRUE(pv_as_int32.WaitForConnected(1.0));
  EXPECT_TRUE(pv_as_uint32.WaitForConnected(1.0));
  EXPECT_TRUE(pv_as_uint64.WaitForConnected(1.0));

  // Enumeration records cannot be read out by integer types that require string backends in CA:
  EXPECT_FALSE(pv_as_int64.WaitForValidValue(0.5));

  {
    // set first value
    sup::dto::uint16 uint16_v = 3;
    ASSERT_TRUE(pv_as_enum.SetValue(uint16_v));
    EXPECT_TRUE(WaitForValue(pv_as_enum, uint16_v, 5.0));

    // reading variables through different clients
    std::string string_v = "Ready";
    EXPECT_TRUE(WaitForValue(pv_as_string, string_v, 5.0));
    EXPECT_EQ(pv_as_string.GetValue().As<std::string>(), string_v);

    sup::dto::boolean bool_v = true;
    EXPECT_TRUE(WaitForValue(pv_as_bool, bool_v, 5.0));

    sup::dto::uint8 uint8_v = 3u;
    EXPECT_TRUE(WaitForValue(pv_as_uint8, uint8_v, 5.0));

    sup::dto::int32 int32_v = 3;
    EXPECT_TRUE(WaitForValue(pv_as_int32, int32_v, 5.0));

    sup::dto::uint32 uint32_v = 3u;
    EXPECT_TRUE(WaitForValue(pv_as_uint32, uint32_v, 5.0));

    sup::dto::uint64 uint64_v = 3u;
    EXPECT_TRUE(WaitForValue(pv_as_uint64, uint64_v, 5.0));
  }
  {
    // set second value
    std::string string_v = "Undefined";
    ASSERT_TRUE(pv_as_string.SetValue(string_v));
    EXPECT_TRUE(WaitForValue(pv_as_string, string_v, 5.0));
    EXPECT_EQ(pv_as_string.GetValue().As<std::string>(), string_v);

    // reading variables through different clients
    sup::dto::uint16 uint16_v = 0;
    EXPECT_TRUE(WaitForValue(pv_as_enum, uint16_v, 5.0));

    sup::dto::boolean bool_v = false;
    EXPECT_TRUE(WaitForValue(pv_as_bool, bool_v, 5.0));

    sup::dto::uint8 uint8_v = 0u;
    EXPECT_TRUE(WaitForValue(pv_as_uint8, uint8_v, 5.0));

    sup::dto::int32 int32_v = 0;
    EXPECT_TRUE(WaitForValue(pv_as_int32, int32_v, 5.0));

    sup::dto::uint32 uint32_v = 0u;
    EXPECT_TRUE(WaitForValue(pv_as_uint32, uint32_v, 5.0));

    sup::dto::uint64 uint64_v = 0u;
    EXPECT_TRUE(WaitForValue(pv_as_uint64, uint64_v, 5.0));
  }
}

TEST_F(ChannelAccessPVTest, CharWaveform)
{
  using namespace sup::epics;

  sup::dto::AnyType char_array_t(1024, sup::dto::Character8Type, "char8[]");
  ChannelAccessPV ca_chararray_var("CA-TESTS:CHARRAY", char_array_t);
  sup::dto::AnyType bool_array_t(1024, sup::dto::BooleanType, "bool[]");
  ChannelAccessPV ca_boolarray_var("CA-TESTS:CHARRAY", bool_array_t);
  sup::dto::AnyType int32_array_t(1024, sup::dto::SignedInteger32Type, "int32[]");
  ChannelAccessPV ca_int32array_var("CA-TESTS:CHARRAY", int32_array_t);
  sup::dto::AnyType uint64_array_t(1024, sup::dto::UnsignedInteger64Type, "uint64[]");
  ChannelAccessPV ca_uint64array_var("CA-TESTS:CHARRAY", uint64_array_t);

  // waiting for connected clients
  EXPECT_TRUE(ca_chararray_var.WaitForConnected(5.0));
  EXPECT_TRUE(ca_boolarray_var.WaitForConnected(1.0));
  EXPECT_TRUE(ca_int32array_var.WaitForConnected(1.0));
  EXPECT_TRUE(ca_uint64array_var.WaitForConnected(1.0));

  {
    // set first value
    sup::dto::AnyValue char_array_v{char_array_t};
    ASSERT_TRUE(ca_chararray_var.SetValue(char_array_v));
    EXPECT_TRUE(WaitForValue(ca_chararray_var, char_array_v, 5.0));

    // reading variables through different clients
    sup::dto::AnyValue bool_array_v{bool_array_t};
    EXPECT_TRUE(WaitForValue(ca_boolarray_var, bool_array_v, 5.0));

    sup::dto::AnyValue int32_array_v{int32_array_t};
    EXPECT_TRUE(WaitForValue(ca_int32array_var, int32_array_v, 5.0));

    sup::dto::AnyValue uint64_array_v{uint64_array_t};
    EXPECT_TRUE(WaitForValue(ca_uint64array_var, uint64_array_v, 5.0));
  }
  {
    // set first value
    sup::dto::AnyValue char_array_v{char_array_t};
    FillWithScalars(char_array_v, 1);
    ASSERT_TRUE(ca_chararray_var.SetValue(char_array_v));
    EXPECT_TRUE(WaitForValue(ca_chararray_var, char_array_v, 5.0));

    // reading variables through different clients
    sup::dto::AnyValue bool_array_v{bool_array_t};
    FillWithScalars(bool_array_v, 1);
    EXPECT_TRUE(WaitForValue(ca_boolarray_var, bool_array_v, 5.0));

    sup::dto::AnyValue int32_array_v{int32_array_t};
    FillWithScalars(int32_array_v, 1);
    EXPECT_TRUE(WaitForValue(ca_int32array_var, int32_array_v, 5.0));

    sup::dto::AnyValue uint64_array_v{uint64_array_t};
    FillWithScalars(uint64_array_v, 1);
    EXPECT_TRUE(WaitForValue(ca_uint64array_var, uint64_array_v, 5.0));
  }
}

TEST_F(ChannelAccessPVTest, UInt64Waveform)
{
  using namespace sup::epics;

  sup::dto::AnyType uint64_array_t(10, sup::dto::UnsignedInteger64Type, "uint64[]");
  ChannelAccessPV ca_uint64array_var("CA-TESTS:UINT64ARRAY", uint64_array_t);
  sup::dto::AnyType bool_array_t(10, sup::dto::BooleanType, "bool[]");
  ChannelAccessPV ca_boolarray_var("CA-TESTS:UINT64ARRAY", bool_array_t);
  sup::dto::AnyType uint8_array_t(10, sup::dto::UnsignedInteger8Type, "uint8[]");
  ChannelAccessPV ca_uint8array_var("CA-TESTS:UINT64ARRAY", uint8_array_t);
  sup::dto::AnyType uint32_array_t(10, sup::dto::UnsignedInteger32Type, "uint32[]");
  ChannelAccessPV ca_uint32array_var("CA-TESTS:UINT64ARRAY", uint32_array_t);
  sup::dto::AnyType int32_array_t(10, sup::dto::SignedInteger32Type, "int32[]");
  ChannelAccessPV ca_int32array_var("CA-TESTS:UINT64ARRAY", int32_array_t);

  // waiting for connected clients
  EXPECT_TRUE(ca_uint64array_var.WaitForConnected(5.0));
  EXPECT_TRUE(ca_boolarray_var.WaitForConnected(1.0));
  EXPECT_TRUE(ca_uint8array_var.WaitForConnected(1.0));
  EXPECT_TRUE(ca_uint32array_var.WaitForConnected(1.0));
  EXPECT_TRUE(ca_int32array_var.WaitForConnected(1.0));

  {
    // set first value
    sup::dto::AnyValue uint64_array_v{uint64_array_t};
    ASSERT_TRUE(ca_uint64array_var.SetValue(uint64_array_v));
    EXPECT_TRUE(WaitForValue(ca_uint64array_var, uint64_array_v, 5.0));

    // reading variables through different clients
    sup::dto::AnyValue bool_array_v{bool_array_t};
    EXPECT_TRUE(WaitForValue(ca_boolarray_var, bool_array_v, 5.0));

    sup::dto::AnyValue uint8_array_v{uint8_array_t};
    EXPECT_TRUE(WaitForValue(ca_uint8array_var, uint8_array_v, 5.0));

    sup::dto::AnyValue uint32_array_v{uint32_array_t};
    EXPECT_TRUE(WaitForValue(ca_uint32array_var, uint32_array_v, 5.0));

    sup::dto::AnyValue int32_array_v{int32_array_t};
    EXPECT_TRUE(WaitForValue(ca_int32array_var, int32_array_v, 5.0));
  }
  {
    // set first value
    sup::dto::AnyValue uint64_array_v{uint64_array_t};
    FillWithScalars(uint64_array_v, 1);
    ASSERT_TRUE(ca_uint64array_var.SetValue(uint64_array_v));
    EXPECT_TRUE(WaitForValue(ca_uint64array_var, uint64_array_v, 5.0));

    // reading variables through different clients
    sup::dto::AnyValue bool_array_v{bool_array_t};
    FillWithScalars(bool_array_v, 1);
    EXPECT_TRUE(WaitForValue(ca_boolarray_var, bool_array_v, 5.0));

    sup::dto::AnyValue uint8_array_v{uint8_array_t};
    FillWithScalars(uint8_array_v, 1);
    EXPECT_TRUE(WaitForValue(ca_uint8array_var, uint8_array_v, 5.0));

    sup::dto::AnyValue uint32_array_v{uint32_array_t};
    FillWithScalars(uint32_array_v, 1);
    EXPECT_TRUE(WaitForValue(ca_uint32array_var, uint32_array_v, 5.0));

    sup::dto::AnyValue int32_array_v{int32_array_t};
    FillWithScalars(int32_array_v, 1);
    EXPECT_TRUE(WaitForValue(ca_int32array_var, int32_array_v, 5.0));
  }
}

TEST_F(ChannelAccessPVTest, ShortenedWaveform)
{
  using namespace sup::epics;

  sup::dto::AnyType float_array_t(6, sup::dto::Float32Type, "float32[]");
  ChannelAccessPV ca_floatarray_var("CA-TESTS:SHORTFLOATARRAY", float_array_t);

  // waiting for connected client and valid value
  EXPECT_TRUE(ca_floatarray_var.WaitForConnected(5.0));
  EXPECT_TRUE(ca_floatarray_var.WaitForValidValue(5.0));

  sup::dto::AnyValue float_array_v{float_array_t};
  float_array_v[0] = 1.0f;
  float_array_v[1] = 2.0f;
  float_array_v[2] = 3.0f;
  float_array_v[3] = 4.0f;
  float_array_v[4] = 5.0f;
  EXPECT_TRUE(WaitForValue(ca_floatarray_var, float_array_v, 5.0));
}

TEST_F(ChannelAccessPVTest, DISABLED_ShortLivedPV)
{
  using namespace sup::epics;
  using sup::epics::test::CAPut;

  // create reader pv
  ChannelAccessPV reader_pv("CA-TESTS:BOOL", sup::dto::BooleanType);
  EXPECT_TRUE(reader_pv.WaitForValidValue(5.0));

  // set first value through shortlived pv
  bool value = false;
  {
    ChannelAccessPV writer_pv("CA-TESTS:BOOL", sup::dto::BooleanType);
    EXPECT_TRUE(writer_pv.WaitForConnected(5.0));

    EXPECT_TRUE(writer_pv.SetValue(value));
    // Uncommmenting the next line will make the test pass
    // std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // wait for value to reach reader pv
  EXPECT_TRUE(WaitForValue(reader_pv, value, 2.0));
  CAPut("CA-TESTS:BOOL", "TRUE");
  EXPECT_TRUE(WaitForValue(reader_pv, true, 2.0));
  CAPut("CA-TESTS:BOOL", "FALSE");
  EXPECT_TRUE(WaitForValue(reader_pv, false, 2.0));

  // set second value through shortlived pv
  value = true;
  {
    ChannelAccessPV writer_pv("CA-TESTS:BOOL", sup::dto::BooleanType);
    EXPECT_TRUE(writer_pv.WaitForConnected(5.0));

    EXPECT_TRUE(writer_pv.SetValue(value));
    // Uncommmenting the next line will make the test pass
    // std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // wait for value to reach reader pv
  EXPECT_TRUE(WaitForValue(reader_pv, value, 2.0));

  // set pv value from command line
  CAPut("CA-TESTS:BOOL", "FALSE");
  EXPECT_TRUE(WaitForValue(reader_pv, false, 2.0));
  CAPut("CA-TESTS:BOOL", "TRUE");
  EXPECT_TRUE(WaitForValue(reader_pv, true, 2.0));
}

static bool WaitForValue(const sup::epics::ChannelAccessPV& variable,
                         const sup::dto::AnyValue& expected_value, double timeout_sec)
{
  auto timeout =
      std::chrono::system_clock::now() + std::chrono::duration<double>(timeout_sec);
  sup::dto::AnyValue value_read = variable.GetValue();
  while (value_read != expected_value)
  {
    if (std::chrono::system_clock::now() > timeout)
    {
      return false;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    value_read = variable.GetValue();
  }
  return true;
}
