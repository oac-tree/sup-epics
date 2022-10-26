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
    // TODO: investigate correct status for this case
    return rpc::InvalidRequest;
  }
  // TODO:
  // * translate request to pvxs and handle failure
  // * send request over rpc
  // * check validity of reply
  // * translate/extract reply
  (void)output;
  return rpc::Success;
}

}  // namespace epics

}  // namespace sup
