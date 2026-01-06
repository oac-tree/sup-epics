/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Walter Van Herck (IO)
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

#include <cadef.h>

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <functional>
#include <thread>

static bool WaitForValue(std::atomic_int& var, std::function<bool(int)> pred, double timeout_sec);

void Monitor_CB(event_handler_args args);
void Connection_CB(connection_handler_args args);

static std::atomic_int mon_count{0};
static std::atomic_int conn_count{0};

static const char* channel_name = "CA-TESTS:LONG";

/**
 * Preconditions for this test:
 * There has to be an EPICS longout record available, with a current value not equal to 1
*/
TEST(ChannelAccessBaseTest, DISABLED_MissedCallback)
{
  ASSERT_EQ(ca_context_create(ca_enable_preemptive_callback), ECA_NORMAL);

  // Preconditions
  mon_count.store(0);
  conn_count.store(0);
  EXPECT_EQ(mon_count, 0);
  EXPECT_EQ(conn_count, 0);

  // Create two channels to the same pv and subscriptions
  chid id_1, id_2;
  EXPECT_EQ(ca_create_channel(channel_name, &Connection_CB, nullptr, 10, &id_1), ECA_NORMAL);
  EXPECT_EQ(ca_create_channel(channel_name, &Connection_CB, nullptr, 10, &id_2), ECA_NORMAL);
  ASSERT_NE(id_1, nullptr);
  ASSERT_NE(id_2, nullptr);
  EXPECT_EQ(ca_create_subscription(DBR_TIME_LONG, 0, id_1, DBE_VALUE | DBE_ALARM, &Monitor_CB,
                                   nullptr, nullptr), ECA_NORMAL);
  EXPECT_EQ(ca_create_subscription(DBR_TIME_LONG, 0, id_2, DBE_VALUE | DBE_ALARM, &Monitor_CB,
                                   nullptr, nullptr), ECA_NORMAL);
  EXPECT_EQ(ca_flush_io(), ECA_NORMAL);

  // Expect callbacks to be called for both channels
  auto equals_2 = [](int val){ return val == 2; };
  EXPECT_TRUE(WaitForValue(conn_count, equals_2, 2.0));
  EXPECT_TRUE(WaitForValue(mon_count, equals_2, 2.0));
  EXPECT_EQ(conn_count, 2);
  EXPECT_EQ(mon_count, 2);

  // Set PV value through second channel
  int dummy = 1;
  EXPECT_EQ(ca_array_put(DBR_LONG, 1u, id_2, &dummy), ECA_NORMAL);
  EXPECT_EQ(ca_flush_io(), ECA_NORMAL);

  // Expect monitoring callbacks to be called twice since last check
  auto equals_4 = [](int val){ return val == 4; };
  EXPECT_TRUE(WaitForValue(mon_count, equals_4, 2.0));
  EXPECT_EQ(conn_count, 2);
  EXPECT_EQ(mon_count, 4);

  // Set PV value through second channel again and destroy that channel immediately
  dummy = 2;
  EXPECT_EQ(ca_array_put(DBR_LONG, 1u, id_2, &dummy), ECA_NORMAL);
  EXPECT_EQ(ca_flush_io(), ECA_NORMAL);
  // Uncommenting the next line will fix all checks!
  // std::this_thread::sleep_for(std::chrono::milliseconds(10));
  EXPECT_EQ(ca_clear_channel(id_2), ECA_NORMAL);
  EXPECT_EQ(ca_flush_io(), ECA_NORMAL);

  // The following two checks are the problematic ones, with everything else passing.
  // The check on the connection count is not an issue.
  // Expect monitoring callbacks to be called twice since last check
  auto larger_than_4 = [](int val){ return val > 4; };
  EXPECT_TRUE(WaitForValue(mon_count, larger_than_4, 2.0));
  EXPECT_GT(mon_count, 4);
  EXPECT_EQ(conn_count, 2);

  // Read PV value, using the first channel. Check that the PV was updated.
  int val_read;
  EXPECT_EQ(ca_array_get(DBR_LONG, 1u, id_1, &val_read), ECA_NORMAL);
  EXPECT_EQ(ca_pend_io(1), ECA_NORMAL);
  EXPECT_EQ(val_read, 2);

  // Clear remaining channel and destroy context
  EXPECT_EQ(ca_clear_channel(id_1), ECA_NORMAL);
  ca_context_destroy();
}

static bool WaitForValue(std::atomic_int& var, std::function<bool(int)> pred, double timeout_sec)
{
  auto timeout = std::chrono::system_clock::now() +
                 std::chrono::duration<double>(timeout_sec);
  int current = var.load();
  while (!pred(current))
  {
    if (std::chrono::system_clock::now() > timeout)
    {
      return false;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    current = var.load();
  }
  return true;
}

void Monitor_CB(event_handler_args /*args*/)
{
  mon_count++;
}

void Connection_CB(connection_handler_args /*args*/)
{
  conn_count++;
}
