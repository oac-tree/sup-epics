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

#ifndef SUP_EPICS_PV_ACCESS_SERVER_PV_H_
#define SUP_EPICS_PV_ACCESS_SERVER_PV_H_

#include <sup/epics/dto_types_fwd.h>

#include <functional>
#include <string>

namespace sup
{
namespace epics
{

//! Represents a single variable on board of PvAccessServer.
//!
//! @note Based on PVXS shared variable. The variable is initialized with AnyValue used at the
//! construction. Scalar AnyValues are converted to structs containing a single `value` field. This
//! is because PVXS shared variables can be initialized only with structs.

class PvAccessServerPV
{
public:
  using callback_t = std::function<void(const sup::dto::AnyValue&)>;

  PvAccessServerPV(const std::string& variable_name, const sup::dto::AnyValue& any_value,
                         callback_t callback);
  ~PvAccessServerPV();

  PvAccessServerPV(const PvAccessServerPV&) = delete;
  PvAccessServerPV& operator=(const PvAccessServerPV&) = delete;
  PvAccessServerPV(PvAccessServerPV&&) = delete;
  PvAccessServerPV& operator=(PvAccessServerPV&&) = delete;

  std::string GetVariableName() const;

  //! Returns the variable's value.
  //! @return The value of underlying cache variable if connected, empty value otherwise.
  sup::dto::AnyValue GetValue() const;

  //! The PVXS variable held in the cache is assigned with the <value> parameter and marked for
  //! asynchronous update. Will throw if assignment was not possible.
  bool SetValue(const sup::dto::AnyValue& value);

  //! Add variable to given server. Will throw if variables has been already added.
  void AddToServer(pvxs::server::Server& server);

private:
  struct PVAccessServerVariableImpl;
  PVAccessServerVariableImpl* p_impl{nullptr};
};

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_PV_ACCESS_SERVER_PV_H_
