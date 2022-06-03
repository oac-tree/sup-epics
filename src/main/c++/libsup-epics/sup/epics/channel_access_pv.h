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

#ifndef SUP_EPICS_CHANNEL_ACCESS_VARIABLE_H_
#define SUP_EPICS_CHANNEL_ACCESS_VARIABLE_H_

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
  using VariableChangedCallback = std::function<void(const std::string&, const ExtendedValue&)>;
  /**
   * @brief Constructor.
   *
   * @param name EPICS channel name.
   * @param type Type to use for the connected channel.
   * @param cb Callback function to call when the variable's value or status changed.
   *
   * @return True if the variable was connected within the timeout period.
   *
   * @throws std::runtime_error when the EPICS context or channel could not be created (not
   * related to the fact that the specific PV might not be connected).
   */
  ChannelAccessPV(const std::string& name, const sup::dto::AnyType& type,
                        VariableChangedCallback cb = {});

    /**
   * @brief Destructor.
   */
  ~ChannelAccessPV();

    /**
   * @brief Check if variable is connected.
   *
   * @return True if variable is connected, false otherwise.
   */
  bool IsConnected() const;

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

private:
  void OnConnectionChanged(const std::string& name, bool connected);
  void OnMonitorCalled(const std::string& name,const CAMonitorInfo& info);
  ExtendedValue cache;
  ChannelID id;
  mutable std::mutex mon_mtx;
  mutable std::condition_variable connected_cv;
  VariableChangedCallback var_changed_cb;
};
}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_CHANNEL_ACCESS_VARIABLE_H_
