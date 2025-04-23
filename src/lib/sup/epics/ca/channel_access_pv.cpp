/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2025 ITER Organization,
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
  : m_channel_name{channel}
  , m_cache{}
  , m_id{0}
  , m_mon_mtx{}
  , m_monitor_cv{}
  , m_var_changed_cb{std::move(cb)}
{
  m_id = SharedCAChannelManager().AddChannel(channel, type,
    std::bind(&ChannelAccessPV::OnConnectionChanged, this, std::placeholders::_1),
    std::bind(&ChannelAccessPV::OnMonitorCalled, this, std::placeholders::_1));
  if (m_id == 0)
  {
    throw std::runtime_error("Could not construct ChannelAccessPV");
  }
}

ChannelAccessPV::~ChannelAccessPV()
{
  if (m_id > 0)
  {
    SharedCAChannelManager().RemoveChannel(m_id);
  }
}

bool ChannelAccessPV::IsConnected() const
{
  std::lock_guard<std::mutex> lk(m_mon_mtx);
  return m_cache.connected;
}

std::string ChannelAccessPV::GetChannelName() const
{
  return m_channel_name;
}

sup::dto::AnyValue ChannelAccessPV::GetValue() const
{
  std::lock_guard<std::mutex> lk(m_mon_mtx);
  if(!m_cache.connected)
  {
    return {};
  }
  return m_cache.value;
}

ChannelAccessPV::ExtendedValue ChannelAccessPV::GetExtendedValue() const
{
  std::lock_guard<std::mutex> lk(m_mon_mtx);
  return m_cache;
}

bool ChannelAccessPV::SetValue(const sup::dto::AnyValue& value)
{
  return SharedCAChannelManager().UpdateChannel(m_id, value);
}

bool ChannelAccessPV::WaitForConnected(double timeout_sec) const
{
  auto duration = std::chrono::duration<double>(timeout_sec);
  auto pred = [this]{
    return m_cache.connected;
  };
  std::unique_lock<std::mutex> lk(m_mon_mtx);
  return m_monitor_cv.wait_for(lk, duration, pred);
}

bool ChannelAccessPV::WaitForValidValue(double timeout_sec) const
{
  auto duration = std::chrono::duration<double>(timeout_sec);
  auto pred = [this]{
    return m_cache.connected && !sup::dto::IsEmptyValue(m_cache.value);
  };
  std::unique_lock<std::mutex> lk(m_mon_mtx);
  return m_monitor_cv.wait_for(lk, duration, pred);
}

void ChannelAccessPV::OnConnectionChanged(bool connected)
{
  {
    std::lock_guard<std::mutex> lk(m_mon_mtx);
    m_cache.connected = connected;
    if (m_var_changed_cb)
    {
      m_var_changed_cb(m_cache);
    }
  }
  m_monitor_cv.notify_one();
}

void ChannelAccessPV::OnMonitorCalled(const CAMonitorInfo& info)
{
  {
    std::lock_guard<std::mutex> lk(m_mon_mtx);
    m_cache.timestamp = info.timestamp;
    m_cache.status = info.status;
    m_cache.severity = info.severity;
    m_cache.value = info.value;
    if (m_var_changed_cb)
    {
      m_var_changed_cb(m_cache);
    }
  }
  m_monitor_cv.notify_one();
}

}  // namespace epics

}  // namespace sup
