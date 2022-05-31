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
  , mon_mtx{}
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

bool ChannelAccessVariable::IsConnected() const
{
  std::lock_guard<std::mutex> lk(mon_mtx);
  return cache.connected;
}

sup::dto::AnyValue ChannelAccessVariable::GetValue() const
{
  std::lock_guard<std::mutex> lk(mon_mtx);
  if(!cache.connected)
  {
    return {};
  }
  return cache.value;
}

ChannelAccessVariable::ExtendedValue ChannelAccessVariable::GetExtendedValue() const
{
  std::lock_guard<std::mutex> lk(mon_mtx);
  return cache;
}

bool ChannelAccessVariable::SetValue(const sup::dto::AnyValue& value)
{
  ChannelID local_id;
  {
    std::lock_guard<std::mutex> lk(mon_mtx);
    local_id = id;
  }
  return SharedCAChannelManager().UpdateChannel(local_id, value);
}

bool ChannelAccessVariable::SetCallBack(
  std::function<void(const std::string&, const sup::dto::AnyValue&)> cb)
{
  return true;
}

void ChannelAccessVariable::OnConnectionChanged(const std::string& name, bool connected)
{
  std::lock_guard<std::mutex> lk(mon_mtx);
  cache.connected = connected;
}

void ChannelAccessVariable::OnMonitorCalled(const std::string& name,const CAMonitorInfo& info)
{
  std::lock_guard<std::mutex> lk(mon_mtx);
  cache.timestamp = info.timestamp;
  cache.status = info.status;
  cache.severity = info.severity;
  cache.value = info.value;
}

}  // namespace sup::epics
