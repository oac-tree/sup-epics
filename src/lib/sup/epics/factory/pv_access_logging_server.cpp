/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2026 ITER Organization,
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

#include "pv_access_logging_server.h"

namespace sup
{
namespace epics
{

PVAccessLoggingServer::PVAccessLoggingServer(
  const PvAccessRPCServerConfig& server_config, sup::dto::AnyFunctor& functor,
  sup::protocol::LogAnyFunctorDecorator::LogFunction log_function)
  : m_log_decorator{functor, log_function}
  , m_rpc_server{server_config, m_log_decorator}
{}

PVAccessLoggingServer::~PVAccessLoggingServer() = default;

}  // namespace epics

}  // namespace sup
