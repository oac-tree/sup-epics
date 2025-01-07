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
std::vector<sup::dto::uint8> GetUpdateBuffer(const sup::dto::AnyValue& value,
                                             const sup::dto::AnyType& dest_type);
std::vector<sup::dto::uint8> GetStringsUpdateBuffer(const sup::dto::AnyValue& value,
                                                    unsigned long multiplicity);
std::vector<sup::dto::uint8> GetStringUpdateBuffer(const sup::dto::AnyValue& value);
}  // unnamed namespace

namespace sup
{
namespace epics
{
struct CAChannelManager::ChannelInfo
{
  ChannelInfo(const sup::dto::AnyType& anytype, ConnectionCallBack&& conn_cb,
              MonitorCallBack&& mon_cb);
  sup::dto::AnyType channel_anytype;
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
  auto dest_type = it->second.channel_anytype;
  auto type = cahelper::ChannelType(dest_type);
  if (type == -1)
  {
    return false;
  }
  auto count = cahelper::ChannelMultiplicity(dest_type);
  auto channel_id = it->second.channel_id;
  auto byte_rep = GetUpdateBuffer(value, dest_type);
  if (byte_rep.size() == 0)
  {
    return false;
  }
  auto ref = byte_rep.data();
  auto update_task = std::packaged_task<bool()>([type, count, channel_id, ref](){
    return channeltasks::UpdateChannelTask(type, count, channel_id, ref);
  });
  return context_handle->HandleTask(std::move(update_task));
}

ChannelID CAChannelManager::GenerateID()
{
  while (callback_map.find(++last_id) != callback_map.end())
  {}
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
  : channel_anytype{anytype}
  , channel_id{nullptr}
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

std::vector<sup::dto::uint8> GetUpdateBuffer(const sup::dto::AnyValue& value,
                                             const sup::dto::AnyType& dest_type)
{
  using namespace sup::epics::cahelper;
  sup::dto::AnyValue dest_val{dest_type};
  if (!sup::dto::TryConvert(dest_val, value))
  {
    return {};
  }
  auto multiplicity = ChannelMultiplicity(dest_type);
  auto ch_type = ChannelType(dest_type);
  if (ch_type == DBR_STRING)
  {
    return GetStringsUpdateBuffer(dest_val, multiplicity);
  }
  return sup::dto::ToBytes(dest_val);
}

std::vector<sup::dto::uint8> GetStringsUpdateBuffer(const sup::dto::AnyValue& value,
                                                    unsigned long multiplicity)
{
  if (multiplicity > 1)
  {
    std::vector<sup::dto::uint8> result;
    for (unsigned long idx = 0; idx < multiplicity; ++idx)
    {
      auto el_buffer = GetStringUpdateBuffer(value[idx]);
      result.insert(result.end(), el_buffer.begin(), el_buffer.end());
    }
    return result;
  }
  return GetStringUpdateBuffer(value);
}

std::vector<sup::dto::uint8> GetStringUpdateBuffer(const sup::dto::AnyValue& value)
{
  const std::size_t kEpicsStringLength = dbr_size[DBR_STRING];
  std::vector<sup::dto::uint8> result(kEpicsStringLength);
  std::string str;
  if (value.GetType() == sup::dto::StringType)
  {
    str = value.As<std::string>();
  }
  else
  {
    str = sup::dto::ValuesToJSONString(value);
  }
  strncpy((char*)result.data(), str.c_str(), std::min(kEpicsStringLength, str.size()));
  return result;
}

}  // unnamed namespace
