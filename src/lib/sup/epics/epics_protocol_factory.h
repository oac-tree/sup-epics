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

#include <sup/epics/pv_access_rpc_client_config.h>
#include <sup/epics/pv_access_rpc_server_config.h>

#include <sup/protocol/protocol_factory.h>
#include <sup/protocol/protocol_factory_utils.h>

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

  /**
   * @brief Create EPICS RPC ProcessVariable.
   *
   * @param var_definition Configuration of the variable. This AnyValue structure contains the
   * following field to indicate the type of ProcessVariable to create:
   *   - Class: mandatory string providing the type of ProcessVariable. Supported types are:
   *            'ChannelAccessClient', 'PvAccessClient' and 'PvAccessServer'.
   * All class types require the following field:
   *   - ChannelName: mandatory string providing the channel name of the network variable.
   * Depending on the class type, extra fields can be defined:
   *    - For 'ChannelAccessClient':
   *      - VarType: mandatory string providing the JSON representation of its AnyType.
   *    - For 'PvAccessClient': none.
   *    - For 'PvAccessServer':
   *      - VarValue: mandatory AnyValue providing the initial value of the network variable.
   *
   * @return EPICS ProcessVariable.
   */
  std::unique_ptr<sup::protocol::ProcessVariable> CreateProcessVariable(
    const sup::dto::AnyValue& var_definition) const override;

  /**
   * @brief Create EPICS RPC server stack with the injected protocol.
   *
   * @param protocol Protocol to inject.
   * @param server_definition Configuration for the server. This is an AnyValue structure with
   * the following field:
   *   - ServiceName: mandatory string providing the service name on the network.
   *
   * @return EPICS RPC server stack.
   */
  std::unique_ptr<sup::protocol::RPCServerInterface> CreateRPCServer(
    sup::protocol::Protocol& protocol,
    const sup::dto::AnyValue& server_definition) const override;

  /**
   * @brief Create EPICS RPC client stack with a Procotol interface.
   *
   * @param client_definition Configuration for the client. This is an AnyValue structure with
   * the following fields:
   *   - ServiceName: mandatory string providing the service name on the network to connect to,
   *   - Timeout: optional float64 field, providing the maximum timeout in seconds for requests
   *              to the server. Default is 5 seconds.
   *   - Encoding: optional string field providing the encoding used for the encapsulated
   *               ProtocolRPCClient. Supported encodings are: 'None' and 'Base64'. Default is
   *               'Base64'.
   *
   * @return EPICS RPC client stack.
   */
  std::unique_ptr<sup::protocol::Protocol> CreateRPCClient(
    const sup::dto::AnyValue& client_definition) const override;
};

/**
 * @brief Helper function to create an EPICS ChannelAccess ProcessVariable.
 *
 * @param channel Channel name.
 * @param var_type Variable AnyType.
 * @return EPICS ProcessVariable.
 */
std::unique_ptr<sup::protocol::ProcessVariable> CreateCAClientProcessVariable(
  const std::string& channel, const sup::dto::AnyType& var_type);

/**
 * @brief Helper function to create an EPICS PvAccess client ProcessVariable.
 *
 * @param channel Channel name.
 * @return EPICS ProcessVariable.
 */
std::unique_ptr<sup::protocol::ProcessVariable> CreatePVAClientProcessVariable(
  const std::string& channel);

/**
 * @brief Helper function to create an EPICS PvAccess server ProcessVariable.
 *
 * @param channel Channel name.
 * @param value Initial value.
 * @return EPICS ProcessVariable.
 */
std::unique_ptr<sup::protocol::ProcessVariable> CreatePVAServerProcessVariable(
  const std::string& channel, const sup::dto::AnyValue& value);

/**
 * @brief Helper function to create an EPICS RPC server stack with an injected Protocol.
 *
 * @param protocol Protocol to be injected.
 * @param server_config Server configuration.
 * @return EPICS RPC server stack.
 */
std::unique_ptr<sup::protocol::RPCServerInterface> CreateEPICSRPCServerStack(
  sup::protocol::Protocol& protocol, const PvAccessRPCServerConfig& server_config);

/**
 * @brief Helper function to create an EPICS RPC client stack.
 *
 * @param client_config Client configuration.
 * @param encoding Optional encoding to be used in the RPC communication.
 * @return EPICS RPC client stack.
 */
std::unique_ptr<sup::protocol::Protocol> CreateEPICSRPCClientStack(
  const PvAccessRPCClientConfig& client_config,
  sup::protocol::PayloadEncoding encoding = sup::protocol::PayloadEncoding::kBase64);

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_EPICS_PROTOCOL_FACTORY_H_
