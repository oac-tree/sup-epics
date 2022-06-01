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

#include "sup/epics/ca_monitor_wrapper.h"

#include <sup/dto/AnyValueHelper.h>
#include <string.h>

namespace
{
sup::dto::AnyValue AnyValueFromMonitorRef(const sup::dto::AnyType& anytype, void* ref,
                                          std::size_t size);
}  // unnamed namespace

namespace sup
{
namespace epics
{
CAMonitorWrapper::CAMonitorWrapper(sup::dto::AnyType anytype_, MonitorCallBack&& mon_cb_)
  : anytype{std::move(anytype_)}
  , size{0}
  , mon_cb{std::move(mon_cb_)}
{
  sup::dto::AnyValue value(anytype);
  auto bytes = sup::dto::ToBytes(value);
  size = bytes.size();
}

void CAMonitorWrapper::operator()(const std::string& name, sup::dto::uint64 timestamp,
                                  sup::dto::int16 status, sup::dto::int16 severity, void* ref)
{
  CAMonitorInfo info;
  info.timestamp = timestamp;
  info.status = status;
  info.severity = severity;
  info.value = AnyValueFromMonitorRef(anytype, ref, size);
  return mon_cb(name, info);
}

}  // namespace epics

}  // namespace sup

namespace
{
sup::dto::AnyValue AnyValueFromMonitorRef(const sup::dto::AnyType& anytype, void* ref,
                                          std::size_t size)
{
  sup::dto::AnyValue result(anytype);
  if (anytype.GetTypeCode() == sup::dto::TypeCode::String)
  {
    const std::size_t kEpicsStringLength{40};
    char buffer[kEpicsStringLength+1];
    buffer[kEpicsStringLength] = 0;
    strncpy(buffer, (const char*)ref, kEpicsStringLength);
    result = std::string(buffer);
  }
  else
  {
    sup::dto::FromBytes(result, (const sup::dto::uint8*)ref, size);
  }
  return result;
}
}  // unnamed namespace
