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

#ifndef SPU_EPICS_PVXS_PV_ACCESS_CLIENT_VARIABLE_H_
#define SPU_EPICS_PVXS_PV_ACCESS_CLIENT_VARIABLE_H_

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

class PVAccessClientVariable
{
public:
  using context_t = std::weak_ptr<pvxs::client::Context>;
  using callback_t = std::function<void(const sup::dto::AnyValue&)>;

  //! Constructor
  //! @param variable_name EPICS channel name.
  //! @param context A weak pointer to shared PVXS client's context.
  //! @param callback A callback to report changed variable.
  explicit PVAccessClientVariable(const std::string& variable_name, context_t context,
                                  callback_t callbacpk = {});
  ~PVAccessClientVariable();

  PVAccessClientVariable(const PVAccessClientVariable&) = delete;
  PVAccessClientVariable& operator=(const PVAccessClientVariable&) = delete;

  std::string GetVariableName() const;

  bool IsConnected() const;

  //! Returns the variable's value.
  //! @return The value of underlying cache variable if connected, empty value otherwise.
  sup::dto::AnyValue GetValue() const;

  //! The PVXS variable held in the cache is assigned with the <value> parameter and marked for
  //! asynchronous update. Will throw if assignment was not possible.
  bool SetValue(const sup::dto::AnyValue& value);

private:
  struct PVAccessClientVariableImpl;
  PVAccessClientVariableImpl* p_impl;
};

}  // namespace epics

}  // namespace sup

#endif  // SPU_EPICS_PVXS_PV_ACCESS_CLIENT_VARIABLE_H_
