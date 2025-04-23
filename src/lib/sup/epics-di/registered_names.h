/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : SUP EPICS
 *
 * Description   : SUP EPICS framework
 *
 * Author        : Walter Van Herck
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

#ifndef SUP_EPICS_DI_REGISTERED_NAMES_H_
#define SUP_EPICS_DI_REGISTERED_NAMES_H_

#include <string>

namespace sup
{
namespace epics
{

// PvAccessRPCClient registered names:
const std::string PV_ACCESS_RPC_CLIENT_CONFIG_DEFAULT = "PvAccessRPCClientConfigDefault";
const std::string PV_ACCESS_RPC_CLIENT_CONFIG = "PvAccessRPCClientConfig";
const std::string PV_ACCESS_RPC_CLIENT = "PvAccessRPCClient";

// PvAccessRPCServer registered names:
const std::string PV_ACCESS_RPC_SERVER_CONFIG = "PvAccessRPCServerConfig";
const std::string PV_ACCESS_RPC_SERVER = "PvAccessRPCServer";

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_DI_REGISTERED_NAMES_H_
