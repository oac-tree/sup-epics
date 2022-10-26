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

#include <sup/rpc/protocol_result.h>

static const double DEFAULT_TIMEOUT_SECONDS = 5.0;

namespace sup
{
namespace epics
{

PvAccessRPCClient::PvAccessRPCClient(const PvAccessRPCClientConfig& config)
  : m_config{config}
{}

PvAccessRPCClient::~PvAccessRPCClient() = default;

rpc::ProtocolResult PvAccessRPCClient::Invoke(const sup::dto::AnyValue& input,
                                              sup::dto::AnyValue& output)
{
  (void)input;
  (void)output;
  return rpc::Success;
}

PvAccessRPCClientConfig GetDefaultRPCClientConfig(const std::string& service_name)
{
  return { service_name, DEFAULT_TIMEOUT_SECONDS };
}

}  // namespace epics

}  // namespace sup
