/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP common components for EPICS network protocol
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

#ifndef SUP_EPICS_ICLIENTVARIABLE_H
#define SUP_EPICS_ICLIENTVARIABLE_H

#include "sup/dto/AnyValue.h"

#include <functional>

namespace sup::epics
{

//! Interface of a generic client variable.

class IClientVariable
{
public:
  virtual ~IClientVariable();

  virtual sup::dto::AnyValue GetValue() const = 0;
  bool virtual SetValue(const sup::dto::AnyValue& value) = 0;

  bool virtual IsConnected() const = 0;

  void virtual SetCallBack(std::function<void(const sup::dto::AnyValue&)> update_callback) = 0;
};

}  // namespace sup::epics

#endif  // SUP_EPICS_ICLIENTVARIABLE_H
