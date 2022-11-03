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

#include <sup/epics/pv_access_rpc_server.h>

#include <sup/epics/rpc/pv_access_rpc_server_impl.h>

/** When the server receives a request, the following steps are performed:
  * - Translate the request to AnyValue
  * - Extract 'query' field
  *   - if this fails, return specific status code and reason
  * - Call embedded protocol(query, output) -> result
  *   - If result is not success, set 'reason' to something like 'protocol-specific issue'
  * - If needed, translate output to pvxs
  *   - If failure: return status code
  * - return reply = { result, timestamp, reason, output } (output is omitted if empty)
  *
  * This implies the server requires the following failure statuses/messages:
  * - Extraction of 'query' field failed: BOTH status and reason
  * - Protocol did not return success: generic reason message
  * - Translation of output value failed: status code
  *
  * A request structure without a 'query' field could be used to echo back a timestamp
  */

namespace sup
{
namespace epics
{

PvAccessRPCServer::IsolatedTag PvAccessRPCServer::Isolated{};

PvAccessRPCServer::PvAccessRPCServer(const PvAccessRPCServerConfig& config,
                                     std::unique_ptr<sup::dto::AnyFunctor>&& handler)
  : m_impl{CreateRPCServerImplFromEnv(config, std::move(handler))}
{}

PvAccessRPCServer::PvAccessRPCServer(IsolatedTag, const PvAccessRPCServerConfig& config,
                                     std::unique_ptr<sup::dto::AnyFunctor>&& handler)
  : m_impl{CreateIsolatedRPCServerImpl(config, std::move(handler))}
{}

PvAccessRPCServer::~PvAccessRPCServer() = default;

PvAccessRPCServerConfig GetDefaultRPCServerConfig(const std::string& service_name)
{
  return { service_name };
}

}  // namespace epics

}  // namespace sup
