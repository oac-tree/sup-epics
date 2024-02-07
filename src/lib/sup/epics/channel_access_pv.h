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

#ifndef SUP_EPICS_CHANNEL_ACCESS_PV_H_
#define SUP_EPICS_CHANNEL_ACCESS_PV_H_

#include <sup/epics/ca_types.h>

#include <condition_variable>
#include <mutex>

namespace sup
{
namespace epics
{
class ChannelAccessPV
{
public:
  struct ExtendedValue
  {
    ExtendedValue();
    bool connected;
    sup::dto::uint64 timestamp;
    sup::dto::int16 status;
    sup::dto::int16 severity;
    sup::dto::AnyValue value;
  };
  using VariableChangedCallback = std::function<void(const ExtendedValue&)>;
  /**
   * @brief Constructor.
   *
   * @param channel EPICS channel name.
   * @param type Type to use for the connected channel.
   * @param cb Callback function to call when the variable's value or status changed.
   *
   * @details The optional callback will be called while holding an internal lock that is also
   * used for any read operation on the PV (IsConnected/GetValue/etc.) So be aware for deadlocks
   * in callbacks that will acquire another lock! While the EPICS CA library ensures that callbacks
   * will be called serially, we need to hold this lock to prevent reordering of value updates with
   * respect to their callbacks being called.
   *
   * @return True if the variable was connected within the timeout period.
   *
   * @throws std::runtime_error when the EPICS context or channel could not be created (not
   * related to the fact that the specific PV might not be connected).
   */
  ChannelAccessPV(const std::string& channel, const sup::dto::AnyType& type,
                  VariableChangedCallback cb = {});

    /**
   * @brief Destructor.
   */
  ~ChannelAccessPV();

    /**
   * @brief Deleted copy/move constructor/assigment.
   */
  ChannelAccessPV(const ChannelAccessPV& other) = delete;
  ChannelAccessPV(ChannelAccessPV&& other) = delete;
  ChannelAccessPV& operator=(const ChannelAccessPV& other) = delete;
  ChannelAccessPV& operator=(ChannelAccessPV&& other) = delete;

    /**
   * @brief Check if variable is connected.
   *
   * @return True if variable is connected, false otherwise.
   */
  bool IsConnected() const;

    /**
   * @brief Retrieve the variable's channel name.
   *
   * @return The variable's channel name.
   */
  std::string GetChannelName() const;

    /**
   * @brief Retrieve the variable's value.
   *
   * @return Variable's value if connected, empty value otherwise.
   */
  sup::dto::AnyValue GetValue() const;

    /**
   * @brief Retrieve extended information on the variable.
   *
   * @return Structure with value and different status fields (e.g. connected, status, etc.)
   */
  ExtendedValue GetExtendedValue() const;

    /**
   * @brief Propagate the value to the EPICS server.
   *
   * @param value Value to be written to the server.
   *
   * @return True if successful, false otherwise.
   */
  bool SetValue(const sup::dto::AnyValue& value);

  /**
   * @brief This method waits for the variable to be connected with a timeout.
   *
   * @param timeout_sec Timeout in seconds to wait for the variable to be connected.
   * @return True if the variable was connected within the timeout period.
   */
  bool WaitForConnected(double timeout_sec) const;

  /**
   * @brief This method waits with a timeout for the variable cache to be valid.
   *
   * @param timeout_sec Timeout in seconds to wait for the variable to be valid.
   * @return True if the variable was valid within the timeout period.
   *
   * @note Valid in this context means that at least one successful monitor callback was issued and
   * the channel is connected. After a reconnect, it will not wait for an extra callback.
   */
  bool WaitForValidValue(double timeout_sec) const;

private:
  void OnConnectionChanged(bool connected);
  void OnMonitorCalled(const CAMonitorInfo& info);
  const std::string m_channel_name;
  ExtendedValue m_cache;
  ChannelID m_id;
  mutable std::mutex m_mon_mtx;
  mutable std::condition_variable m_monitor_cv;
  VariableChangedCallback m_var_changed_cb;
};
}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_CHANNEL_ACCESS_PV_H_
