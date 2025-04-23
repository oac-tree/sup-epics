/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2025 ITER Organization,
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

#ifndef SUP_EPICS_PV_ACCESS_CLIENT_PV_IMPL_H_
#define SUP_EPICS_PV_ACCESS_CLIENT_PV_IMPL_H_

#include <sup/epics/pv_access_client_pv.h>

#include <pvxs/client.h>

#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>

namespace sup
{
namespace epics
{
//! Represents a client to access/update a single pvAccess variable with value cache
//! and asyncronious update.
//!
//! @note Internally relies on pvxs::client and it's structured data packaged
//! into a pvxs::Value container (specified on a server side). The design is based
//! on a pimpl idiom to hide implementation details.

class PvAccessClientPVImpl
{
public:
  /**
   * @brief Constructor.
   *
   * @param channel EPICS channel name.
   * @param context The PVXS client context to use.
   * @param cb Callback function to call when the variable's value or status changed.
   *
   * @return True if the variable was connected within the timeout period.
   */
  PvAccessClientPVImpl(const std::string& channel, std::shared_ptr<pvxs::client::Context> context,
                       PvAccessClientPV::VariableChangedCallback cb = {});
  ~PvAccessClientPVImpl();

  PvAccessClientPVImpl(const PvAccessClientPVImpl&) = delete;
  PvAccessClientPVImpl& operator=(const PvAccessClientPVImpl&) = delete;
  PvAccessClientPVImpl(PvAccessClientPVImpl&&) = delete;
  PvAccessClientPVImpl& operator=(PvAccessClientPVImpl&&) = delete;

    /**
   * @brief Check if channel is connected.
   *
   * @return True if channel is connected, false otherwise.
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
  PvAccessClientPV::ExtendedValue GetExtendedValue() const;

    /**
   * @brief Write the value to the EPICS PvAccess server.
   *
   * @param value Value to be written.
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
  void ProcessMonitor(pvxs::client::Subscription& sub);
  const std::string m_channel_name;
  std::shared_ptr<pvxs::client::Context> m_context;
  PvAccessClientPV::VariableChangedCallback m_changed_cb;
  PvAccessClientPV::ExtendedValue m_cache;
  mutable std::mutex m_mon_mtx;
  mutable std::condition_variable m_cv;
  std::shared_ptr<pvxs::client::Subscription> m_subscription;
  double m_max_put_timeout;
};

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_PV_ACCESS_CLIENT_PV_IMPL_H_
