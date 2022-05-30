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

#include <cadef.h>
#include <functional>
#include <string>

namespace sup::epics
{
using ChannelID = void*;

struct MonitorInfo
{
  sup::dto::uint64 timestamp;
  sup::dto::int16 status;
  sup::dto::int16 severity;
  sup::dto::AnyValue value;
};

using ConnectionCallBack = std::function<void(const std::string&,bool)>;
using MonitorCallBack = std::function<void(const std::string&,const MonitorInfo&)>;
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
                       ConnectionCallBack conn_cb, MonitorCallBack mon_cb);

  bool RemoveChannel(ChannelID id);

  bool UpdateChannel(ChannelID id, const sup::dto::AnyValue& value);
private:

};

}  // namespace sup::epics

#endif  // SUP_EPICS_CAChannelManager_H
