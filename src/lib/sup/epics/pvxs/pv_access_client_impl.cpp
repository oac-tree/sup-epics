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

#include "pv_access_client_impl.h"

#include "pv_access_client_pv_impl.h"

namespace sup
{
namespace epics
{

PvAccessClientImpl::PvAccessClientImpl(std::shared_ptr<pvxs::client::Context> context,
                                       PvAccessClient::VariableChangedCallback cb)
  : m_cb{cb}
  , m_context{context}
  , m_variables{}
{}

PvAccessClientImpl::~PvAccessClientImpl() = default;

//! Adds channel with given name to the map of channels.
void PvAccessClientImpl::AddVariable(const std::string& channel)
{
  auto iter = m_variables.find(channel);
  if (iter != m_variables.end())
  {
    throw std::runtime_error("Error in PvAccessClientImpl: existing variable name '" + channel + "'.");
  }

  auto cb = [this, channel](const PvAccessClientPV::ExtendedValue& value) {
      OnVariableChanged(channel, value);
  };
  auto pv_impl = std::make_unique<sup::epics::PvAccessClientPVImpl>(channel, m_context, cb);
  m_variables.emplace(channel, std::make_unique<sup::epics::PvAccessClientPV>(std::move(pv_impl)));
}

std::vector<std::string> PvAccessClientImpl::GetVariableNames() const
{
  std::vector<std::string> result;
  std::transform(std::begin(m_variables), end(m_variables), back_inserter(result),
                 [](const auto& pair) { return pair.first; });
  return result;
}

const std::map<std::string, std::unique_ptr<PvAccessClientPV>>& PvAccessClientImpl::GetVariables() const
{
  return m_variables;
}

void PvAccessClientImpl::OnVariableChanged(const std::string& channel,
                                           const PvAccessClientPV::ExtendedValue& value)
{
  if (m_cb)
  {
    m_cb(channel, value);
  }
}

}  // namespace epics

}  // namespace sup
