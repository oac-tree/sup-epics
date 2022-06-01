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

#ifndef SUP_EPICS_CAContextHandle_H
#define SUP_EPICS_CAContextHandle_H

#include <atomic>
#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>

namespace sup::epics
{

/**
 * @brief CAContextHandle handles a preemptive CA context in a dedicated thread.
 *
 * @note The class creates a CA context in a separate thread and forwards all channel
 * operations to that thread as generic packaged_tasks.
 */
class CAContextHandle
{
public:
  CAContextHandle();
  ~CAContextHandle();

  bool HandleTask(std::packaged_task<bool()>&& task);

private:
  bool LaunchContext();
  void HaltContext();
  void ContextThread(std::promise<bool>& context_promise);
  std::queue<std::packaged_task<bool()>> tasks;
  std::mutex task_mtx;
  std::condition_variable cond;
  std::atomic_bool halt;
  std::future<void> context_future;
};

}  // namespace sup::epics

#endif  // SUP_EPICS_CAContextHandle_H
