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

#include "CAHelper.h"

#include <cadef.h>

namespace
{
sup::dto::uint64 ToAbsoluteTime_ns(sup::dto::uint64 seconds, sup::dto::uint64 nanoseconds);
}  // unnamed namespace

namespace sup::epics::cahelper
{
void* GetValueFieldReference(event_handler_args args)
{
  if (args.status != ECA_NORMAL)
  {
    return nullptr;
  }
  void* ref = dbr_value_ptr(args.dbr, args.type);
  return ref;
}

sup::dto::int16 GetStatusField(event_handler_args args)
{
  if (args.status != ECA_NORMAL)
  {
    return 0;
  }
  sup::dto::int16 ret = *((sup::dto::int16 *)(((char *)args.dbr) + 0 * sizeof(dbr_short_t)));
  return ret;
}

sup::dto::int16 GetSeverityField(event_handler_args args)
{
  if (args.status != ECA_NORMAL)
  {
    return 0;
  }
  sup::dto::int16 ret = *((sup::dto::int16 *)(((char *)args.dbr) + 1 * sizeof(dbr_short_t)));
  return ret;
}

sup::dto::uint64 GetTimestampField(event_handler_args args)
{
  if (args.status != ECA_NORMAL)
  {
    return 0;
  }
  epicsTimeStamp *_time = (epicsTimeStamp *)(((char *)args.dbr) + 2 * sizeof(dbr_short_t));
  return ToAbsoluteTime_ns(_time->secPastEpoch + POSIX_TIME_AT_EPICS_EPOCH, _time->nsec);
}

}  // namespace sup::epics::cahelper

namespace
{
sup::dto::uint64 ToAbsoluteTime_ns(sup::dto::uint64 seconds, sup::dto::uint64 nanoseconds)
{
  return seconds * 1000000000ul + nanoseconds;
}
}  // unnamed namespace
