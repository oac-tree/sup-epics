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

#include "sup/epics/pv_client_pv.h"

#include <pvxs/client.h>

#include <chrono>
#include <cmath>

namespace sup
{
namespace epics
{

PvClientPV::ExtendedValue::ExtendedValue()
  : connected{false}
  , value{}
{}

PvClientPV::PvClientPV(const std::string& channel, const sup::dto::AnyType& anytype,
                       VariableChangedCallback cb)
    : m_channel_name{channel}
    , m_anytype{anytype}
    , m_cache{}
    , m_mon_mtx{}
    , m_cv{}
    , m_changed_cb{std::move(cb)}
{
}

PvClientPV::~PvClientPV() = default;

bool PvClientPV::IsConnected() const
{
  std::lock_guard<std::mutex> lk(m_mon_mtx);
  return m_cache.connected;
}

std::string PvClientPV::GetChannelName() const
{
  return m_channel_name;
}

sup::dto::AnyValue PvClientPV::GetValue() const
{
  std::lock_guard<std::mutex> lk(m_mon_mtx);
  if (!m_cache.connected)
  {
    return {};
  }
  return m_cache.value;
}

PvClientPV::ExtendedValue PvClientPV::GetExtendedValue() const
{
  std::lock_guard<std::mutex> lk(m_mon_mtx);
  return m_cache;
}

bool PvClientPV::SetValue(const sup::dto::AnyValue& value)
{
  (void)value;
  return true;
}

bool PvClientPV::WaitForConnected(double timeout_sec) const
{
  auto end_time = std::chrono::system_clock::now() +
                 std::chrono::nanoseconds(std::lround(timeout_sec * 1e9));
  std::unique_lock<std::mutex> lk(m_mon_mtx);
  auto pred = [this]{
    return m_cache.connected;
  };
  return m_cv.wait_until(lk, end_time, pred);
}

bool PvClientPV::WaitForValidValue(double timeout_sec) const
{
  auto end_time = std::chrono::system_clock::now() +
                 std::chrono::nanoseconds(std::lround(timeout_sec * 1e9));
  std::unique_lock<std::mutex> lk(m_mon_mtx);
  auto pred = [this]{
    return m_cache.connected && !sup::dto::IsEmptyValue(m_cache.value);
  };
  return m_cv.wait_until(lk, end_time, pred);
}

}  // namespace epics

}  // namespace sup
