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

#ifndef SUP_EPICS_PV_ACCESS_SERVER_H_
#define SUP_EPICS_PV_ACCESS_SERVER_H_

#include <sup/epics/dto_types_fwd.h>

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace sup
{
namespace epics
{

//! Represents a server to run multiple pvAccess variables.

class PVAccessServer
{
public:
  using callback_t = std::function<void(const std::string&, const sup::dto::AnyValue&)>;
  using context_t = std::unique_ptr<pvxs::server::Server>;

  //! Constructor.
  //! @param context PVXS server.
  //! @param callback A callback to report changed variable.
  explicit PVAccessServer(context_t context, callback_t callback = {});
  ~PVAccessServer();

  PVAccessServer(const PVAccessServer&) = delete;
  PVAccessServer& operator=(const PVAccessServer&) = delete;
  PVAccessServer(PVAccessServer&&) = delete;
  PVAccessServer& operator=(PVAccessServer&&) = delete;

  //! Add variable to the server with given channl name and initial value. Type of underlying PVA
  //! record will be deduced from AnyValue type. Will throw if such channel already exists.
  //! @param name EPICS channel name.
  //! @param any_value Initial value.
  void AddVariable(const std::string& name, const sup::dto::AnyValue& any_value);

  //! Returns the names of all managed channels.
  //! @return List of all channel names.
  std::vector<std::string> GetVariableNames() const;

  //! Get the value from a specific channel. Will throw if channel was not added yet.
  //! @param name EPICS channel name.
  //! @return Channel's value.
  sup::dto::AnyValue GetValue(const std::string& name) const;

  //! Propagate the value to a specific channel. Will throw if channel was not added yet.
  //! @param name EPICS channel name.
  //! @param value Value to be written to the channel.
  //! @return True if successful, false otherwise.
  bool SetValue(const std::string& name, const sup::dto::AnyValue& value);

  //! Starts PVXS server and publishes all added variables.
  void Start();

private:
  struct PVAccessServerImpl;
  PVAccessServerImpl* p_impl{nullptr};
};

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_PV_ACCESS_SERVER_H_
