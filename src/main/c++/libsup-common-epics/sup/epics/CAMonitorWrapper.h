/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP common components for EPICS network protocol
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

#ifndef SUP_EPICS_CAMonitorWrapper_H
#define SUP_EPICS_CAMonitorWrapper_H

#include "CAChannelManager.h"
#include <sup/dto/AnyType.h>
#include <string>

namespace sup::epics
{
class CAMonitorWrapper
{
public:
  CAMonitorWrapper(sup::dto::AnyType anytype, MonitorCallBack&& mon_cb);
  void operator()(const std::string& name, sup::dto::uint64 timestamp, sup::dto::int16 status,
                  sup::dto::int16 severity, void* ref);
private:
  sup::dto::AnyType anytype;
  std::size_t size;
  MonitorCallBack mon_cb;
};

}  // namespace sup::epics

#endif  // SUP_EPICS_CAMonitorWrapper_H
