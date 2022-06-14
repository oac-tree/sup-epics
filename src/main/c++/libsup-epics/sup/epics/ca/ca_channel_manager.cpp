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

#include <sup/epics/ca/ca_channel_manager.h>

#include <sup/epics/ca/ca_channel_tasks.h>
#include <sup/epics/ca/ca_context_handle.h>
#include <sup/epics/ca/ca_helper.h>
#include <sup/epics/ca/ca_monitor_wrapper.h>

#include <sup/dto/anyvalue_helper.h>
#include <cadef.h>
#include <utility>

namespace
{
bool DelegateRemoveChannel(sup::epics::CAContextHandle* context, chid id);
}  // unnamed namespace

namespace sup
{
namespace epics
{
struct CAChannelManager::ChannelInfo
{
  ChannelInfo(const sup::dto::AnyType& anytype, ConnectionCallBack&& conn_cb,
              MonitorCallBack&& mon_cb);
  chid channel_id;
  ConnectionCallBack connection_cb;
  CAMonitorWrapper monitor_cb;
};

CAChannelManager::CAChannelManager()
  : last_id{0}
  , context_handle{}
  , callback_map{}
  , mtx{}
{}

CAChannelManager::~CAChannelManager() = default;

ChannelID CAChannelManager::AddChannel(const std::string& name, const sup::dto::AnyType& type,
                                       ConnectionCallBack&& conn_cb, MonitorCallBack&& mon_cb)
{
  auto channel_type = cahelper::ChannelType(type);
  if (channel_type < 0)
  {
    return 0;
  }
  std::lock_guard<std::mutex> lk(mtx);
  EnsureContext();
  auto id = GenerateID();
  auto insert_result = callback_map.emplace(
    std::make_pair(id, ChannelInfo(type, std::move(conn_cb), std::move(mon_cb))));
  auto channel_info_it = &insert_result.first->second;
  auto add_task = std::packaged_task<bool()>([&name, channel_type, channel_info_it](){
    return channeltasks::AddChannelTask(name, channel_type, &channel_info_it->channel_id,
                                        &channel_info_it->connection_cb,
                                        &channel_info_it->monitor_cb);
  });
  if (!context_handle->HandleTask(std::move(add_task)))
  {
    if (channel_info_it->channel_id != nullptr)
    {
      DelegateRemoveChannel(context_handle.get(), channel_info_it->channel_id);
    }
    callback_map.erase(insert_result.first);
    id = 0;
  }
  ClearContextIfNotNeeded();
  return id;
}

bool CAChannelManager::RemoveChannel(ChannelID id)
{
  std::lock_guard<std::mutex> lk(mtx);
  auto it = callback_map.find(id);
  if (it == callback_map.end())
  {
    return false;
  }
  chid channel_id = it->second.channel_id;
  auto result = DelegateRemoveChannel(context_handle.get(), channel_id);
  callback_map.erase(it);
  ClearContextIfNotNeeded();
  return result;
}

bool CAChannelManager::UpdateChannel(ChannelID id, const sup::dto::AnyValue& value)
{
  std::lock_guard<std::mutex> lk(mtx);
  auto it = callback_map.find(id);
  if (it == callback_map.end())
  {
    return false;
  }
  auto anytype = value.GetType();
  auto type = cahelper::ChannelType(anytype);
  if (type == -1)
  {
    return false;
  }
  auto count = cahelper::ChannelMultiplicity(anytype);
  auto channel_id = it->second.channel_id;
  auto byte_representation = sup::dto::ToBytes(value);
  auto ref = byte_representation.data();
  auto update_task = std::packaged_task<bool()>([type, count, channel_id, ref](){
    return channeltasks::UpdateChannelTask(type, count, channel_id, ref);
  });
  return context_handle->HandleTask(std::move(update_task));
}

ChannelID CAChannelManager::GenerateID()
{
  while (callback_map.find(++last_id) != callback_map.end()) {}
  return last_id;
}

void CAChannelManager::EnsureContext()
{
  if (!context_handle)
  {
    context_handle.reset(new CAContextHandle());
  }
}

void CAChannelManager::ClearContextIfNotNeeded()
{
  if (context_handle && callback_map.empty())
  {
    context_handle.reset();
  }
}

CAChannelManager& SharedCAChannelManager()
{
  static CAChannelManager channel_manager;
  return channel_manager;
}

CAChannelManager::ChannelInfo::ChannelInfo(const sup::dto::AnyType& anytype,
                                           ConnectionCallBack&& conn_cb,
                                           MonitorCallBack&& mon_cb)
  : channel_id{nullptr}
  , connection_cb{std::move(conn_cb)}
  , monitor_cb{anytype, std::move(mon_cb)}
{}

}  // namespace epics

}  // namespace sup

namespace
{
bool DelegateRemoveChannel(sup::epics::CAContextHandle* context, chid id)
{
  auto remove_task = std::packaged_task<bool()>([id](){
    return sup::epics::channeltasks::RemoveChannelTask(id);
  });
  return context->HandleTask(std::move(remove_task));
}
}  // unnamed namespace
