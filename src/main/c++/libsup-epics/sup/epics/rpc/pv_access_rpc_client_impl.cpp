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

#include "pv_access_rpc_client_impl.h"

#include <sup/epics/pvxs/pv_access_utils.h>
#include <sup/epics/rpc/pv_access_rpc_utils.h>
#include <sup/epics/utils/dto_conversion_utils.h>

#include <sup/dto/anyvalue_helper.h>

namespace sup
{
namespace epics
{

PvAccessRPCClientImpl::PvAccessRPCClientImpl(const PvAccessRPCClientConfig& config)
  : m_config{config}
  , m_context{utils::GetSharedClientContext()}
{}

PvAccessRPCClientImpl::~PvAccessRPCClientImpl() = default;

rpc::ProtocolResult PvAccessRPCClientImpl::Invoke(const sup::dto::AnyValue& input,
                                                  sup::dto::AnyValue& output)
{
  auto request = utils::CreateRPCRequest(input);
  if (sup::dto::IsEmptyValue(request))
  {
    return rpc::InvalidRequest;
  }
  auto reply = utils::ClientRPCCall(m_context, m_config, request);
  if (reply.HasField(utils::constants::REPLY_PAYLOAD))
  {
    if (!sup::dto::TryConvert(output, reply[utils::constants::REPLY_PAYLOAD]))
    {
      return rpc::ProtocolError;  //TODO: use correct status
    }
  }
  auto status_code = reply[utils::constants::REPLY_RESULT].As<sup::dto::uint32>();
  return rpc::ProtocolResult(status_code);
}

}  // namespace epics

}  // namespace sup
