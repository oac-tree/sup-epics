/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2022 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 *****************************************************************************/

#ifndef SUP_EPICS_CHANNEL_ACCESS_CLIENT_H_
#define SUP_EPICS_CHANNEL_ACCESS_CLIENT_H_

#include <sup/epics/channel_access_pv.h>

#include <map>
#include <memory>
#include <vector>

namespace sup
{
namespace epics
{
/**
 * @brief ChannelAccessClient manages a set of ChannelAccessPVs.
 */
class ChannelAccessClient
{
public:
  using VariableUpdatedCallback =
    std::function<void(const std::string&,const ChannelAccessPV::ExtendedValue&)>;
  /**
   * @brief Constructor.
   */
  ChannelAccessClient(VariableUpdatedCallback cb = {});

    /**
   * @brief Destructor.
   *
   * @details Destroys all owned channels.
   */
  ~ChannelAccessClient();

    /**
   * @brief Deleted copy/move constructor/assigment.
   */
  ChannelAccessClient(const ChannelAccessClient& other) = delete;
  ChannelAccessClient(ChannelAccessClient&& other) = delete;
  ChannelAccessClient& operator=(const ChannelAccessClient& other) = delete;
  ChannelAccessClient& operator=(ChannelAccessClient&& other) = delete;

    /**
   * @brief Add a new variable with the given channel and type.
   *
   * @param channel EPICS channel name.
   * @param type Type to use for this variable.
   *
   * @return True if variable was successfully constructed, false otherwise.
   */
  bool AddVariable(const std::string& channel, const sup::dto::AnyType& type);

    /**
   * @brief Retrieve the names of all managed channels.
   *
   * @return List of all channel names.
   */
  std::vector<std::string> GetVariableNames() const;

    /**
   * @brief Check if specific channel is connected.
   *
   * @param channel EPICS channel name.
   *
   * @return True if channel is connected, false otherwise.
   */
  bool IsConnected(const std::string& channel) const;

    /**
   * @brief Retrieve the value from a specific channel.
   *
   * @param channel EPICS channel name.
   *
   * @return Channel's value if connected, empty value otherwise.
   */
  sup::dto::AnyValue GetValue(const std::string& channel) const;

    /**
   * @brief Retrieve extended information from a specific channel.
   *
   * @param channel EPICS channel name.
   *
   * @return Structure with value and different status fields (e.g. connected, status, etc.)
   */
  ChannelAccessPV::ExtendedValue GetExtendedValue(const std::string& channel) const;

    /**
   * @brief Propagate the value to a specific channel.
   *
   * @param channel EPICS channel name.
   * @param value Value to be written to the channel.
   *
   * @return True if successful, false otherwise.
   */
  bool SetValue(const std::string& channel, const sup::dto::AnyValue& value);

  /**
   * @brief This method waits for a specific channel to be connected with a timeout.
   *
   * @param channel EPICS channel name.
   * @param timeout_sec Timeout in seconds to wait for the channel to be connected.
   * @return True if the channel was connected within the timeout period.
   */
  bool WaitForConnected(const std::string& channel, double timeout_sec) const;

  /**
   * @brief Remove the variable with the given name.
   *
   * @param channel EPICS channel name.
   *
   * @return True if the variable was successfully removed.
   */
  bool RemoveVariable(const std::string& channel);

private:
  void OnVariableUpdated(const std::string& channel, const ChannelAccessPV::ExtendedValue& value);
  std::map<std::string, std::unique_ptr<ChannelAccessPV>> pv_map;
  VariableUpdatedCallback var_updated_cb;
};
}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_CHANNEL_ACCESS_CLIENT_H_
