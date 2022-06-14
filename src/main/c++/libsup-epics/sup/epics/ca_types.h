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

#ifndef SUP_EPICS_CA_TYPES_H_
#define SUP_EPICS_CA_TYPES_H_

#include <sup/dto/anyvalue.h>
#include <functional>
#include <string>

namespace sup
{
namespace epics
{

using ChannelID = sup::dto::uint64;

struct CAMonitorInfo
{
  sup::dto::uint64 timestamp;
  sup::dto::int16 status;
  sup::dto::int16 severity;
  sup::dto::AnyValue value;
};

using ConnectionCallBack = std::function<void(bool)>;
using MonitorCallBack = std::function<void(const CAMonitorInfo&)>;

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_CA_TYPES_H_
