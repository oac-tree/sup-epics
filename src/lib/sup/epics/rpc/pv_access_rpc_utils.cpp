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

#include "pv_access_rpc_utils.h"

#include <sup/epics/utils/dto_conversion_utils.h>

#include <sup/protocol/protocol_rpc.h>

namespace sup
{
namespace epics
{
namespace utils
{

sup::dto::AnyValue ClientRPCCall(std::shared_ptr<pvxs::client::Context> context,
                                 const PvAccessRPCClientConfig& config,
                                 const sup::dto::AnyValue& request)
{
  pvxs::Value pvxs_request;
  try
  {
    pvxs_request = BuildPVXSValue(request);
  }
  catch(...)
  {
    return sup::protocol::utils::CreateRPCReply(sup::protocol::ClientNetworkEncodingError);
  }
  pvxs::Value pvxs_reply;
  try
  {
    // Create synchronous request with timeout
    pvxs_reply = context->rpc(config.service_name, pvxs_request).exec()->wait(config.timeout);
  }
  catch (const pvxs::client::Timeout&)
  {
    return sup::protocol::utils::CreateRPCReply(sup::protocol::NotConnected);
  }
  if (!pvxs_reply)
  {
    return sup::protocol::utils::CreateRPCReply(sup::protocol::ClientNetworkDecodingError);
  }
  sup::dto::AnyValue reply;
  try
  {
    reply = BuildAnyValue(pvxs_reply);
  }
  catch(...)
  {
    return sup::protocol::utils::CreateRPCReply(sup::protocol::ClientNetworkDecodingError);
  }
  return reply;
}

pvxs::Value HandleRPCCall(sup::dto::AnyFunctor& handler, const pvxs::Value& pvxs_request)
{
  sup::dto::AnyValue request;
  try
  {
    request = BuildAnyValue(pvxs_request);
  }
  catch(...)
  {
    return BuildPVXSValue(sup::protocol::utils::CreateRPCReply(sup::protocol::ServerNetworkDecodingError));
  }
  pvxs::Value pvxs_reply;
  try
  {
    auto reply = handler(request);
    pvxs_reply = BuildPVXSValue(reply);
  }
  catch(...)
  {
    return BuildPVXSValue(sup::protocol::utils::CreateRPCReply(sup::protocol::ServerNetworkEncodingError));
  }
  if (!pvxs_reply)
  {
    return BuildPVXSValue(sup::protocol::utils::CreateRPCReply(sup::protocol::ServerNetworkEncodingError));
  }
  return pvxs_reply;
}

}  // namespace utils

}  // namespace epics

}  // namespace sup
