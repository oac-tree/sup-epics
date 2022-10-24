/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Gennady Pospelov (IO)
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

#ifndef SUP_EPICS_PV_CLIENT_PV_H_
#define SUP_EPICS_PV_CLIENT_PV_H_

#include <sup/dto/anyvalue.h>

#include <functional>
#include <memory>

namespace sup
{
namespace epics
{
class PvAccessClientPVImpl;

//! Represents a client to access/update a single pvAccess variable with value cache
//! and asyncronious update.
//!
//! @note Internally relies on pvxs::client and it's structured data packaged
//! into a pvxs::Value container (specified on a server side). The design is based
//! on a pimpl idiom to hide implementation details.

class PvAccessClientPV
{
public:
  struct ExtendedValue
  {
    ExtendedValue();
    bool connected;
    sup::dto::AnyValue value;
  };
  using VariableChangedCallback = std::function<void(const ExtendedValue&)>;

  /**
   * @brief Constructor.
   *
   * @param channel EPICS channel name.
   * @param cb Callback function to call when the variable's value or status changed.
   */
  PvAccessClientPV(const std::string& channel, VariableChangedCallback cb = {});

  /**
   * @brief Constructor.
   *
   * @param impl Injected implementation.
   */
  PvAccessClientPV(std::unique_ptr<PvAccessClientPVImpl>&& impl);

  ~PvAccessClientPV();

  PvAccessClientPV(const PvAccessClientPV&) = delete;
  PvAccessClientPV& operator=(const PvAccessClientPV&) = delete;
  PvAccessClientPV(PvAccessClientPV&&) = delete;
  PvAccessClientPV& operator=(PvAccessClientPV&&) = delete;

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
  ExtendedValue GetExtendedValue() const;

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
  std::unique_ptr<PvAccessClientPVImpl> m_impl;
};

bool operator==(const PvAccessClientPV::ExtendedValue& lhs, const PvAccessClientPV::ExtendedValue& rhs);

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_PV_CLIENT_PV_H_
