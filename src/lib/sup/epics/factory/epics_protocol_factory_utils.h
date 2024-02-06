/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2024 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 *****************************************************************************/

#ifndef SUP_EPICS_EPICS_PROTOCOL_FACTORY_UTILS_H_
#define SUP_EPICS_EPICS_PROTOCOL_FACTORY_UTILS_H_

#include <sup/epics/pv_access_rpc_client_config.h>
#include <sup/epics/pv_access_rpc_server_config.h>

#include <sup/protocol/protocol_factory.h>
#include <sup/protocol/protocol_rpc.h>
#include <sup/dto/any_functor.h>

namespace sup
{
namespace epics
{
namespace utils
{
PvAccessRPCServerConfig ParsePvAccessRPCServerConfig(const sup::dto::AnyValue& config);

PvAccessRPCClientConfig ParsePvAccessRPCClientConfig(const sup::dto::AnyValue& config);

std::unique_ptr<sup::protocol::ProcessVariable> CreateChannelAccessClientVar(
  const sup::dto::AnyValue& config);

std::unique_ptr<sup::protocol::ProcessVariable> CreatePvAccessClientVar(
  const sup::dto::AnyValue& config);

std::unique_ptr<sup::protocol::ProcessVariable> CreatePvAccessServerVar(
  const sup::dto::AnyValue& config);

}  // namespace utils

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_EPICS_PROTOCOL_FACTORY_UTILS_H_
