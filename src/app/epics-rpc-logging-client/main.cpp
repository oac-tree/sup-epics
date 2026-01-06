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
 * Copyright (c) : 2010-2026 ITER Organization,
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

#include <iostream>
#include <functional>

using namespace sup::epics;

int main(int argc, char* argv[])
{
  using namespace std::placeholders;
  sup::cli::CommandLineParser parser;
  parser.SetDescription(
      /*header*/ "",
      "The program creates an EPICS RPC client that will send a provided AnyValue and logs\n"
      "all incoming/outgoing network packets to standard output.");
  parser.AddHelpOption();

  parser.AddOption({"-s", "--service"}, "Name of the RPC server")
      .SetParameter(true)
      .SetValueName("service_name")
      .SetRequired(true);
  parser.AddOption({"-f", "--file"}, "JSON file containing the fixed AnyValue to send as a request")
      .SetParameter(true)
      .SetValueName("filename")
      .SetRequired(true);
  parser.AddOption({"-t", "--timeout"}, "Optional timeout in seconds for the RPC call")
      .SetParameter(true)
      .SetValueName("sec")
      .SetDefaultValue("5.0");

  if (!parser.Parse(argc, argv))
  {
    std::cout << parser.GetUsageString();
    return 0;
  }
  auto request = utils::GetFromJSONFile(parser);
  auto client_config = utils::GetRPCClientConfiguration(parser);
  auto logger = std::bind(utils::LogNetworkPacketsToStdOut, _1, _2, utils::kClientInputPacketTitle,
                         utils::kClientOutputPacketTitle);
  auto client = CreateLoggingEPICSRPCClient(client_config, logger);
  auto response = (*client)(request);
  return 0;
}
