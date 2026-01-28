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

#include <sup/epics/pv_access_rpc_client_config.h>

static const double DEFAULT_TIMEOUT_SECONDS = 5.0;

namespace sup
{
namespace epics
{

PvAccessRPCClientConfig GetDefaultRPCClientConfig(const std::string& service_name)
{
  return { service_name, DEFAULT_TIMEOUT_SECONDS };
}

}  // namespace epics

}  // namespace sup
