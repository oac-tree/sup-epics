/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Gennady Pospelov (IO)
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

#ifndef SUP_EPICS_PV_ACCESS_SERVER_H_
#define SUP_EPICS_PV_ACCESS_SERVER_H_

#include <sup/epics/pv_access_client.h>

#include <sup/dto/anyvalue.h>

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace sup
{
namespace epics
{
class PvAccessServerImpl;
//! Represents a server to run multiple pvAccess variables.

class PvAccessServer
{
public:
  using VariableChangedCallback = std::function<void(const std::string&, const sup::dto::AnyValue&)>;

  struct IsolatedTag {};
  static IsolatedTag Isolated;

  /**
   * @brief Constructor.
   *
   * @param cb Callback function to call when the variable's value changed.
   */
  explicit PvAccessServer(VariableChangedCallback cb = {});

  /**
   * @brief Constructor.
   *
   * @param isolated Tag to request an isolated context for testing.
   * @param cb Callback function to call when the variable's value changed.
   */
  explicit PvAccessServer(IsolatedTag isolated, VariableChangedCallback callback = {});

  ~PvAccessServer();

  PvAccessServer(const PvAccessServer&) = delete;
  PvAccessServer& operator=(const PvAccessServer&) = delete;
  PvAccessServer(PvAccessServer&&) = delete;
  PvAccessServer& operator=(PvAccessServer&&) = delete;

  /**
   * @brief Add variable to the server with given channel name and initial value.
   *
   * @param channel EPICS channel name.
   * @param any_value Initial value.
   *
   * @note The type of the underlying PVA record will be deduced from the AnyValue type.
   * It will throw if such a channel already exists.
   */
  void AddVariable(const std::string& channel, const sup::dto::AnyValue& any_value);

  /**
   * @brief Returns the names of all managed channels.
   *
   * @return List of all channel names.
   */
  std::vector<std::string> GetVariableNames() const;

  /**
   * @brief Get the value from a specific channel. Will throw if the channel was not added yet.
   *
   * @param channel EPICS channel name.
   *
   * @return Channel's value.
   */
  sup::dto::AnyValue GetValue(const std::string& channel) const;

  /**
   * @brief Propagate the value to a specific channel. Will throw if the channel was not added yet.
   *
   * @param channel EPICS channel name.
   * @param value Value to be written to the channel.
   *
   * @return True if successful, false otherwise.
   */
  bool SetValue(const std::string& channel, const sup::dto::AnyValue& value);

  /**
   * @brief Starts PvAccess server and publishes all added variables.
   */
  void Start();

  /**
   * @brief Create a PvAccess client based on this server's context.
   *
   * @param cb Optional callback function to add to the client.
   *
   * @return A PvAccess client with a context associated to the server.
   */
  PvAccessClient CreateClient(PvAccessClient::VariableChangedCallback cb = {});

  /**
   * @brief Create a PvAccess client PV based on this server's context.
   *
   * @param cb Optional callback function to add to the client PV.
   *
   * @return A PvAccess client PV with a context associated to the server.
   */
  PvAccessClientPV CreateClientPV(const std::string& channel,
                                  PvAccessClientPV::VariableChangedCallback cb = {});

private:
  std::unique_ptr<PvAccessServerImpl> p_impl;
};

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_PV_ACCESS_SERVER_H_
