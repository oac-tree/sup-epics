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

#include <sup/dto/AnyType.h>
#include <sup/dto/AnyValue.h>

#include <functional>
#include <map>
#include <mutex>
#include <list>
#include <string>

namespace sup::epics
{
class CAContextHandle;
using ChannelID = sup::dto::uint64;

struct CAMonitorInfo
{
  sup::dto::uint64 timestamp;
  sup::dto::int16 status;
  sup::dto::int16 severity;
  void* ref;
};

using ConnectionCallBack = std::function<void(const std::string&,bool)>;
using MonitorCallBack = std::function<void(const std::string&,const CAMonitorInfo&)>;
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

  ChannelID AddChannel(const std::string& name, const sup::dto::AnyType& type,
                       ConnectionCallBack&& conn_cb, MonitorCallBack&& mon_cb);

  bool RemoveChannel(ChannelID id);

  bool UpdateChannel(ChannelID id, const sup::dto::AnyValue& value);
private:
  ChannelID last_id;
  ChannelID GenerateID();
  struct ChannelInfo;
  std::unique_ptr<CAContextHandle> context_handle;
  std::map<ChannelID, ChannelInfo> callback_map;
  std::mutex mtx;
};

}  // namespace sup::epics

#endif  // SUP_EPICS_CAChannelManager_H
