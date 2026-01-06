/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Gennady Pospelov (IO)
 *
 * Copyright (c) : 2010-2026 ITER Organization,
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

#ifndef SUP_EPICS_PV_ACCESS_SERVER_PV_H_
#define SUP_EPICS_PV_ACCESS_SERVER_PV_H_

#include <sup/epics/utils/dto_conversion_utils.h>

#include <sup/dto/anyvalue.h>

#include <pvxs/data.h>
#include <pvxs/server.h>
#include <pvxs/sharedpv.h>

#include <functional>
#include <mutex>
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
  using VariableChangedCallback = std::function<void(const sup::dto::AnyValue&)>;

  PvAccessServerPV(const std::string& variable_name, const sup::dto::AnyValue& any_value,
                         VariableChangedCallback callback);
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

  //! Add variable to given server. Will throw if variable has been already added.
  void AddToServer(pvxs::server::Server& server);

private:
  void OnSharedValueChanged(pvxs::server::SharedPV& pv,
                            std::unique_ptr<pvxs::server::ExecOp>&& op, pvxs::Value&& value);
  const std::string m_variable_name;
  sup::dto::AnyValue m_any_value;  //!< The main value of this variable.
  pvxs::Value m_pvxs_cache;        //!< Necessary for open/post operations of SharedPV
  VariableChangedCallback m_callback;
  pvxs::server::SharedPV m_shared_pv;
  mutable std::mutex m_mutex;
};

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_PV_ACCESS_SERVER_PV_H_
