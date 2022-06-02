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

#ifndef SUP_EPICS_CA_CHANNEL_MANAGER_H_
#define SUP_EPICS_CA_CHANNEL_MANAGER_H_

#include <sup/epics/ca_types.h>

#include <sup/dto/anytype.h>
#include <sup/dto/anyvalue.h>

#include <functional>
#include <map>
#include <mutex>
#include <list>
#include <string>

namespace sup
{
namespace epics
{
class CAContextHandle;

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
  ChannelID GenerateID();
  void EnsureContext();
  void ClearContextIfNotNeeded();
  ChannelID last_id;
  struct ChannelInfo;
  std::unique_ptr<CAContextHandle> context_handle;
  std::map<ChannelID, ChannelInfo> callback_map;
  std::mutex mtx;
};

CAChannelManager& SharedCAChannelManager();

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_CA_CHANNEL_MANAGER_H_
