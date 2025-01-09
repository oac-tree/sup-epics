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
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 *****************************************************************************/

#include <sup/epics/pv_access_rpc_client.h>

#include <sup/epics/rpc/pv_access_rpc_client_impl.h>
#include <sup/epics/pvxs/pv_access_utils.h>

#include <sup/dto/anyvalue.h>
#include <sup/protocol/protocol_result.h>

static const double DEFAULT_TIMEOUT_SECONDS = 5.0;

namespace sup
{
namespace epics
{

PvAccessRPCClient::PvAccessRPCClient(const PvAccessRPCClientConfig& config)
  : m_impl{std::make_unique<PvAccessRPCClientImpl>(config, utils::GetSharedClientContext())}
{}

PvAccessRPCClient::PvAccessRPCClient(std::unique_ptr<PvAccessRPCClientImpl>&& impl)
  : m_impl{std::move(impl)}
{}

PvAccessRPCClient::~PvAccessRPCClient() = default;

PvAccessRPCClient::PvAccessRPCClient(PvAccessRPCClient&& other)
  : m_impl{std::move(other.m_impl)}
{}

PvAccessRPCClient& PvAccessRPCClient::operator=(PvAccessRPCClient&& other)
{
  if (this != &other)
  {
    std::swap(m_impl, other.m_impl);
  }
  return *this;
}

sup::dto::AnyValue PvAccessRPCClient::operator()(const sup::dto::AnyValue& request)
{
  return m_impl->operator()(request);
}

PvAccessRPCClientConfig GetDefaultRPCClientConfig(const std::string& service_name)
{
  return { service_name, DEFAULT_TIMEOUT_SECONDS };
}

}  // namespace epics

}  // namespace sup
