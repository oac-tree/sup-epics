/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : Supervision and Automation - Configuration
 *
 * Description   : Configuration and CVVF libraries for the Supervision and Automation System.
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
 ******************************************************************************/

#ifndef SUP_EPICS_EPICS_RPC_LOGGING_SERVER_UTILS_H_
#define SUP_EPICS_EPICS_RPC_LOGGING_SERVER_UTILS_H_

#include <sup/cli/command_line_parser.h>
#include <sup/protocol/log_any_functor_decorator.h>

#include <string>

namespace sup
{
namespace epics
{
namespace utils
{

std::unique_ptr<sup::dto::AnyFunctor> GetFixedReplyFunctor(sup::cli::CommandLineParser& parser);

void LogNetworkPacketsToStdOut(const sup::dto::AnyValue& packet,
                               sup::protocol::LogAnyFunctorDecorator::PacketDirection direction);
}  // namespace utils

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_EPICS_RPC_LOGGING_SERVER_UTILS_H_
