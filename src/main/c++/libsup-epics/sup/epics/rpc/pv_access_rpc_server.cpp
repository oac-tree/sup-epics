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

namespace sup
{
namespace epics
{

PvAccessRPCServer::PvAccessRPCServer(const PvAccessRPCServerConfig& config,
                                     std::unique_ptr<rpc::Protocol>&& protocol)
  : m_config{config}
  , m_protocol{std::move(protocol)}
{}

PvAccessRPCServer::~PvAccessRPCServer() = default;

PvAccessRPCServerConfig GetDefaultRPCServerConfig(const std::string& service_name)
{
  return { service_name };
}

}  // namespace epics

}  // namespace sup
