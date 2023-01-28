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

#include "pv_access_client_impl.h"
#include "pv_access_utils.h"

namespace sup
{
namespace epics
{

PvAccessClient::PvAccessClient(VariableChangedCallback cb)
  : m_impl{new PvAccessClientImpl(utils::GetSharedClientContext(), cb)}
{}

PvAccessClient::PvAccessClient(std::unique_ptr<PvAccessClientImpl>&& impl)
  : m_impl{std::move(impl)}
{}

PvAccessClient::~PvAccessClient() = default;

PvAccessClient::PvAccessClient(PvAccessClient&& other)
  : m_impl{std::move(other.m_impl)}
{}

PvAccessClient& PvAccessClient::operator=(PvAccessClient&& other)
{
  if (this != &other)
  {
    std::swap(m_impl, other.m_impl);
  }
  return *this;
}

void PvAccessClient::AddVariable(const std::string& channel)
{
  m_impl->AddVariable(channel);
}

std::vector<std::string> PvAccessClient::GetVariableNames() const
{
  return m_impl->GetVariableNames();
}

bool PvAccessClient::IsConnected(const std::string& channel) const
{
  auto& var_map = m_impl->GetVariables();
  auto it = var_map.find(channel);
  return it == var_map.end() ? false : it->second->IsConnected();
}

dto::AnyValue PvAccessClient::GetValue(const std::string& channel) const
{
  auto& var_map = m_impl->GetVariables();
  auto it = var_map.find(channel);
  if (it == var_map.end())
  {
    throw std::runtime_error("Error in PvAccessClient: non-existing variable name '" + channel + "'.");
  }
  return it->second->GetValue();
}

PvAccessClientPV::ExtendedValue PvAccessClient::GetExtendedValue(const std::string& channel) const
{
  auto& var_map = m_impl->GetVariables();
  auto it = var_map.find(channel);
  if (it == var_map.end())
  {
    throw std::runtime_error("Error in PvAccessClient: non-existing variable name '" + channel + "'.");
  }
  return it->second->GetExtendedValue();
}

bool PvAccessClient::SetValue(const std::string& channel, const dto::AnyValue& value)
{
  auto& var_map = m_impl->GetVariables();
  auto it = var_map.find(channel);
  if (it == var_map.end())
  {
    throw std::runtime_error("Error in PvAccessClient: non-existing variable name '" +
                             channel + "'.");
  }
  return it->second->SetValue(value);
}

bool PvAccessClient::WaitForConnected(const std::string& channel, double timeout_sec) const
{
  auto& var_map = m_impl->GetVariables();
  auto it = var_map.find(channel);
  if (it == var_map.end())
  {
    throw std::runtime_error("Error in PvAccessClient: non-existing variable name '" +
                             channel + "'.");
  }
  return it->second->WaitForConnected(timeout_sec);
}

bool PvAccessClient::WaitForValidValue(const std::string& channel, double timeout_sec) const
{
  auto& var_map = m_impl->GetVariables();
  auto it = var_map.find(channel);
  if (it == var_map.end())
  {
    throw std::runtime_error("Error in PvAccessClient: non-existing variable name '" +
                             channel + "'.");
  }
  return it->second->WaitForValidValue(timeout_sec);
}

}  // namespace epics

}  // namespace sup
