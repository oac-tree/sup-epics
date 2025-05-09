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

#ifndef SUP_EPICS_PV_ACCESS_SERVER_PV_WRAPPER_H_
#define SUP_EPICS_PV_ACCESS_SERVER_PV_WRAPPER_H_

#include <sup/epics/pv_access_server.h>

#include <sup/protocol/process_variable.h>

#include <mutex>

namespace sup
{
namespace epics
{

class PVAccessServerPVWrapper : public sup::protocol::ProcessVariable
{
public:
  PVAccessServerPVWrapper(const std::string& channel, const sup::dto::AnyValue& value);
  ~PVAccessServerPVWrapper();

  bool IsAvailable() const override;
  std::pair<bool, sup::dto::AnyValue> GetValue(double timeout_sec) const override;
  bool SetValue(const sup::dto::AnyValue& value, double timeout_sec) override;
  bool WaitForAvailable(double timeout_sec) const override;
  bool SetMonitorCallback(Callback func) override;
private:
  void OnUpdate(const sup::dto::AnyValue& val);
  std::string m_channel;
  sup::protocol::ProcessVariable::Callback m_callback;
  std::mutex m_cb_mtx;
  std::unique_ptr<PvAccessServer> m_server;
};

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_PV_ACCESS_SERVER_PV_WRAPPER_H_
