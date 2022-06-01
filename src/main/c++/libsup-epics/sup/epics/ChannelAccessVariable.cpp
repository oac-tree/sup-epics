/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
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
#include <chrono>
#include <cmath>
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
  const std::string& name, const sup::dto::AnyType& type, VariableChangedCallback cb)
  : cache{}
  , id{0}
  , mon_mtx{}
  , connected_cv{}
  , var_changed_cb{std::move(cb)}
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

bool ChannelAccessVariable::WaitForConnected(double timeout_sec) const
{
  auto timeout = std::chrono::system_clock::now() +
                 std::chrono::nanoseconds(std::lround(timeout_sec * 1e9));
  std::unique_lock<std::mutex> lk(mon_mtx);
  bool connected = cache.connected;
  while (!connected)
  {
    auto wait_result = connected_cv.wait_until(lk, timeout);
    if (wait_result == std::cv_status::timeout)
    {
      return false;
    }
    connected = cache.connected;
  }
  return connected;
}

void ChannelAccessVariable::OnConnectionChanged(const std::string& name, bool connected)
{
  ExtendedValue result;
  {
    std::lock_guard<std::mutex> lk(mon_mtx);
    cache.connected = connected;
    result = cache;
  }
  connected_cv.notify_one();
  if (var_changed_cb)
  {
    var_changed_cb(name, result);
  }
}

void ChannelAccessVariable::OnMonitorCalled(const std::string& name,const CAMonitorInfo& info)
{
  ExtendedValue result;
  {
    std::lock_guard<std::mutex> lk(mon_mtx);
    cache.timestamp = info.timestamp;
    cache.status = info.status;
    cache.severity = info.severity;
    cache.value = info.value;
    result = cache;
  }
  if (var_changed_cb)
  {
    var_changed_cb(name, result);
  }
}

}  // namespace sup::epics
