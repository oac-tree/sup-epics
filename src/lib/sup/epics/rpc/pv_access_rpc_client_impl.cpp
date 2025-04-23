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

#include "pv_access_rpc_client_impl.h"

#include <sup/epics/pvxs/pv_access_utils.h>
#include <sup/epics/rpc/pv_access_rpc_utils.h>

#include <sup/dto/anyvalue_helper.h>

namespace sup
{
namespace epics
{

PvAccessRPCClientImpl::PvAccessRPCClientImpl(const PvAccessRPCClientConfig& config,
                                             std::shared_ptr<pvxs::client::Context> context)
  : m_config{config}
  , m_context{context}
{}

PvAccessRPCClientImpl::~PvAccessRPCClientImpl() = default;

sup::dto::AnyValue PvAccessRPCClientImpl::operator()(const sup::dto::AnyValue& request)
{
  if (sup::dto::IsEmptyValue(request))
  {
    return {};
  }
  return utils::ClientRPCCall(m_context, m_config, request);
}

}  // namespace epics

}  // namespace sup
