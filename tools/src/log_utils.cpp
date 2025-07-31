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

#include "log_utils.h"

#include <sup/dto/anyvalue_helper.h>

#include <iostream>

void PrintAnyvaluePacket(const std::string& title, const sup::dto::AnyValue& value)
{
  std::cout << title << std::endl;
  std::cout << std::string(title.size(), '-') << std::endl;
  std::cout << sup::dto::PrintAnyValue(value) << std::endl;
  std::cout << std::endl;
}