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

#include "pv_access_rpc_utils.h"

#include <sup/epics/utils/dto_conversion_utils.h>

#include <sup/rpc/protocol_rpc.h>

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
  auto pvxs_request = BuildPVXSValue(request);
  sup::dto::AnyValue reply;
  try
  {
    // Create synchronous request with timeout
    auto result =
        context->rpc(config.service_name, pvxs_request).exec()->wait(config.timeout);
    if (result)
    {
      // Extract reply
      reply = BuildAnyValue(result);
    }
  }
  catch (const pvxs::client::Timeout&)
  {
    return sup::rpc::utils::CreateRPCReply(sup::rpc::NotConnected);
  }
  return reply;
}

}  // namespace utils

}  // namespace epics

}  // namespace sup
