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
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 *****************************************************************************/

#ifndef SUP_EPICS_CA_MONITOR_WRAPPER_H_
#define SUP_EPICS_CA_MONITOR_WRAPPER_H_

#include <sup/epics/ca/ca_channel_manager.h>
#include <sup/dto/anytype.h>
#include <string>

namespace sup
{
namespace epics
{
class CAMonitorWrapper
{
public:
  CAMonitorWrapper(sup::dto::AnyType anytype, MonitorCallBack&& mon_cb);
  void operator()(sup::dto::uint64 timestamp, sup::dto::int16 status,
                  sup::dto::int16 severity, long count, void* ref);
private:
  bool VerifyCount(long count);
  sup::dto::AnyType m_anytype;
  MonitorCallBack m_mon_cb;
};

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_CA_MONITOR_WRAPPER_H_
