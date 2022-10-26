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

#include <sup/epics/pv_access_rpc_client.h>

#include <sup/epics/rpc/pv_access_rpc_client_impl.h>

#include <sup/dto/anyvalue.h>
#include <sup/rpc/protocol_result.h>

/** When the client's Invoke is called, the following steps are performed:
  * - Create a request with a timestamp and the provided payload
  * - Translate this to pvxs
  *   - If this fails return a specific status code
  * - Send over the network
  *   - No response: status code
  * - Translate response to AnyValue
  * - Parse the reply
  *   - Incorrect format: return specific error code
  * - Return status code and output from the reply
  *
  * This implies the client requires the following failure statuses/messages:
  * - Translation of request structure failed: status code
  * - No response from server: status code
  * - Incorrect response format from server: status code
  *
  * For echo testing, a request without a 'query' field can be used. The reply should then only
  * be checked for its 'result', 'timestamp' and 'reason' fields.
  */

static const double DEFAULT_TIMEOUT_SECONDS = 5.0;

namespace sup
{
namespace epics
{

PvAccessRPCClient::PvAccessRPCClient(const PvAccessRPCClientConfig& config)
  : m_impl{new PvAccessRPCClientImpl(config)}
{}

PvAccessRPCClient::~PvAccessRPCClient() = default;

rpc::ProtocolResult PvAccessRPCClient::Invoke(const sup::dto::AnyValue& input,
                                              sup::dto::AnyValue& output)
{
  return m_impl->Invoke(input, output);
}

PvAccessRPCClientConfig GetDefaultRPCClientConfig(const std::string& service_name)
{
  return { service_name, DEFAULT_TIMEOUT_SECONDS };
}

}  // namespace epics

}  // namespace sup
