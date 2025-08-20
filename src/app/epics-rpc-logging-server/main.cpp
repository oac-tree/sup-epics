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

#include "app_utils.h"

#include <sup/epics/epics_protocol_factory.h>

#include <sup/cli/command_line_parser.h>

#include <chrono>
#include <functional>
#include <iostream>
#include <thread>

using namespace sup::epics;

int main(int argc, char* argv[])
{
  using namespace std::placeholders;
  sup::cli::CommandLineParser parser;
  parser.SetDescription(
      /*header*/ "",
      "The program launches an EPICS RPC server that will reply with a fixed AnyValue and logs\n"
      "all incoming/outgoing network packets to standard output.");
  parser.AddHelpOption();

  parser.AddOption({"-s", "--service"}, "Name of the RPC server")
      .SetParameter(true)
      .SetValueName("service_name")
      .SetRequired(true);
  parser.AddOption({"-f", "--file"}, "JSON file containing the fixed AnyValue to return")
      .SetParameter(true)
      .SetValueName("filename")
      .SetRequired(true);
  parser.AddOption({"-d", "--delay"}, "Optional delay in seconds to respond")
      .SetParameter(true)
      .SetValueName("sec")
      .SetDefaultValue("0.0");

  if (!parser.Parse(argc, argv))
  {
    std::cout << parser.GetUsageString();
    return 0;
  }
  auto fixed_reply_functor = utils::GetFixedReplyFunctor(parser);

  auto service_name = parser.GetValue<std::string>("--service");
  PvAccessRPCServerConfig server_config{service_name};
  auto logger = std::bind(utils::LogNetworkPacketsToStdOut, _1, _2, utils::kServerInputPacketTitle,
                         utils::kServerOutputPacketTitle);
  auto server = CreateLoggingEPICSRPCServer(server_config, *fixed_reply_functor, logger);
  while (true)
  {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  return 0;
}
