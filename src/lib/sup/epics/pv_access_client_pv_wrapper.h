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

#ifndef SUP_EPICS_PV_ACCESS_CLIENT_PV_WRAPPER_H_
#define SUP_EPICS_PV_ACCESS_CLIENT_PV_WRAPPER_H_

#include <sup/epics/pv_access_client_pv.h>

#include <sup/protocol/process_variable.h>

#include <mutex>

namespace sup
{
namespace epics
{

class PVAccessClientPVWrapper : public sup::protocol::ProcessVariable
{
public:
  PVAccessClientPVWrapper(const std::string& channel);
  ~PVAccessClientPVWrapper();

  bool IsAvailable() const override;
  std::pair<bool, sup::dto::AnyValue> GetValue(double timeout_sec) const override;
  bool SetValue(const sup::dto::AnyValue& value, double timeout_sec) override;
  bool WaitForAvailable(double timeout_sec) const override;
  bool SetMonitorCallback(Callback func) override;
private:
  void OnUpdate(const PvAccessClientPV::ExtendedValue& val);
  sup::protocol::ProcessVariable::Callback m_callback;
  std::mutex m_cb_mtx;
  std::unique_ptr<PvAccessClientPV> m_pv_impl;
};

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_PV_ACCESS_CLIENT_PV_WRAPPER_H_
