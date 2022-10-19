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

#include "sup/epics/pv_client_pv.h"

#include <pvxs/client.h>

namespace sup
{
namespace epics
{

PvClientPV::ExtendedValue::ExtendedValue()
  : connected{false}
  , value{}
{}

PvClientPV::PvClientPV(const std::string& channel, const sup::dto::AnyType& anytype,
                       VariableChangedCallback cb)
    : m_channel_name{channel}
    , m_cache{}
    , m_changed_cb{std::move(cb)}
{
  m_cache.value = sup::dto::AnyValue(anytype);
}

PvClientPV::~PvClientPV() = default;

bool PvClientPV::IsConnected() const
{
  return m_cache.connected;
}

std::string PvClientPV::GetChannelName() const
{
  return m_channel_name;
}

sup::dto::AnyValue PvClientPV::GetValue() const
{
  return m_cache.value;
}

PvClientPV::ExtendedValue PvClientPV::GetExtendedValue() const
{
  return m_cache;
}

bool PvClientPV::SetValue(const sup::dto::AnyValue& value)
{
  (void)value;
  return true;
}

bool PvClientPV::WaitForConnected(double timeout_sec) const
{
  (void)timeout_sec;
  return true;
}

bool PvClientPV::WaitForValidValue(double timeout_sec) const
{
  (void)timeout_sec;
  return true;
}

}  // namespace epics

}  // namespace sup
