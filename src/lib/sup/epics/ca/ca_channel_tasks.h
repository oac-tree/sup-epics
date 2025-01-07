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

#ifndef SUP_EPICS_CA_CHANNEL_TASKS_H_
#define SUP_EPICS_CA_CHANNEL_TASKS_H_

#include <sup/epics/ca/ca_channel_manager.h>
#include <sup/epics/ca/ca_monitor_wrapper.h>

#include <cadef.h>

namespace sup
{
namespace epics
{
namespace channeltasks
{

bool AddChannelTask(const std::string& name, chtype type, chid* id,
                    ConnectionCallBack* connect_cb, CAMonitorWrapper* monitor_cb);

bool RemoveChannelTask(chid id);

bool UpdateChannelTask(chtype type, unsigned long count, chid id, void* ref);

}  // namespace channeltasks

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_CA_CHANNEL_TASKS_H_
