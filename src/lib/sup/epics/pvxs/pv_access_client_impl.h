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

#ifndef SUP_EPICS_PV_ACCESS_CLIENT_IMPL_H_
#define SUP_EPICS_PV_ACCESS_CLIENT_IMPL_H_

#include <sup/epics/pv_access_client.h>
#include <sup/epics/pv_access_client_pv.h>
#include <sup/dto/anyvalue.h>

#include <pvxs/client.h>

#include <map>
#include <string>
#include <vector>

namespace sup
{
namespace epics
{

class PvAccessClientImpl
{
public:
  PvAccessClientImpl(std::shared_ptr<pvxs::client::Context> context,
                     PvAccessClient::VariableChangedCallback cb);

  ~PvAccessClientImpl();

  void AddVariable(const std::string& channel);

  std::vector<std::string> GetVariableNames() const;

  const std::map<std::string, std::unique_ptr<PvAccessClientPV>>& GetVariables() const;

private:
  void OnVariableChanged(const std::string& channel, const PvAccessClientPV::ExtendedValue& value);
  std::map<std::string, std::unique_ptr<PvAccessClientPV>> m_variables;
  std::shared_ptr<pvxs::client::Context> m_context;
  PvAccessClient::VariableChangedCallback m_cb;
};

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_PV_ACCESS_CLIENT_IMPL_H_
