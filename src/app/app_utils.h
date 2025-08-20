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

#ifndef SUP_EPICS_APP_UTILS_H_
#define SUP_EPICS_APP_UTILS_H_

#include <sup/epics/pv_access_rpc_client_config.h>

#include <sup/cli/command_line_parser.h>
#include <sup/protocol/log_any_functor_decorator.h>
#include <sup/protocol/log_protocol_decorator.h>
#include <sup/protocol/protocol_rpc_client_config.h>

namespace sup
{
namespace epics
{
namespace utils
{

sup::dto::AnyValue GetFromJSONFile(sup::cli::CommandLineParser& parser);

PvAccessRPCClientConfig GetRPCClientConfiguration(sup::cli::CommandLineParser& parser);

sup::protocol::ProtocolRPCClientConfig GetProtocolRPCClientConfiguration(
  sup::cli::CommandLineParser& parser);

std::unique_ptr<sup::dto::AnyFunctor> GetFixedReplyFunctor(sup::cli::CommandLineParser& parser);

std::unique_ptr<sup::protocol::Protocol> GetFixedOutputProtocol(
  sup::cli::CommandLineParser& parser);

void LogNetworkPacketsToStdOut(const sup::dto::AnyValue& packet,
                               sup::protocol::LogAnyFunctorDecorator::PacketDirection direction);

void LogInputProtocolPacketToStdOut(const sup::dto::AnyValue& packet,
                                    sup::protocol::LogProtocolDecorator::PacketType type);

void LogOutputProtocolPacketToStdOut(sup::protocol::ProtocolResult result,
                                     const sup::dto::AnyValue& packet,
                                     sup::protocol::LogProtocolDecorator::PacketType type);

}  // namespace utils

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_APP_UTILS_H_
