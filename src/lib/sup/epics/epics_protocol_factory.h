/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2024 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 *****************************************************************************/

#ifndef SUP_EPICS_EPICS_PROTOCOL_FACTORY_H_
#define SUP_EPICS_EPICS_PROTOCOL_FACTORY_H_

#include <sup/protocol/protocol_factory.h>

namespace sup
{
namespace epics
{
// Constants for RPC clients/servers:
const std::string kServiceName = "ServiceName";
const std::string kTimeout = "Timeout";

// Constants for ProcessVariables:
// Class of ProcessVariable
const std::string kProcessVariableClass = "Class";
const std::string kChannelAccessClientClass = "ChannelAccessClient";
const std::string kPvAccessClientClass = "PvAccessClient";
const std::string kPvAccessServerClass = "PvAccessServer";
// Other fields for the supported ProcessVariables
const std::string kChannelName = "ChannelName";
const std::string kVariableType = "VarType";
const std::string kVariableValue = "VarValue";

class EPICSProtocolFactory : public sup::protocol::ProtocolFactory
{
public:
  EPICSProtocolFactory();
  ~EPICSProtocolFactory();

  std::unique_ptr<sup::protocol::ProcessVariable> CreateProcessVariable(
    const sup::dto::AnyValue& var_definition) const override;

  std::unique_ptr<sup::protocol::RPCServerInterface> CreateRPCServer(
    sup::protocol::Protocol& protocol,
    const sup::dto::AnyValue& server_definition) const override;

  std::unique_ptr<sup::protocol::Protocol> CreateRPCClient(
    const sup::dto::AnyValue& client_definition) const override;
};

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_EPICS_PROTOCOL_FACTORY_H_
