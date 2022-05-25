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

#ifndef SUP_EPICS_CAChannelManager_H
#define SUP_EPICS_CAChannelManager_H

#include <atomic>
#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>

namespace sup::epics
{

/**
 * @brief CAChannelManager manages a collection of channels in an owned context.
 *
 * @note
 */
class CAChannelManager
{
public:
  CAChannelManager();
  ~CAChannelManager();


private:

};

}  // namespace sup::epics

#endif  // SUP_EPICS_CAChannelManager_H
