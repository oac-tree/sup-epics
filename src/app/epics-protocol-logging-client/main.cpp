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
#include <sup/protocol/protocol_rpc_client.h>

#include <iostream>

using namespace sup::epics;

int main(int argc, char* argv[])
{
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
  parser.AddOption({"-f", "--file"}, "JSON file containing the fixed AnyValue to send as input")
      .SetParameter(true)
      .SetValueName("filename")
      .SetRequired(true);
  parser.AddOption({"-e", "--encoding"}, "Optional encoding to use (None/Base64)")
      .SetParameter(true)
      .SetValueName("encoding")
      .SetDefaultValue("Base64");
  parser.AddOption({"-p", "--polling_interval"}, "Polling interval in seconds (implies asynchronous communication)")
      .SetParameter(true)
      .SetValueName("interval");
  parser.AddOption({"-t", "--timeout"}, "Optional timeout in seconds for the RPC call")
      .SetParameter(true)
      .SetValueName("sec")
      .SetDefaultValue("5.0");

  if (!parser.Parse(argc, argv))
  {
    std::cout << parser.GetUsageString();
    return 0;
  }
  auto input = utils::GetFromJSONFile(parser);

  auto client_config = utils::GetRPCClientConfiguration(parser);
  auto client = CreateLoggingEPICSRPCClient(client_config, utils::LogNetworkPacketsToStdOut);

  auto protocol_client_config = utils::GetProtocolRPCClientConfiguration(parser);
  sup::protocol::ProtocolRPCClient protocol_client{*client, protocol_client_config};
  sup::protocol::LogProtocolDecorator protocol_decorator{
    protocol_client, utils::LogInputProtocolPacketToStdOut,
    utils::LogOutputProtocolPacketToStdOut};

  sup::dto::AnyValue output{};
  auto response = protocol_decorator.Invoke(input, output);
  return 0;
}
