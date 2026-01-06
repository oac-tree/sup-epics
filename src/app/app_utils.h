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
const std::string kServerInputPacketTitle = "Server received network packet";
const std::string kServerOutputPacketTitle = "Server replied with network packet";

const std::string kClientInputPacketTitle = "Client sent network packet";
const std::string kClientOutputPacketTitle = "Client received network packet";

const std::string kServerProtocolInputNormalTitle = "Server received standard protocol packet";
const std::string kServerProtocolInputServiceTitle = "Server received service protocol packet";

const std::string kServerProtocolOutputNormalTitle = "Server replied with standard protocol packet";
const std::string kServerProtocolOutputServiceTitle = "Server replied with service protocol packet";

const std::string kClientProtocolInputNormalTitle = "Client sent standard protocol packet";
const std::string kClientProtocolInputServiceTitle = "Client sent service protocol packet";

const std::string kClientProtocolOutputNormalTitle = "Client received standard protocol packet";
const std::string kClientProtocolOutputServiceTitle = "Client received service protocol packet";


sup::dto::AnyValue GetFromJSONFile(sup::cli::CommandLineParser& parser);

PvAccessRPCClientConfig GetRPCClientConfiguration(sup::cli::CommandLineParser& parser);

sup::protocol::ProtocolRPCClientConfig GetProtocolRPCClientConfiguration(
  sup::cli::CommandLineParser& parser);

std::unique_ptr<sup::dto::AnyFunctor> GetFixedReplyFunctor(sup::cli::CommandLineParser& parser);

std::unique_ptr<sup::protocol::Protocol> GetFixedOutputProtocol(
  sup::cli::CommandLineParser& parser);

void LogNetworkPacketsToStdOut(const sup::dto::AnyValue& packet,
                               sup::protocol::LogAnyFunctorDecorator::PacketDirection direction,
                               const std::string& input_title, const std::string& output_title);

void LogInputProtocolPacketToStdOut(const sup::dto::AnyValue& packet,
                                    sup::protocol::LogProtocolDecorator::PacketType type,
                                    const std::string& normal_title,
                                    const std::string& service_title);

void LogOutputProtocolPacketToStdOut(sup::protocol::ProtocolResult result,
                                     const sup::dto::AnyValue& packet,
                                     sup::protocol::LogProtocolDecorator::PacketType type,
                                     const std::string& normal_title,
                                     const std::string& service_title);

}  // namespace utils

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_APP_UTILS_H_
