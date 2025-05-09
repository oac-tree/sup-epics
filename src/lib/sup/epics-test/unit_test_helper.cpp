/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Walter Van Herck (IO)
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

#include "unit_test_helper.h"

#include <chrono>
#include <cmath>
#include <thread>

namespace sup
{
namespace epics
{
namespace test
{
bool BusyWaitFor(double timeout_sec, std::function<bool()> predicate)
{
  auto timedelta = std::chrono::duration<double>(timeout_sec);
  auto time_end = std::chrono::system_clock::now() + timedelta;
  while (!predicate() && std::chrono::system_clock::now() < time_end)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }
  return predicate();
}

}  // namespace test

}  // namespace epics

}  // namespace sup
