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
#include <chrono>
#include <cmath>
#include <cstring>
#include <thread>

#include "softioc_runner.h"
#include "softioc_utils.h"
#include <sup/epics/channel_access_client.h>

static const std::string BOOL_CHANNEL = "CA-TESTS:BOOL";
static const std::string FLOAT_CHANNEL = "CA-TESTS:FLOAT";
static const std::string STRING_CHANNEL = "CA-TESTS:STRING";
static const std::string CHARRAY_CHANNEL = "CA-TESTS:CHARRAY";
static const std::string UNKNOWN_CHANNEL = "CA-TESTS:UNKNOWN";

static bool WaitForValue(const sup::epics::ChannelAccessClient& client, const std::string& name,
                         const sup::dto::AnyValue& expected_value, double timeout_sec);

class ChannelAccessClientTest : public ::testing::Test
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

SoftIocRunner ChannelAccessClientTest::m_softioc_service{"ChannelAccessClientTest"};

TEST_F(ChannelAccessClientTest, SingleClient)
{
  using namespace sup::epics;

  // Assert the softIoc is active
  ASSERT_TRUE(m_softioc_service.IsActive());

  // preparing client
  ChannelAccessClient client;
  EXPECT_TRUE(client.AddVariable(BOOL_CHANNEL, sup::dto::BooleanType));
  EXPECT_TRUE(client.AddVariable(FLOAT_CHANNEL, sup::dto::Float32Type));
  EXPECT_TRUE(client.AddVariable(STRING_CHANNEL, sup::dto::StringType));

  sup::dto::AnyType char_array_t(1024, sup::dto::Character8Type, "char8[]");
  EXPECT_TRUE(client.AddVariable(CHARRAY_CHANNEL, char_array_t));

  // test variable names
  auto var_names = client.GetVariableNames();
  EXPECT_EQ(var_names.size(), 4);
  EXPECT_FALSE(std::find(var_names.begin(), var_names.end(), BOOL_CHANNEL) == var_names.end());
  EXPECT_FALSE(std::find(var_names.begin(), var_names.end(), FLOAT_CHANNEL) == var_names.end());
  EXPECT_FALSE(std::find(var_names.begin(), var_names.end(), STRING_CHANNEL) == var_names.end());
  EXPECT_FALSE(std::find(var_names.begin(), var_names.end(), CHARRAY_CHANNEL) == var_names.end());
  EXPECT_TRUE(std::find(var_names.begin(), var_names.end(), UNKNOWN_CHANNEL) == var_names.end());

  // waiting for variables to connect
  EXPECT_TRUE(client.WaitForConnected(BOOL_CHANNEL, 5.0));
  EXPECT_TRUE(client.WaitForConnected(FLOAT_CHANNEL, 1.0));
  EXPECT_TRUE(client.WaitForConnected(STRING_CHANNEL, 1.0));
  EXPECT_TRUE(client.WaitForConnected(CHARRAY_CHANNEL, 1.0));
  EXPECT_FALSE(client.WaitForConnected(UNKNOWN_CHANNEL, 1.0));

  // checking connected
  EXPECT_TRUE(client.IsConnected(BOOL_CHANNEL));
  EXPECT_TRUE(client.IsConnected(FLOAT_CHANNEL));
  EXPECT_TRUE(client.IsConnected(STRING_CHANNEL));
  EXPECT_TRUE(client.IsConnected(CHARRAY_CHANNEL));
  EXPECT_FALSE(client.IsConnected(UNKNOWN_CHANNEL));

  // set bool
  sup::dto::boolean bool_val = true;
  EXPECT_TRUE(client.SetValue(BOOL_CHANNEL, bool_val));

  // set float
  sup::dto::float32 float_val = 0.25F;
  EXPECT_TRUE(client.SetValue(FLOAT_CHANNEL, float_val));

  // set string
  std::string string_val = "some value";
  EXPECT_TRUE(client.SetValue(STRING_CHANNEL, string_val));

  // set unknown channel
  sup::dto::AnyValue unknown_val = 77;
  EXPECT_FALSE(client.SetValue(UNKNOWN_CHANNEL, unknown_val));

  // reading bool
  EXPECT_TRUE(WaitForValue(client, BOOL_CHANNEL, bool_val, 5.0));

  // reading float
  EXPECT_TRUE(WaitForValue(client, FLOAT_CHANNEL, float_val, 5.0));

  // reading string
  EXPECT_TRUE(WaitForValue(client, STRING_CHANNEL, string_val, 5.0));

  // writing long string
  sup::dto::AnyValue chararray_val(1024, sup::dto::Character8Type, "char8[]");
  sup::dto::char8 char_data[1024] =
        "Some very long string which is longer than the maximum length of EPICSv3 string and "
        "should be serialised on a waveform record";
  sup::dto::AssignFromCType(chararray_val, char_data);
  EXPECT_TRUE(client.SetValue(CHARRAY_CHANNEL, chararray_val));

  // reading long string
  EXPECT_TRUE(WaitForValue(client, CHARRAY_CHANNEL, chararray_val, 5.0));

  // retrieve timestamp
  auto now = std::chrono::system_clock::now();
  auto now_timestamp =
    std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
  ChannelAccessPV::ExtendedValue ext_boolean;
  EXPECT_NO_THROW(ext_boolean = client.GetExtendedValue(BOOL_CHANNEL));

  auto timestamp = ext_boolean.timestamp;
  EXPECT_TRUE(now_timestamp > timestamp);
  EXPECT_NE(timestamp, 0);

  // remove variable
  EXPECT_TRUE(client.RemoveVariable(CHARRAY_CHANNEL));
  EXPECT_FALSE(client.RemoveVariable(UNKNOWN_CHANNEL));

    // retest variable names
  var_names = client.GetVariableNames();
  EXPECT_EQ(var_names.size(), 3);
  EXPECT_FALSE(std::find(var_names.begin(), var_names.end(), BOOL_CHANNEL) == var_names.end());
  EXPECT_FALSE(std::find(var_names.begin(), var_names.end(), FLOAT_CHANNEL) == var_names.end());
  EXPECT_FALSE(std::find(var_names.begin(), var_names.end(), STRING_CHANNEL) == var_names.end());
  EXPECT_TRUE(std::find(var_names.begin(), var_names.end(), CHARRAY_CHANNEL) == var_names.end());
  EXPECT_TRUE(std::find(var_names.begin(), var_names.end(), UNKNOWN_CHANNEL) == var_names.end());
}

TEST_F(ChannelAccessClientTest, MultipleClients)
{
  using namespace sup::epics;

  // Assert the softIoc is active
  ASSERT_TRUE(m_softioc_service.IsActive());

  // create clients
  ChannelAccessClient client_writer;
  EXPECT_TRUE(client_writer.AddVariable(FLOAT_CHANNEL, sup::dto::Float32Type));
  ChannelAccessClient client_reader;
  EXPECT_TRUE(client_reader.AddVariable(FLOAT_CHANNEL, sup::dto::Float32Type));

  // waiting for connected clients
  EXPECT_TRUE(client_writer.WaitForConnected(FLOAT_CHANNEL, 5.0));
  EXPECT_TRUE(client_reader.WaitForConnected(FLOAT_CHANNEL, 5.0));

  // set first value
  sup::dto::float32 value1 = 3.5F;
  ASSERT_TRUE(client_writer.SetValue(FLOAT_CHANNEL, value1));

  // reading variable through first client
  EXPECT_TRUE(WaitForValue(client_writer, FLOAT_CHANNEL, value1, 5.0));

  // reading variable through second client
  EXPECT_TRUE(WaitForValue(client_reader, FLOAT_CHANNEL, value1, 5.0));

  // set second value
  sup::dto::float32 value2 = -1.5F;
  ASSERT_TRUE(client_writer.SetValue(FLOAT_CHANNEL, value2));

  // reading variable through first client
  EXPECT_TRUE(WaitForValue(client_writer, FLOAT_CHANNEL, value2, 5.0));

  // reading variable through second client
  EXPECT_TRUE(WaitForValue(client_reader, FLOAT_CHANNEL, value2, 5.0));
}

static bool WaitForValue(const sup::epics::ChannelAccessClient& client,
                         const std::string& name, const sup::dto::AnyValue& expected_value,
                         double timeout_sec)
{
  auto timeout = std::chrono::system_clock::now() +
                 std::chrono::nanoseconds(std::lround(timeout_sec * 1e9));
  sup::dto::AnyValue value_read = client.GetValue(name);
  while (value_read != expected_value)
  {
    if (std::chrono::system_clock::now() > timeout)
    {
      return false;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    value_read = client.GetValue(name);
  }
  return true;
}