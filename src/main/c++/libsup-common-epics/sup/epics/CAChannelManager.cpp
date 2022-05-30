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

#include "CAChannelManager.h"

namespace
{
void Monitor_CB(event_handler_args args);
void Connection_CB(connection_handler_args args);
}  // unnamed namespace

namespace sup::epics
{

}  // namespace sup::epics

namespace
{
void Monitor_CB(event_handler_args args)
{
  // using namespace ccs::HelperTools::ChannelAccess;
  // auto name = std::string(ca_name(args.chid));
  // IContextManager::MonitorInfo info;
  // info.timestamp = GetTimestampField(args);
  // info.status = GetStatusField(args);
  // info.severity = GetSeverityField(args);
  // info.ref = GetValueFieldReference(args);
  // auto func = reinterpret_cast<std::function<void(const std::string&,
  //                                                 const IContextManager::MonitorInfo&)>*>(args.usr);
  // return (*func)(name, info);
}

void Connection_CB(connection_handler_args args)
{
  auto name = std::string(ca_name(args.chid));
  bool connected = (args.op == CA_OP_CONN_UP);
  auto func = static_cast<std::function<void(const std::string&, bool)>*>(ca_puser(args.chid));
  return (*func)(name, connected);
}

}  // unnamed namespace
