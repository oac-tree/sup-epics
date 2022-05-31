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

#include "ChannelAccessVariable.h"

#include "CAChannelManager.h"
#include <stdexcept>

namespace sup::epics
{
ChannelAccessVariable::ExtendedValue::ExtendedValue()
  : connected{false}
  , timestamp{0}
  , status{0}
  , severity{0}
  , value{}
{}

ChannelAccessVariable::ChannelAccessVariable(
  const std::string& name, const sup::dto::AnyType& type)
  : cache{}
  , id{0}
{
  id = SharedCAChannelManager().AddChannel(name, type,
    std::bind(&ChannelAccessVariable::OnConnectionChanged, this,
    std::placeholders::_1, std::placeholders::_2),
    std::bind(&ChannelAccessVariable::OnMonitorCalled, this,
    std::placeholders::_1, std::placeholders::_2));
  if (id == 0)
  {
    throw std::runtime_error("Could not construct ChannelAccessVariable");
  }
}

ChannelAccessVariable::~ChannelAccessVariable()
{
  if (id > 0)
  {
    SharedCAChannelManager().RemoveChannel(id);
  }
}

bool ChannelAccessVariable::IsConnected() const;
sup::dto::AnyValue ChannelAccessVariable::GetValue() const;
ExtendedValue ChannelAccessVariable::GetExtendedValue() const;

bool ChannelAccessVariable::SetValue(const sup::dto::AnyValue& value);

bool ChannelAccessVariable::SetCallBack(std::function<void(const std::string&, const sup::dto::AnyValue&)> cb);

}  // namespace sup::epics

#endif  // SUP_EPICS_ChannelAccessVariable_H
