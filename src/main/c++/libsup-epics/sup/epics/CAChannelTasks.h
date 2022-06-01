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

#ifndef SUP_EPICS_CAChannelTasks_H
#define SUP_EPICS_CAChannelTasks_H

#include "CAChannelManager.h"
#include "CAMonitorWrapper.h"

#include <cadef.h>

namespace sup::epics::channeltasks
{
bool AddChannelTask(const std::string& name, chtype type, chid* id,
                    ConnectionCallBack* connect_cb, CAMonitorWrapper* monitor_cb);

bool RemoveChannelTask(chid id);

bool UpdateChannelTask(chtype type, unsigned long count, chid id, void* ref);

}  // namespace sup::epics::channeltasks

#endif  // SUP_EPICS_CAChannelTasks_H
