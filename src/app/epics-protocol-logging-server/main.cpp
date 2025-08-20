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
#include <sup/protocol/log_protocol_decorator.h>
#include <sup/protocol/protocol_rpc_server.h>

#include <sup/cli/command_line_parser.h>

#include <chrono>
#include <iostream>
#include <thread>

using namespace sup::epics;

int main(int argc, char* argv[])
{
  sup::cli::CommandLineParser parser;
  parser.SetDescription(
      /*header*/ "",
      "The program launches an EPICS RPC protocol server that will reply with a fixed AnyValue\n"
      "and logs incoming/outgoing protocol and network packets to standard output.");
  parser.AddHelpOption();

  parser.AddOption({"-s", "--service"}, "Name of the RPC server")
      .SetParameter(true)
      .SetValueName("service_name")
      .SetRequired(true);
  parser.AddOption({"-f", "--file"}, "JSON file containing the fixed AnyValue to return")
      .SetParameter(true)
      .SetValueName("filename")
      .SetRequired(true);
  parser.AddOption({"-r", "--result"},
                    "Optional ProtocolResult code to return (default is 0 for success)")
      .SetParameter(true)
      .SetValueName("result")
      .SetDefaultValue("0");
  parser.AddOption({"-d", "--delay"}, "Optional delay in seconds to respond")
      .SetParameter(true)
      .SetValueName("sec")
      .SetDefaultValue("0.0");

  if (!parser.Parse(argc, argv))
  {
    std::cout << parser.GetUsageString();
    return 0;
  }
  auto fixed_output_protocol = utils::GetFixedOutputProtocol(parser);

  sup::protocol::LogProtocolDecorator protocol_decorator{
    *fixed_output_protocol, utils::LogInputProtocolPacketToStdOut,
    utils::LogOutputProtocolPacketToStdOut};
  sup::protocol::ProtocolRPCServer protocol_server{protocol_decorator};

  auto service_name = parser.GetValue<std::string>("--service");
  PvAccessRPCServerConfig server_config{service_name};
  auto server = CreateLoggingEPICSRPCServer(server_config, protocol_server,
                                            utils::LogNetworkPacketsToStdOut);
  while (true)
  {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  return 0;
}
