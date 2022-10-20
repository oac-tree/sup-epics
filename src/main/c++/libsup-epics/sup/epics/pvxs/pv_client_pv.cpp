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
#include <sup/epics/pv_client_pv.h>

#include "pv_access_client_pv_impl.h"

namespace
{
std::shared_ptr<pvxs::client::Context> GetSharedClientContext();
}  // unnamed namespace

namespace sup
{
namespace epics
{

PvClientPV::ExtendedValue::ExtendedValue()
  : connected{false}
  , value{}
{}

PvClientPV::PvClientPV(const std::string& channel, VariableChangedCallback cb)
  : m_impl{new PvAccessClientPVImpl(channel, GetSharedClientContext(), cb)}
{}

PvClientPV::PvClientPV(std::unique_ptr<PvAccessClientPVImpl>&& impl)
  : m_impl{std::move(impl)}
{}

PvClientPV::~PvClientPV() = default;

bool PvClientPV::IsConnected() const
{
  return m_impl->IsConnected();
}

std::string PvClientPV::GetChannelName() const
{
  return m_impl->GetChannelName();
}

sup::dto::AnyValue PvClientPV::GetValue() const
{
  return m_impl->GetValue();
}

PvClientPV::ExtendedValue PvClientPV::GetExtendedValue() const
{
  return m_impl->GetExtendedValue();
}

bool PvClientPV::SetValue(const sup::dto::AnyValue& value)
{
  return m_impl->SetValue(value);
}

bool PvClientPV::WaitForConnected(double timeout_sec) const
{
  return m_impl->WaitForConnected(timeout_sec);
}

bool PvClientPV::WaitForValidValue(double timeout_sec) const
{
  return m_impl->WaitForValidValue(timeout_sec);
}

}  // namespace epics

}  // namespace sup

namespace
{
std::shared_ptr<pvxs::client::Context> GetSharedClientContext()
{
  static std::shared_ptr<pvxs::client::Context> context =
    std::make_shared<pvxs::client::Context>(pvxs::client::Context::fromEnv());
  return context;
}

}  // unnamed namespace
