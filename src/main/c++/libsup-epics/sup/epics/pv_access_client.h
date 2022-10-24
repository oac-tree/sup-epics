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

#ifndef SUP_EPICS_PV_ACCESS_CLIENT_H_
#define SUP_EPICS_PV_ACCESS_CLIENT_H_

#include <sup/epics/pv_access_client_pv.h>

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace sup
{
namespace epics
{
class PvAccessClientImpl;

//! Represents a client to access/update multiple pvAccess variables.

class PvAccessClient
{
public:
  using VariableChangedCallback =
    std::function<void(const std::string&, const PvAccessClientPV::ExtendedValue&)>;

  //! Constructor.
  //! @param cb A callback to report changed variable.
  explicit PvAccessClient(VariableChangedCallback cb = {});
  explicit PvAccessClient(std::unique_ptr<PvAccessClientImpl>&& impl);

  ~PvAccessClient();

  PvAccessClient(const PvAccessClient&) = delete;
  PvAccessClient& operator=(const PvAccessClient&) = delete;
  PvAccessClient(PvAccessClient&&) = delete;
  PvAccessClient& operator=(PvAccessClient&&) = delete;

  //! Add variable with the given channel. Will throw if such channel already exists.
  //! @param channel EPICS channel name.
  void AddVariable(const std::string& channel);

  //! Returns the names of all managed channels.
  //! @return List of all channel names.
  std::vector<std::string> GetVariableNames() const;

  //! Check if specific channel is connected.
  //! @param channel EPICS channel name.
  //! @return True if channel is connected, false otherwise.
  bool IsConnected(const std::string& channel) const;

  //! Get the value from a specific channel. Will throw if channel was not added yet.
  //! @param channel EPICS channel name.
  //! @return Channel's value.
  sup::dto::AnyValue GetValue(const std::string& channel) const;

  //! Propagate the value to a specific channel. Will throw if channel was not added yet.
  //! @param channel EPICS channel name.
  //! @param value Value to be written to the channel.
  //! @return True if successful, false otherwise.
  bool SetValue(const std::string& channel, const sup::dto::AnyValue& value);

private:
  std::unique_ptr<PvAccessClientImpl> m_impl;
};

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_PV_ACCESS_CLIENT_H_
