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

#include "sup/epics/pvxs/pv_access_client_variable.h"

#include <pvxs/client.h>
#include <sup/dto/anyvalue.h>
#include <sup/epics/dto_conversion_utils.h>

#include <mutex>
#include <thread>

namespace sup
{
namespace epics
{
struct PVAccessClientVariable::PVAccessClientVariableImpl
{
  using subscription_t = pvxs::client::Subscription;
  std::string m_variable_name;
  pvxs::Value m_pvxs_value;
  context_t m_context;
  callback_t m_callback;
  bool m_is_connected{false};
  std::shared_ptr<subscription_t> m_subscription;
  std::shared_ptr<pvxs::client::Operation> m_operation;
  std::mutex m_mutex;

  PVAccessClientVariableImpl(const std::string& variable_name, context_t context,
                             callback_t callback)
      : m_variable_name(variable_name), m_context(context), m_callback(callback)
  {
  }

  //! Inits subscription to monitor the variable.
  void InitSubscription()
  {
    if (auto sp = m_context.lock())
    {
      m_subscription = sp->monitor(m_variable_name.c_str())
                           .maskConnected(false)
                           .maskDisconnected(false)
                           .event([this](subscription_t& sup) { ProcessMonitorEvents(sup); })
                           .exec();
    }
    else
    {
      throw std::runtime_error("Error in PVAccessClientVariable: context has expired.");
    }
  }

  //! Processes monitoring events coming from PVXS. Function updates isConnected
  //! flag and the value of cache variable accordint to server reports.
  void ProcessMonitorEvents(subscription_t& sub)
  {
    std::lock_guard<std::mutex> guard(m_mutex);
    bool can_pop{true};
    while (can_pop)
    {
      try
      {
        auto update = sub.pop();
        if (update)
        {
          m_pvxs_value = update;
          if (m_callback)
          {
            m_callback(sup::epics::BuildAnyValue(m_pvxs_value));
          }
        }
        else
        {
          can_pop = false;
        }
      }
      catch (pvxs::client::Connected& ex)
      {
        m_is_connected = true;
      }
      catch (pvxs::client::Disconnect& ex)
      {
        m_is_connected = false;
      }
      catch (std::exception& ex)
      {
        throw ex;
      }
    }
  }

  //! Sets the cache variable and schedules update of the server.
  bool SetCache(const sup::dto::AnyValue& any_value)
  {
    std::lock_guard<std::mutex> guard(m_mutex);
    pvxs::Value new_pvxs_value = sup::epics::BuildPVXSValue(any_value);

    m_pvxs_value = new_pvxs_value;
    // copying the value inside lambda
    if (auto sp = m_context.lock())
    {
      m_operation = sp->put(m_variable_name.c_str())
                        .build([new_pvxs_value](pvxs::Value&& /*proto*/) { return new_pvxs_value; })
                        .exec();
    }
    else
    {
      throw std::runtime_error("Error in PVAccessClientVariable: context has expired.");
    }
    return true;
  }
};

PVAccessClientVariable::PVAccessClientVariable(const std::string& variable_name, context_t context,
                                               callback_t callback)
    : p_impl(new PVAccessClientVariableImpl(variable_name, context, callback))
{
  p_impl->InitSubscription();
}

PVAccessClientVariable::~PVAccessClientVariable()
{
  delete p_impl;
}

std::string PVAccessClientVariable::GetVariableName() const
{
  return p_impl->m_variable_name;
}

bool PVAccessClientVariable::IsConnected() const
{
  return p_impl->m_is_connected;
}

sup::dto::AnyValue PVAccessClientVariable::GetValue() const
{
  return sup::epics::BuildAnyValue(p_impl->m_pvxs_value);
}

bool PVAccessClientVariable::SetValue(const sup::dto::AnyValue& value)
{
  return p_impl->SetCache(value);
}

bool PVAccessClientVariable::WaitForConnected(double timeout_sec) const
{
  const size_t msec_in_sec = 1000;
  const std::chrono::milliseconds timeout_precision_msec(10);
  auto timeout = std::chrono::system_clock::now()
                 + std::chrono::milliseconds(static_cast<long>(timeout_sec * msec_in_sec));
  while (std::chrono::system_clock::now() < timeout)
  {
    if (IsConnected())
    {
      return true;
    }
    std::this_thread::sleep_for(timeout_precision_msec);
  }
  return false;
}

}  // namespace epics

}  // namespace sup
