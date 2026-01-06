/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Gennady Pospelov (IO)
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
#include <sup/epics/pv_access_client_pv.h>

#include "pv_access_client_pv_impl.h"
#include "pv_access_utils.h"

namespace sup
{
namespace epics
{

PvAccessClientPV::ExtendedValue::ExtendedValue()
  : connected{false}
  , value{}
{}

PvAccessClientPV::PvAccessClientPV(const std::string& channel, VariableChangedCallback cb)
  : m_impl{std::make_unique<PvAccessClientPVImpl>(channel, utils::GetSharedClientContext(), cb)}
{}

PvAccessClientPV::PvAccessClientPV(std::unique_ptr<PvAccessClientPVImpl>&& impl)
  : m_impl{std::move(impl)}
{}

PvAccessClientPV::~PvAccessClientPV() = default;

PvAccessClientPV::PvAccessClientPV(PvAccessClientPV&& other) noexcept
  : m_impl{std::move(other.m_impl)}
{}

PvAccessClientPV& PvAccessClientPV::operator=(PvAccessClientPV&& other) noexcept
{
  if (this != &other)
  {
    std::swap(m_impl, other.m_impl);
  }
  return *this;
}

bool PvAccessClientPV::IsConnected() const
{
  return m_impl->IsConnected();
}

std::string PvAccessClientPV::GetChannelName() const
{
  return m_impl->GetChannelName();
}

sup::dto::AnyValue PvAccessClientPV::GetValue() const
{
  return m_impl->GetValue();
}

PvAccessClientPV::ExtendedValue PvAccessClientPV::GetExtendedValue() const
{
  return m_impl->GetExtendedValue();
}

bool PvAccessClientPV::SetValue(const sup::dto::AnyValue& value)
{
  return m_impl->SetValue(value);
}

bool PvAccessClientPV::WaitForConnected(double timeout_sec) const
{
  return m_impl->WaitForConnected(timeout_sec);
}

bool PvAccessClientPV::WaitForValidValue(double timeout_sec) const
{
  return m_impl->WaitForValidValue(timeout_sec);
}

bool operator==(const PvAccessClientPV::ExtendedValue& lhs, const PvAccessClientPV::ExtendedValue& rhs)
{
  return lhs.connected == rhs.connected && lhs.value == rhs.value;
}

bool operator!=(const PvAccessClientPV::ExtendedValue& lhs, const PvAccessClientPV::ExtendedValue& rhs)
{
  return !(lhs == rhs);
}

}  // namespace epics

}  // namespace sup
