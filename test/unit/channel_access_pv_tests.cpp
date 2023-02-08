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
 * Copyright (c) : 2010-2021 ITER Organization,
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

  // check bool
  auto chararray_val = ca_chararray_var.GetValue();
  EXPECT_FALSE(sup::dto::IsEmptyValue(chararray_val));
  EXPECT_EQ(chararray_val.GetType(), char_array_t);
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
  EXPECT_TRUE(ca_float_reader.WaitForConnected(5.0));

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

TEST_F(ChannelAccessPVTest, DISABLED_ShortLivedPV)
{
  using namespace sup::epics;

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
      std::chrono::system_clock::now() + std::chrono::nanoseconds(std::lround(timeout_sec * 1e9));
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
