/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2024 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 *****************************************************************************/

#include <sup/epics/channel_access_pv.h>

#include <sup/epics/ca/ca_channel_manager.h>

#include <chrono>
#include <cmath>
#include <stdexcept>

namespace sup
{
namespace epics
{
ChannelAccessPV::ExtendedValue::ExtendedValue()
  : connected{false}
  , timestamp{0}
  , status{0}
  , severity{0}
  , value{}
{}

ChannelAccessPV::ChannelAccessPV(
  const std::string& channel, const sup::dto::AnyType& type, VariableChangedCallback cb)
  : channel_name{channel}
  , cache{}
  , id{0}
  , mon_mtx{}
  , monitor_cv{}
  , var_changed_cb{std::move(cb)}
{
  id = SharedCAChannelManager().AddChannel(channel, type,
    std::bind(&ChannelAccessPV::OnConnectionChanged, this, std::placeholders::_1),
    std::bind(&ChannelAccessPV::OnMonitorCalled, this, std::placeholders::_1));
  if (id == 0)
  {
    throw std::runtime_error("Could not construct ChannelAccessPV");
  }
}

ChannelAccessPV::~ChannelAccessPV()
{
  if (id > 0)
  {
    SharedCAChannelManager().RemoveChannel(id);
  }
}

bool ChannelAccessPV::IsConnected() const
{
  std::lock_guard<std::mutex> lk(mon_mtx);
  return cache.connected;
}

std::string ChannelAccessPV::GetChannelName() const
{
  return channel_name;
}

sup::dto::AnyValue ChannelAccessPV::GetValue() const
{
  std::lock_guard<std::mutex> lk(mon_mtx);
  if(!cache.connected)
  {
    return {};
  }
  return cache.value;
}

ChannelAccessPV::ExtendedValue ChannelAccessPV::GetExtendedValue() const
{
  std::lock_guard<std::mutex> lk(mon_mtx);
  return cache;
}

bool ChannelAccessPV::SetValue(const sup::dto::AnyValue& value)
{
  ChannelID local_id;
  {
    std::lock_guard<std::mutex> lk(mon_mtx);
    local_id = id;
  }
  return SharedCAChannelManager().UpdateChannel(local_id, value);
}

bool ChannelAccessPV::WaitForConnected(double timeout_sec) const
{
  auto end_time = std::chrono::system_clock::now() +
                 std::chrono::nanoseconds(std::lround(timeout_sec * 1e9));
  std::unique_lock<std::mutex> lk(mon_mtx);
  auto pred = [this]{
    return cache.connected;
  };
  return monitor_cv.wait_until(lk, end_time, pred);
}

bool ChannelAccessPV::WaitForValidValue(double timeout_sec) const
{
  auto end_time = std::chrono::system_clock::now() +
                 std::chrono::nanoseconds(std::lround(timeout_sec * 1e9));
  std::unique_lock<std::mutex> lk(mon_mtx);
  auto pred = [this]{
    return cache.connected && !sup::dto::IsEmptyValue(cache.value);
  };
  return monitor_cv.wait_until(lk, end_time, pred);
}

void ChannelAccessPV::OnConnectionChanged(bool connected)
{
  ExtendedValue result;
  {
    std::lock_guard<std::mutex> lk(mon_mtx);
    cache.connected = connected;
    result = cache;
  }
  monitor_cv.notify_one();
  if (var_changed_cb)
  {
    var_changed_cb(result);
  }
}

void ChannelAccessPV::OnMonitorCalled(const CAMonitorInfo& info)
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
  monitor_cv.notify_one();
  if (var_changed_cb)
  {
    var_changed_cb(result);
  }
}

}  // namespace epics

}  // namespace sup
