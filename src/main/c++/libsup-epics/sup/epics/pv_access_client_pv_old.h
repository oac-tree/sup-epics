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

#ifndef SUP_EPICS_PV_ACCESS_CLIENT_PV_H_
#define SUP_EPICS_PV_ACCESS_CLIENT_PV_H_

#include <sup/epics/dto_types_fwd.h>

#include <functional>
#include <memory>
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

class PvAccessClientPV_old
{
public:
  using context_t = std::weak_ptr<pvxs::client::Context>;
  using callback_t = std::function<void(const sup::dto::AnyValue&)>;

  //! Constructor.
  //! @param variable_name EPICS channel name.
  //! @param context A weak pointer to shared PVXS client's context.
  //! @param callback A callback to report changed variable.
  explicit PvAccessClientPV_old(const std::string& variable_name, context_t context,
                                  callback_t callback = {});
  ~PvAccessClientPV_old();

  PvAccessClientPV_old(const PvAccessClientPV_old&) = delete;
  PvAccessClientPV_old& operator=(const PvAccessClientPV_old&) = delete;
  PvAccessClientPV_old(PvAccessClientPV_old&&) = delete;
  PvAccessClientPV_old& operator=(PvAccessClientPV_old&&) = delete;

  std::string GetVariableName() const;

  bool IsConnected() const;

  //! Returns the variable's value.
  //! @return The value of underlying cache variable if connected, empty value otherwise.
  sup::dto::AnyValue GetValue() const;

  //! The PVXS variable held in the cache is assigned with the <value> parameter and marked for
  //! asynchronous update. Will throw if assignment was not possible.
  bool SetValue(const sup::dto::AnyValue& value);

  //! This method waits for the variable to be connected with a timeout.
  //! @param timeout_sec Timeout in seconds to wait for the variable to be connected.
  //! @return True if the variable was connected within the timeout period.
  bool WaitForConnected(double timeout_sec) const;

private:
  struct PVAccessClientVariableImpl;
  PVAccessClientVariableImpl* p_impl{nullptr};
};

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_PV_ACCESS_CLIENT_PV_H_
