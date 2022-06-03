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

#include <sup/epics/channel_access_pv.h>

#include <sup/epics/ca_channel_manager.h>
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
  const std::string& name, const sup::dto::AnyType& type, VariableChangedCallback cb)
  : cache{}
  , id{0}
  , mon_mtx{}
  , connected_cv{}
  , var_changed_cb{std::move(cb)}
{
  id = SharedCAChannelManager().AddChannel(name, type,
    std::bind(&ChannelAccessPV::OnConnectionChanged, this,
    std::placeholders::_1, std::placeholders::_2),
    std::bind(&ChannelAccessPV::OnMonitorCalled, this,
    std::placeholders::_1, std::placeholders::_2));
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

void ChannelAccessPV::OnConnectionChanged(const std::string& name, bool connected)
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

void ChannelAccessPV::OnMonitorCalled(const std::string& name,const CAMonitorInfo& info)
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

}  // namespace epics

}  // namespace sup
