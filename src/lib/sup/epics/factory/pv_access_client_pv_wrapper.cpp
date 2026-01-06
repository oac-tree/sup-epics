/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2026 ITER Organization,
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

#include "pv_access_client_pv_wrapper.h"

#include <sup/protocol/exceptions.h>

namespace sup
{
namespace epics
{
PVAccessClientPVWrapper::PVAccessClientPVWrapper(const std::string& channel)
  : m_callback{}
  , m_cb_mtx{}
  , m_pv_impl{}
{
  auto callback = [this](const PvAccessClientPV::ExtendedValue& val){
    return OnUpdate(val);
  };
  m_pv_impl = std::make_unique<PvAccessClientPV>(channel, callback);
}

PVAccessClientPVWrapper::~PVAccessClientPVWrapper() = default;

bool PVAccessClientPVWrapper::IsAvailable() const
{
  return m_pv_impl->WaitForValidValue(0.0);
}

std::pair<bool, sup::dto::AnyValue> PVAccessClientPVWrapper::GetValue(double timeout_sec) const
{
  if (!m_pv_impl->WaitForValidValue(timeout_sec))
  {
    return { false, {} };
  }
  auto ext_val = m_pv_impl->GetExtendedValue();
  return { ext_val.connected, ext_val.value };
}

bool PVAccessClientPVWrapper::SetValue(const sup::dto::AnyValue& value, double timeout_sec)
{
  if (!m_pv_impl->WaitForConnected(timeout_sec))
  {
    return false;
  }
  return m_pv_impl->SetValue(value);
}

bool PVAccessClientPVWrapper::WaitForAvailable(double timeout_sec) const
{
  return m_pv_impl->WaitForValidValue(timeout_sec);
}

bool PVAccessClientPVWrapper::SetMonitorCallback(Callback func)
{
  std::lock_guard<std::mutex> lk{m_cb_mtx};
  m_callback = func;
  return true;
}

void PVAccessClientPVWrapper::OnUpdate(const PvAccessClientPV::ExtendedValue& val)
{
  std::lock_guard<std::mutex> lk{m_cb_mtx};
  if (m_callback)
  {
    m_callback(val.value, val.connected);
  }
}

}  // namespace epics

}  // namespace sup
