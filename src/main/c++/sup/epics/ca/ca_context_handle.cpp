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

#include <sup/epics/ca/ca_context_handle.h>

#include <cadef.h>
#include <functional>

namespace sup
{
namespace epics
{

CAContextHandle::CAContextHandle()
  : tasks{}
  , task_mtx{}
  , cond{}
  , halt{false}
  , context_future{}
{
  if (!LaunchContext())
  {
    throw std::runtime_error("EPICS Channel Access context could not be established");
  }
}

CAContextHandle::~CAContextHandle()
{
  HaltContext();
}

bool CAContextHandle::HandleTask(std::packaged_task<bool()>&& task)
{
  auto result = task.get_future();
  {
    std::lock_guard<std::mutex> lk(task_mtx);
    tasks.push(std::move(task));
  }
  cond.notify_one();
  return result.get();
}

bool CAContextHandle::LaunchContext()
{
  std::promise<bool> context_promise;
  context_future = std::async(std::launch::async, &CAContextHandle::ContextThread, this,
                              std::ref(context_promise));
  if (!context_promise.get_future().get())
  {
    context_future.get();
    return false;
  }
  return true;
}

void CAContextHandle::HaltContext()
{
  if (!context_future.valid())
  {
    return;
  }
  {
    std::lock_guard<std::mutex> lk(task_mtx);
    halt.store(true);
  }
  cond.notify_one();
  context_future.get();
}

void CAContextHandle::ContextThread(std::promise<bool>& context_promise)
{
  // Create preemptive CA context
  if (ca_context_create(ca_enable_preemptive_callback) != ECA_NORMAL)
  {
    context_promise.set_value(false);
    return;
  }
  context_promise.set_value(true);
  std::unique_lock<std::mutex> lk(task_mtx);
  // Cache the halt state to ensure that queue tasks pushed before halting are always handled
  // by this thread
  bool halt_cache = halt.load();
  while(!halt_cache)
  {
    cond.wait(lk, [this](){ return halt || !tasks.empty(); });
    std::queue<std::packaged_task<bool()>> task_queue;
    tasks.swap(task_queue);
    halt_cache = halt.load();
    lk.unlock();
    while (!task_queue.empty())
    {
      auto& task = task_queue.front();
      task();
      task_queue.pop();
    }
    lk.lock();
  }
  ca_context_destroy();
}

}  // namespace epics

}  // namespace sup
