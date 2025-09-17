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

#ifndef SUP_EPICS_EPICS_PROTOCOL_FACTORY_UTILS_H_
#define SUP_EPICS_EPICS_PROTOCOL_FACTORY_UTILS_H_

#include <sup/epics/pv_access_rpc_client_config.h>
#include <sup/epics/pv_access_rpc_server_config.h>

#include <sup/protocol/log_any_functor_decorator.h>
#include <sup/protocol/protocol_factory.h>
#include <sup/protocol/protocol_rpc.h>
#include <sup/dto/any_functor.h>

namespace sup
{
namespace epics
{
namespace utils
{
PvAccessRPCServerConfig ParsePvAccessRPCServerConfig(const sup::dto::AnyValue& config);

PvAccessRPCClientConfig ParsePvAccessRPCClientConfig(const sup::dto::AnyValue& config);

std::unique_ptr<sup::protocol::ProcessVariable> CreateChannelAccessClientVar(
  const sup::dto::AnyValue& config);

std::unique_ptr<sup::protocol::ProcessVariable> CreatePvAccessClientVar(
  const sup::dto::AnyValue& config);

std::unique_ptr<sup::protocol::ProcessVariable> CreatePvAccessServerVar(
  const sup::dto::AnyValue& config);

class LoggingEPICSRPCClient : public sup::dto::AnyFunctor
{
public:
  LoggingEPICSRPCClient(const PvAccessRPCClientConfig& config,
                        sup::protocol::LogAnyFunctorDecorator::LogFunction log_function);

  sup::dto::AnyValue operator()(const sup::dto::AnyValue& input) override;

private:
  std::unique_ptr<sup::dto::AnyFunctor> m_epics_client;
  sup::protocol::LogAnyFunctorDecorator m_log_decorator;
};

}  // namespace utils

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_EPICS_PROTOCOL_FACTORY_UTILS_H_
