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

#ifndef SUP_EPICS_ChannelAccessVariable_H
#define SUP_EPICS_ChannelAccessVariable_H

#include "CATypes.h"

#include <mutex>

namespace sup::epics
{
class ChannelAccessVariable
{
public:
  struct ExtendedValue
  {
    ExtendedValue();
    bool connected;
    sup::dto::uint64 timestamp;
    sup::dto::int16 status;
    sup::dto::int16 severity;
    sup::dto::AnyValue value;
  };
  ChannelAccessVariable(const std::string& name, const sup::dto::AnyType& type);
  ~ChannelAccessVariable();

  bool IsConnected() const;
  sup::dto::AnyValue GetValue() const;
  ExtendedValue GetExtendedValue() const;

  bool SetValue(const sup::dto::AnyValue& value);

  bool SetCallBack(std::function<void(const std::string&, const sup::dto::AnyValue&)> cb);
private:
  void OnConnectionChanged(const std::string& name, bool connected);
  void OnMonitorCalled(const std::string& name,const CAMonitorInfo& info);
  ExtendedValue cache;
  ChannelID id;
  mutable std::mutex mon_mtx;
};
}  // namespace sup::epics

#endif  // SUP_EPICS_ChannelAccessVariable_H
