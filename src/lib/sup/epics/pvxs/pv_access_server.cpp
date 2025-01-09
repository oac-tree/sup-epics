/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Gennady Pospelov (IO)
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

#include "sup/epics/pv_access_server.h"

#include <sup/epics/pvxs/pv_access_client_impl.h>
#include <sup/epics/pvxs/pv_access_client_pv_impl.h>
#include <sup/epics/pvxs/pv_access_server_pv.h>
#include <sup/epics/pvxs/pv_access_server_impl.h>

#include <pvxs/server.h>

#include <map>
#include <memory>
#include <stdexcept>
#include <sup/dto/anyvalue.h>

namespace sup
{
namespace epics
{
PvAccessServer::IsolatedTag PvAccessServer::Isolated{};

PvAccessServer::PvAccessServer(VariableChangedCallback callback)
  : p_impl{CreateServerImplFromEnv(std::move(callback))}
{}

PvAccessServer::PvAccessServer(IsolatedTag, VariableChangedCallback callback)
  : p_impl{CreateIsolatedServerImpl(std::move(callback))}
{}

PvAccessServer::~PvAccessServer() = default;

void PvAccessServer::AddVariable(const std::string& name, const dto::AnyValue& any_value)
{
  p_impl->AddVariable(name, any_value);
}

std::vector<std::string> PvAccessServer::GetVariableNames() const
{
  return p_impl->GetVariableNames();
}

dto::AnyValue PvAccessServer::GetValue(const std::string &name) const
{
  auto iter = p_impl->GetVariables().find(name);
  if (iter == p_impl->GetVariables().end())
  {
    throw std::runtime_error("Error in PvAccessServer: non-existing variable name '" + name + "'.");
  }
  return iter->second->GetValue();
}

bool PvAccessServer::SetValue(const std::string &name, const dto::AnyValue &value)
{
  auto iter = p_impl->GetVariables().find(name);
  if (iter == p_impl->GetVariables().end())
  {
    throw std::runtime_error("Error in PvAccessServer: non-existing variable name '" + name + "'.");
  }
  return iter->second->SetValue(value);
}

void PvAccessServer::Start()
{
  for(const auto& [_, variable] : p_impl->GetVariables())
  {
    variable->AddToServer(*p_impl->GetContext());
  }

  // starting PVXS server
  p_impl->GetContext()->start();
}

PvAccessClient PvAccessServer::CreateClient(PvAccessClient::VariableChangedCallback cb)
{
  auto client_impl = std::make_unique<sup::epics::PvAccessClientImpl>(p_impl->GetClientContext(), cb);
  return PvAccessClient{std::move(client_impl)};
}

PvAccessClientPV PvAccessServer::CreateClientPV(const std::string& channel,
                                                PvAccessClientPV::VariableChangedCallback cb)
{
  auto client_pv_impl = std::make_unique<sup::epics::PvAccessClientPVImpl>(channel, p_impl->GetClientContext(), cb);
  return PvAccessClientPV{std::move(client_pv_impl)};
}

}  // namespace epics

}  // namespace sup
