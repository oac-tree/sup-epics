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
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 *****************************************************************************/

#include "pv_access_server_pv_wrapper.h"

#include <sup/protocol/exceptions.h>

namespace sup
{
namespace epics
{
PVAccessServerPVWrapper::PVAccessServerPVWrapper(const std::string& channel,
                                                 const sup::dto::AnyValue& value)
  : m_channel{channel}
  , m_callback{}
  , m_cb_mtx{}
  , m_server{}
{
  auto callback = [this](const std::string&, const sup::dto::AnyValue& val){
    return OnUpdate(val);
  };
  m_server = std::make_unique<PvAccessServer>(callback);
  m_server->AddVariable(m_channel, value);
  m_server->Start();
}

PVAccessServerPVWrapper::~PVAccessServerPVWrapper() = default;

bool PVAccessServerPVWrapper::IsAvailable() const
{
  return true;
}

std::pair<bool, sup::dto::AnyValue> PVAccessServerPVWrapper::GetValue(double timeout_sec) const
{
  (void)timeout_sec;
  return { true, m_server->GetValue(m_channel) };
}

bool PVAccessServerPVWrapper::SetValue(const sup::dto::AnyValue& value, double timeout_sec)
{
  (void)timeout_sec;
  return m_server->SetValue(m_channel, value);
}

bool PVAccessServerPVWrapper::WaitForAvailable(double timeout_sec) const
{
  (void)timeout_sec;
  return true;
}

bool PVAccessServerPVWrapper::SetMonitorCallback(Callback func)
{
  std::lock_guard<std::mutex> lk{m_cb_mtx};
  m_callback = func;
  return true;
}

void PVAccessServerPVWrapper::OnUpdate(const sup::dto::AnyValue& val)
{
  std::lock_guard<std::mutex> lk{m_cb_mtx};
  if (m_callback)
  {
    m_callback(val, true);
  }
}

}  // namespace epics

}  // namespace sup
