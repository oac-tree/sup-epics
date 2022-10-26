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

#ifndef SUP_EPICS_PV_ACCESS_RPC_UTILS_H_
#define SUP_EPICS_PV_ACCESS_RPC_UTILS_H_

#include <sup/dto/anyvalue.h>

namespace sup
{
namespace epics
{
namespace utils
{

sup::dto::uint64 GetTimestamp();

sup::dto::AnyValue CreateRPCRequest(const sup::dto::AnyValue& payload);

}  // namespace utils

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_PV_ACCESS_RPC_UTILS_H_
