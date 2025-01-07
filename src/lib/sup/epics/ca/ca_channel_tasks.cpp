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

#include <sup/epics/ca/ca_channel_tasks.h>

#include <sup/epics/ca/ca_channel_manager.h>
#include <sup/epics/ca/ca_helper.h>

namespace
{
void Monitor_CB(event_handler_args args);
void Connection_CB(connection_handler_args args);
}  // unnamed namespace

namespace sup
{
namespace epics
{
namespace channeltasks
{

bool AddChannelTask(const std::string& name, chtype type, chid* id,
                    ConnectionCallBack* connect_cb, CAMonitorWrapper* monitor_cb)
{
  if (ca_create_channel(name.c_str(), &Connection_CB, connect_cb, 10, id) != ECA_NORMAL)
  {
    return false;
  }
  if (monitor_cb != nullptr)
  {
    if (ca_create_subscription(type + 14, 0, *id, DBE_VALUE | DBE_ALARM, &Monitor_CB, monitor_cb,
                               nullptr)
        != ECA_NORMAL)
    {
      return false;
    }
  }
  ca_flush_io();
  return true;
}

bool RemoveChannelTask(chid id)
{
  if (ca_clear_channel(id) != ECA_NORMAL)
  {
    return false;
  }
  ca_flush_io();
  return true;
}

bool UpdateChannelTask(chtype type, unsigned long count, chid id, void* ref)
{
  if (ca_array_put(type, count, id, ref) != ECA_NORMAL)
  {
    return false;
  }
  ca_flush_io();
  return true;
}

}  // namespace channeltasks

}  // namespace epics

}  // namespace sup

namespace
{
void Monitor_CB(event_handler_args args)
{
  using namespace sup::epics::cahelper;
  auto timestamp = GetTimestampField(args);
  auto status = GetStatusField(args);
  auto severity = GetSeverityField(args);
  auto ref = GetValueFieldReference(args);
  auto count = args.count;
  auto func = static_cast<sup::epics::CAMonitorWrapper*>(args.usr);
  return (*func)(timestamp, status, severity, count, ref);
}

void Connection_CB(connection_handler_args args)
{
  bool connected = (args.op == CA_OP_CONN_UP);
  auto func = static_cast<sup::epics::ConnectionCallBack*>(ca_puser(args.chid));
  return (*func)(connected);
}

}  // unnamed namespace
