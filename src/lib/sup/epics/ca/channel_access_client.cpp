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

#include <sup/epics/channel_access_client.h>

#include <stdexcept>

namespace sup
{
namespace epics
{

ChannelAccessClient::ChannelAccessClient(VariableUpdatedCallback cb)
    : var_updated_cb{std::move(cb)}
    , pv_map{}
{}

ChannelAccessClient::~ChannelAccessClient() = default;

bool ChannelAccessClient::AddVariable(const std::string& channel, const sup::dto::AnyType& type)
{
  if (pv_map.find(channel) != pv_map.end())
  {
    return false;
  }
  std::unique_ptr<ChannelAccessPV> pv;
  try
  {
    pv = std::make_unique<ChannelAccessPV>(
        channel, type, [this, channel](auto&& PH1)
        { OnVariableUpdated(channel, std::forward<decltype(PH1)>(PH1)); });
  }
  catch (const std::runtime_error&)
  {
    return false;
  }
  pv_map.emplace(channel, std::move(pv));
  return true;
}

std::vector<std::string> ChannelAccessClient::GetVariableNames() const
{
  std::vector<std::string> result;
  result.reserve(pv_map.size());
  for (const auto& [name, _] : pv_map)
  {
    result.push_back(name);
  }
  return result;
}

bool ChannelAccessClient::IsConnected(const std::string& channel) const
{
  auto it = pv_map.find(channel);
  if (it == pv_map.end())
  {
    return false;
  }
  return it->second->IsConnected();
}

sup::dto::AnyValue ChannelAccessClient::GetValue(const std::string& channel) const
{
  auto it = pv_map.find(channel);
  if (it == pv_map.end())
  {
    return {};
  }
  return it->second->GetValue();
}

ChannelAccessPV::ExtendedValue ChannelAccessClient::GetExtendedValue(
    const std::string& channel) const
{
  auto it = pv_map.find(channel);
  if (it == pv_map.end())
  {
    return {};
  }
  return it->second->GetExtendedValue();
}

bool ChannelAccessClient::SetValue(const std::string& channel, const sup::dto::AnyValue& value)
{
  auto it = pv_map.find(channel);
  if (it == pv_map.end())
  {
    return false;
  }
  return it->second->SetValue(value);
}

bool ChannelAccessClient::WaitForConnected(const std::string& channel, double timeout_sec) const
{
  auto it = pv_map.find(channel);
  if (it == pv_map.end())
  {
    return false;
  }
  return it->second->WaitForConnected(timeout_sec);
}

bool ChannelAccessClient::WaitForValidValue(const std::string& channel, double timeout_sec) const
{
  auto it = pv_map.find(channel);
  if (it == pv_map.end())
  {
    return false;
  }
  return it->second->WaitForValidValue(timeout_sec);
}

bool ChannelAccessClient::RemoveVariable(const std::string& channel)
{
  auto it = pv_map.find(channel);
  if (it == pv_map.end())
  {
    return false;
  }
  pv_map.erase(it);
  return true;
}

void ChannelAccessClient::OnVariableUpdated(const std::string& channel,
                                            const ChannelAccessPV::ExtendedValue& value)
{
  if (var_updated_cb)
  {
    var_updated_cb(channel, value);
  }
}

}  // namespace epics

}  // namespace sup
