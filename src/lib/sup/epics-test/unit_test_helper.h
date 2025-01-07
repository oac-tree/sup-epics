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
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 *****************************************************************************/

#ifndef SUP_EPICS_TEST_UNIT_TEST_HELPER_H_
#define SUP_EPICS_TEST_UNIT_TEST_HELPER_H_

#include <functional>

namespace sup
{
namespace epics
{
namespace test
{

bool BusyWaitFor(double timeout_sec, std::function<bool()> predicate);

}  // namespace test

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_TEST_UNIT_TEST_HELPER_H_
