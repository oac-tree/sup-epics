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

#include <functional>
#include <iostream>

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
  parser.AddOption({"-f", "--file"}, "JSON file containing the fixed AnyValue to send as input")
      .SetParameter(true)
      .SetValueName("filename")
      .SetRequired(true);
  parser.AddOption({"-e", "--encoding"}, "Optional encoding to use (None/Base64)")
      .SetParameter(true)
      .SetValueName("encoding")
      .SetDefaultValue("Base64");
  parser.AddOption({"-p", "--polling-interval"}, "Polling interval in seconds (implies asynchronous communication)")
      .SetParameter(true)
      .SetValueName("interval");
  parser.AddOption({"-t", "--timeout"}, "Optional timeout in seconds for the RPC call")
      .SetParameter(true)
      .SetValueName("sec")
      .SetDefaultValue("5.0");
  parser.AddOption({"--service-packet"}, "Send a protocol service packet instead of a standard one");

  if (!parser.Parse(argc, argv))
  {
    std::cout << parser.GetUsageString();
    return 0;
  }
  auto input = utils::GetFromJSONFile(parser);

  auto client_config = utils::GetRPCClientConfiguration(parser);
  auto rpc_logger = std::bind(utils::LogNetworkPacketsToStdOut, _1, _2, utils::kClientInputPacketTitle,
                              utils::kClientOutputPacketTitle);
  auto client = CreateLoggingEPICSRPCClient(client_config, rpc_logger);

  auto protocol_client_config = utils::GetProtocolRPCClientConfiguration(parser);
  sup::protocol::ProtocolRPCClient protocol_client{*client, protocol_client_config};
  auto input_protocol_logger = std::bind(utils::LogInputProtocolPacketToStdOut, _1, _2,
                                         utils::kClientProtocolInputNormalTitle,
                                         utils::kClientProtocolInputServiceTitle);
  auto output_protocol_logger = std::bind(utils::LogOutputProtocolPacketToStdOut, _1, _2, _3,
                                          utils::kClientProtocolOutputNormalTitle,
                                          utils::kClientProtocolOutputServiceTitle);
  sup::protocol::LogProtocolDecorator protocol_decorator{
    protocol_client, input_protocol_logger, output_protocol_logger};

  sup::dto::AnyValue output{};
  if (parser.IsSet("--service-packet"))
  {
    auto response = protocol_decorator.Service(input, output);
  }
  else
  {
    auto response = protocol_decorator.Invoke(input, output);
  }
  return 0;
}
