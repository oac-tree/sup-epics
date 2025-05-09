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

#include "pv_access_client_pv_impl.h"

#include <sup/epics/utils/dto_conversion_utils.h>

#include <sup/dto/anyvalue_helper.h>

#include <pvxs/client.h>

#include <chrono>
#include <cmath>

namespace sup
{
namespace epics
{

PvAccessClientPVImpl::PvAccessClientPVImpl(const std::string& channel,
  std::shared_ptr<pvxs::client::Context> context, PvAccessClientPV::VariableChangedCallback cb)
  : m_channel_name{channel}
  , m_context{std::move(context)}
  , m_changed_cb{std::move(cb)}
  , m_cache{}
  , m_mon_mtx{}
  , m_cv{}
  , m_subscription{}
  , m_max_put_timeout{2.0}
{
  if (!m_context)
  {
    throw std::runtime_error("Constructing PvAccessClientPVImpl without context.");
  }
  m_subscription = m_context->monitor(m_channel_name)
                              .maskConnected(false)
                              .maskDisconnected(false)
                              .event([this](pvxs::client::Subscription& sup)
                                     {
                                       ProcessMonitor(sup);
                                     })
                              .exec();
}

PvAccessClientPVImpl::~PvAccessClientPVImpl() = default;

bool PvAccessClientPVImpl::IsConnected() const
{
  std::lock_guard<std::mutex> lk(m_mon_mtx);
  return m_cache.connected;
}

std::string PvAccessClientPVImpl::GetChannelName() const
{
  return m_channel_name;
}

sup::dto::AnyValue PvAccessClientPVImpl::GetValue() const
{
  std::lock_guard<std::mutex> lk(m_mon_mtx);
  if (!m_cache.connected)
  {
    return {};
  }
  return m_cache.value;
}

PvAccessClientPV::ExtendedValue PvAccessClientPVImpl::GetExtendedValue() const
{
  std::lock_guard<std::mutex> lk(m_mon_mtx);
  return m_cache;
}

bool PvAccessClientPVImpl::SetValue(const sup::dto::AnyValue& value)
{
  sup::dto::AnyValue copy;
  {
    std::lock_guard<std::mutex> lk(m_mon_mtx);
    if (!m_cache.connected)
    {
      return false;
    }
    copy = m_cache.value;
  }
  if (sup::dto::IsScalarValue(value))
  {
    throw std::runtime_error("Error in PvAccessClientPV: cannot set a scalar value");
  }
  if (!sup::dto::TryAssignIfEmptyOrConvert(copy, value))
  {
    return false;
  }
  auto pvxs_value = sup::epics::BuildPVXSValue(copy);

  auto operation = m_context->put(m_channel_name)
                    .build([pvxs_value](pvxs::Value&& /*proto*/) { return pvxs_value; })
                    .exec();
  try
  {
    operation->wait(m_max_put_timeout);
  }
  catch (const pvxs::client::Timeout& ex)
  {
    return false;
  }
  catch (const pvxs::client::Interrupted& ex)
  {
    return false;
  }
  return true;
}

bool PvAccessClientPVImpl::WaitForConnected(double timeout_sec) const
{
  auto duration = std::chrono::duration<double>(timeout_sec);
  auto pred = [this]{
    return m_cache.connected;
  };
  std::unique_lock<std::mutex> lk(m_mon_mtx);
  return m_cv.wait_for(lk, duration, pred);
}

bool PvAccessClientPVImpl::WaitForValidValue(double timeout_sec) const
{
  auto duration = std::chrono::duration<double>(timeout_sec);
  auto pred = [this]{
    return m_cache.connected && !sup::dto::IsEmptyValue(m_cache.value);
  };
  std::unique_lock<std::mutex> lk(m_mon_mtx);
  return m_cv.wait_for(lk, duration, pred);
}

void PvAccessClientPVImpl::ProcessMonitor(pvxs::client::Subscription& sub)
{
  {
    std::lock_guard<std::mutex> lk(m_mon_mtx);
    auto result = m_cache;
    while (true)
    {
      try
      {
        auto update = sub.pop();
        if (update)
        {
          if (!sup::dto::TryAssignIfEmptyOrConvert(result.value, sup::epics::BuildAnyValue(update)))
          {
            throw std::runtime_error("PvAccessClientPVImpl received incompatible value update.");
          }
        }
        else
        {
          break;
        }
      }
      catch (pvxs::client::Connected& ex)
      {
        result.connected = true;
      }
      catch (pvxs::client::Disconnect& ex)
      {
        result.connected = false;
      }
      catch (std::exception& ex)
      {
        throw ex;
      }
    }
    m_cache = result;
    if (m_changed_cb)
    {
      m_changed_cb(result);
    }
  }
  m_cv.notify_one();
}

}  // namespace epics

}  // namespace sup
