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

#include <sup/dto/AnyType.h>
#include <sup/dto/AnyValue.h>
#include <cstring>
#include <thread>

#include "SoftIocRunner.h"
#include "SoftIocUtils.h"
#include "sup/epics/ChannelAccessVariable.h"

static const unsigned long SECOND = 1000000000ul;

class ChannelAccessVariableTest : public ::testing::Test
{
public:
  // Create single service for all tests.
  static void SetUpTestCase()
  {
    m_softioc_service.Start(GetEpicsDBContentString());
  }

  static void TearDownTestCase()
  {
    m_softioc_service.Stop();
  }

  static SoftIocRunner m_softioc_service;
};

SoftIocRunner ChannelAccessVariableTest::m_softioc_service{"ChannelAccessVariableTest"};

TEST_F(ChannelAccessVariableTest, SingleReadWrite)
{
  using namespace sup::epics;

  // Wait for softIoc to be running
  ASSERT_TRUE(m_softioc_service.WaitForVariable("CA-TESTS:FLOAT"));

  // preparing variables
  ChannelAccessVariable ca_bool_var("CA-TESTS:BOOL", sup::dto::Boolean);
  ChannelAccessVariable ca_float_var("CA-TESTS:FLOAT", sup::dto::Float32);
  ChannelAccessVariable ca_string_var("CA-TESTS:STRING", sup::dto::String);

  sup::dto::AnyType char_array_t(1024, sup::dto::Character8, "char8[]");
  ChannelAccessVariable ca_chararray_var("CA-TESTS:CHARRAY", char_array_t);

  // waiting for variables to connect
  EXPECT_TRUE(ca_bool_var.WaitForConnected(5.0));
  EXPECT_TRUE(ca_float_var.WaitForConnected(1.0));
  EXPECT_TRUE(ca_string_var.WaitForConnected(1.0));
  EXPECT_TRUE(ca_chararray_var.WaitForConnected(1.0));

  // set bool
  sup::dto::boolean bool_val = true;
  EXPECT_TRUE(ca_bool_var.SetValue(bool_val));

  // set float
  sup::dto::float32 float_val = 0.25f;
  EXPECT_TRUE(ca_float_var.SetValue(float_val));

  // set string
  std::string string_val = "some value";
  EXPECT_TRUE(ca_string_var.SetValue(string_val));

  std::this_thread::sleep_for(std::chrono::nanoseconds(SECOND));

  // reading bool
  sup::dto::AnyValue bool_read;
  EXPECT_NO_THROW(bool_read = ca_bool_var.GetValue());
  EXPECT_EQ(bool_read, bool_val);

  // reading float
  sup::dto::AnyValue float_read;
  EXPECT_NO_THROW(float_read = ca_float_var.GetValue());
  EXPECT_EQ(float_read.As<sup::dto::float32>(), float_val);

  // reading string
  sup::dto::AnyValue string_read;
  EXPECT_NO_THROW(string_read = ca_string_var.GetValue());
  EXPECT_EQ(string_read, string_val);


  // writing too long string
  sup::dto::AnyValue chararray_val(1024, sup::dto::Character8, "char8[]");
  sup::dto::char8 char_data[1024] =
        "Some very long string which is longer than the maximum length of EPICSv3 string and "
        "should be serialised on a waveform record";
  sup::dto::AssignFromCType(chararray_val, char_data);
  EXPECT_TRUE(ca_chararray_var.SetValue(chararray_val));

  std::this_thread::sleep_for(std::chrono::nanoseconds(SECOND));

  // Checking the value of long variable. Not clear what should be here.
  sup::dto::AnyValue chararray_read;
  EXPECT_NO_THROW(chararray_read = ca_chararray_var.GetValue());
  auto chararray_bytes = sup::dto::ToBytes(chararray_read);

  auto array_compare = std::memcmp(chararray_bytes.data(), char_data, 1024);
  EXPECT_EQ(array_compare, 0);

  // Retrieve timestamp
  auto now = std::chrono::system_clock::now();
  auto now_timestamp =
    std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
  ChannelAccessVariable::ExtendedValue ext_boolean;
  EXPECT_NO_THROW(ext_boolean = ca_bool_var.GetExtendedValue());

  auto timestamp = ext_boolean.timestamp;
  EXPECT_TRUE(now_timestamp > timestamp);
  EXPECT_NE(timestamp, 0);
}

TEST_F(ChannelAccessVariableTest, MultipleReadWrite)
{
  using namespace sup::epics;

  // Wait for softIoc to be running
  ASSERT_TRUE(m_softioc_service.WaitForVariable("CA-TESTS:BOOL"));

  // create variables
  ChannelAccessVariable ca_float_writer("CA-TESTS:FLOAT", sup::dto::Float32);
  ChannelAccessVariable ca_float_reader("CA-TESTS:FLOAT", sup::dto::Float32);

  // waiting for connected clients
  EXPECT_TRUE(ca_float_writer.WaitForConnected(5.0));
  EXPECT_TRUE(ca_float_reader.WaitForConnected(5.0));

  // set first value
  sup::dto::float32 value1 = 3.5f;
  ASSERT_TRUE(ca_float_writer.SetValue(value1));

  std::this_thread::sleep_for(std::chrono::nanoseconds(SECOND));

  // reading variable through first client
  sup::dto::AnyValue float_read(sup::dto::Float32);
  EXPECT_NO_THROW(float_read = ca_float_writer.GetValue());
  EXPECT_FLOAT_EQ(float_read.As<sup::dto::float32>(), value1);

  // reading variable through second client
  float_read = 0.0f;
  EXPECT_NO_THROW(float_read = ca_float_reader.GetValue());
  EXPECT_FLOAT_EQ(float_read.As<sup::dto::float32>(), value1);

  // set second value
  sup::dto::float32 value2 = -1.5f;
  ASSERT_TRUE(ca_float_writer.SetValue(value2));

  std::this_thread::sleep_for(std::chrono::nanoseconds(SECOND));

  // reading variable through first client
  float_read = 0.0f;
  EXPECT_NO_THROW(float_read = ca_float_writer.GetValue());
  EXPECT_FLOAT_EQ(float_read.As<sup::dto::float32>(), value2);

  // reading variable through second client
  float_read = 0.0f;
  EXPECT_NO_THROW(float_read = ca_float_reader.GetValue());
  EXPECT_FLOAT_EQ(float_read.As<sup::dto::float32>(), value2);
}