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

#ifndef SUP_EPICS_EPICS_PROTOCOL_FACTORY_H_
#define SUP_EPICS_EPICS_PROTOCOL_FACTORY_H_

#include <sup/epics/pv_access_rpc_client_config.h>
#include <sup/epics/pv_access_rpc_server_config.h>

#include <sup/protocol/log_any_functor_decorator.h>
#include <sup/protocol/protocol_factory.h>
#include <sup/protocol/protocol_factory_utils.h>
#include <sup/protocol/protocol_rpc_server_config.h>

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

/**
 * @brief Helper class that instantiates an EPICS RPC server. To be used with
 * sup::protocol::CreateRPCServerStack.
 */
class EPICSRPCServerFactory
{
public:
  explicit EPICSRPCServerFactory(const PvAccessRPCServerConfig& config);
  ~EPICSRPCServerFactory() = default;

  EPICSRPCServerFactory(const EPICSRPCServerFactory&) = default;
  EPICSRPCServerFactory& operator=(const EPICSRPCServerFactory&) = default;
  EPICSRPCServerFactory(EPICSRPCServerFactory&&) = default;
  EPICSRPCServerFactory& operator=(EPICSRPCServerFactory&&) = default;

  std::unique_ptr<sup::protocol::RPCServerInterface> operator()(sup::dto::AnyFunctor& functor);
private:
  PvAccessRPCServerConfig m_config;
};

/**
 * @brief Helper class that instantiates an EPICS RPC client. To be used with
 * sup::protocol::CreateRPCClientStack.
 */
class EPICSRPCClientFactory
{
public:
  explicit EPICSRPCClientFactory(const PvAccessRPCClientConfig& config);
  ~EPICSRPCClientFactory() = default;

  EPICSRPCClientFactory(const EPICSRPCClientFactory&) = default;
  EPICSRPCClientFactory& operator=(const EPICSRPCClientFactory&) = default;
  EPICSRPCClientFactory(EPICSRPCClientFactory&&) = default;
  EPICSRPCClientFactory& operator=(EPICSRPCClientFactory&&) = default;

  std::unique_ptr<sup::dto::AnyFunctor> operator()();
private:
  PvAccessRPCClientConfig m_config;
};

class EPICSProtocolFactory : public sup::protocol::ProtocolFactory
{
public:
  EPICSProtocolFactory();
  ~EPICSProtocolFactory() override;

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
 * @param server_config Server configuration.
 * @param protocol_config Protocol configuration.
 * @param protocol Protocol to be injected.
 * @return EPICS RPC server stack.
 */
std::unique_ptr<sup::protocol::RPCServerInterface> CreateEPICSRPCServerStack(
  const PvAccessRPCServerConfig& server_config,
  const sup::protocol::ProtocolRPCServerConfig& protocol_config,
  std::unique_ptr<sup::protocol::Protocol> protocol);

/**
 * @brief Helper function to create an EPICS RPC server stack with injected logging functions
 * and Protocol.
 *
 * @param server_config Server configuration.
 * @param protocol_config Protocol configuration.
 * @param protocol Protocol to be injected.
 * @param log_functions Functions to use for loggin network and/or protocol packets.
 * @return EPICS RPC server stack.
 */
std::unique_ptr<sup::protocol::RPCServerInterface> CreateEPICSRPCServerStack(
  const PvAccessRPCServerConfig& server_config,
  const sup::protocol::ProtocolRPCServerConfig& protocol_config,
  std::unique_ptr<sup::protocol::Protocol> protocol,
  sup::protocol::LoggingFunctions log_functions);

/**
 * @brief Helper function to create an EPICS RPC server stack with an injected logging function
 * and AnyFunctor.
 *
 * @param server_config Server configuration.
 * @param functor AnyFunctor to be injected into the encapsulated ProtocolRPCServer.
 * @param log_function Optional function to log network packets.
 * @return EPICS RPC server stack.
 */
std::unique_ptr<sup::protocol::RPCServerInterface> CreateEPICSRPCServerStack(
  const PvAccessRPCServerConfig& server_config,
  std::unique_ptr<sup::dto::AnyFunctor> functor,
  const sup::protocol::LogAnyFunctorDecorator::LogFunction& log_function);

/**
 * @brief Helper function to create an EPICS RPC client stack.
 *
 * @param client_config Client configuration.
 * @param protocol_config Protocol configuration.
 * @return EPICS RPC client stack.
 */
std::unique_ptr<sup::protocol::Protocol> CreateEPICSRPCClientStack(
  const PvAccessRPCClientConfig& client_config,
  const sup::protocol::ProtocolRPCClientConfig& protocol_config);

/**
 * @brief Helper function to create an EPICS RPC client stack with injected logging functions.
 *
 * @param client_config Client configuration.
 * @param protocol_config Protocol configuration.
 * @param log_functions Functions to use for loggin network and/or protocol packets.
 * @return EPICS RPC client stack.
 */
std::unique_ptr<sup::protocol::Protocol> CreateEPICSRPCClientStack(
  const PvAccessRPCClientConfig& client_config,
  const sup::protocol::ProtocolRPCClientConfig& protocol_config,
  sup::protocol::LoggingFunctions log_functions);

/**
 * @brief Helper function to create an EPICS RPC client with an injected logging function.
 *
 * @param client_config Client configuration.
 * @param log_function Function to log the input and output of the AnyFunctor.
 * @return EPICS RPC client stack.
 */
std::unique_ptr<sup::dto::AnyFunctor> CreateEPICSRPCClientStack(
  const PvAccessRPCClientConfig& client_config,
  sup::protocol::LogAnyFunctorDecorator::LogFunction log_function);

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_EPICS_PROTOCOL_FACTORY_H_
