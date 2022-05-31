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

#include "CAChannelTasks.h"
#include "CAContextHandle.h"
#include "CAHelper.h"
#include "CAMonitorWrapper.h"
#include <sup/dto/AnyValueHelper.h>
#include <cadef.h>
#include <utility>

namespace sup::epics
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
  , context_handle{new CAContextHandle()}
{}

CAChannelManager::~CAChannelManager() = default;

ChannelID CAChannelManager::AddChannel(const std::string& name, const sup::dto::AnyType& type,
                                       ConnectionCallBack&& conn_cb, MonitorCallBack&& mon_cb)
{
  std::lock_guard<std::mutex> lk(mtx);
  auto id = GenerateID();
  auto insert_result = callback_map.emplace(
    std::make_pair(id, ChannelInfo(type, std::move(conn_cb), std::move(mon_cb))));
  auto channel_type = cahelper::ChannelType(type);
  auto channel_info_it = &insert_result.first->second;
  auto add_task = std::packaged_task<bool()>([&name, channel_type, channel_info_it](){
    return channeltasks::AddChannelTask(name, channel_type, &channel_info_it->channel_id,
                                        &channel_info_it->connection_cb,
                                        &channel_info_it->monitor_cb);
  });
  if (!context_handle->HandleTask(std::move(add_task)))
  {
    callback_map.erase(insert_result.first);
    return 0;
  }
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
  auto remove_task = std::packaged_task<bool()>([channel_id](){
    return channeltasks::RemoveChannelTask(channel_id);
  });
  auto result = context_handle->HandleTask(std::move(remove_task));
  callback_map.erase(it);
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

CAChannelManager::ChannelInfo::ChannelInfo(const sup::dto::AnyType& anytype,
                                           ConnectionCallBack&& conn_cb,
                                           MonitorCallBack&& mon_cb)
  : channel_id{nullptr}
  , connection_cb{std::move(conn_cb)}
  , monitor_cb{anytype, std::move(mon_cb)}
{}

}  // namespace sup::epics
