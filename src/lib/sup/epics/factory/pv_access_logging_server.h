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

#ifndef SUP_EPICS_PV_ACCESS_LOGGING_SERVER_H_
#define SUP_EPICS_PV_ACCESS_LOGGING_SERVER_H_

#include <sup/epics/pv_access_rpc_server_config.h>
#include <sup/epics/pv_access_rpc_server.h>

#include <sup/protocol/log_any_functor_decorator.h>
#include <sup/protocol/protocol_factory.h>

namespace sup
{
namespace epics
{

class PVAccessLoggingServer : public sup::protocol::RPCServerInterface
{
public:
  PVAccessLoggingServer(const PvAccessRPCServerConfig& server_config, sup::dto::AnyFunctor& functor,
                        sup::protocol::LogAnyFunctorDecorator::LogFunction log_function);
  ~PVAccessLoggingServer() override;

private:
  sup::protocol::LogAnyFunctorDecorator m_log_decorator;
  PvAccessRPCServer m_rpc_server;
};

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_PV_ACCESS_LOGGING_SERVER_H_
