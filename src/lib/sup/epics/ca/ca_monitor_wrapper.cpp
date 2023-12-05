/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Walter Van Herck (IO)
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

#include <sup/epics/ca/ca_monitor_wrapper.h>
#include <sup/epics/ca/ca_helper.h>

#include <sup/dto/anyvalue_helper.h>
#include <string.h>

namespace sup
{
namespace epics
{
CAMonitorWrapper::CAMonitorWrapper(sup::dto::AnyType anytype, MonitorCallBack&& mon_cb)
  : m_anytype{std::move(anytype)}
  , m_size{0}
  , m_mon_cb{std::move(mon_cb)}
{
  sup::dto::AnyValue value(m_anytype);
  auto bytes = sup::dto::ToBytes(value);
  m_size = bytes.size();
}

void CAMonitorWrapper::operator()(sup::dto::uint64 timestamp, sup::dto::int16 status,
                                  sup::dto::int16 severity, void* ref)
{
  CAMonitorInfo info;
  info.timestamp = timestamp;
  info.status = status;
  info.severity = severity;
  if (ref)  // Only dereference ref during success
  {
    info.value = cahelper::ParseAnyValue(m_anytype, ref);
    // info.value = AnyValueFromMonitorRef(m_anytype, ref, m_size);
  }
  return m_mon_cb(info);
}

}  // namespace epics

}  // namespace sup

