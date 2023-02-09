/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Walter Van Herck (IO)
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

#include "unit_test_helper.h"

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
  long timeout_ns = std::lround(timeout_sec * 1e9);
  auto time_end = std::chrono::system_clock::now() + std::chrono::nanoseconds(timeout_ns);
  while (!predicate() && std::chrono::system_clock::now() < time_end)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }
  return predicate();
}

}  // namespace test

}  // namespace epics

}  // namespace sup
