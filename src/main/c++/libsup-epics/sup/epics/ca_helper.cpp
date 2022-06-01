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

#include "sup/epics/ca_helper.h"

#include <sup/dto/AnyType.h>
#include <cadef.h>
#include <map>

namespace
{
sup::dto::uint64 ToAbsoluteTime_ns(sup::dto::uint64 seconds, sup::dto::uint64 nanoseconds);
chtype TypeCodeToChannelType(sup::dto::TypeCode typecode);
}  // unnamed namespace

namespace sup
{
namespace epics
{
namespace cahelper
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

chtype ChannelType(const sup::dto::AnyType& anytype)
{
  sup::dto::AnyType input_type = sup::dto::IsArrayType(anytype) ?
                                 anytype.ElementType() :
                                 anytype;
  return TypeCodeToChannelType(input_type.GetTypeCode());
}

unsigned long ChannelMultiplicity(const sup::dto::AnyType& anytype)
{
  auto result = anytype.NumberOfElements();
  return result ? result : 1u;
}

}  // namespace cahelper

}  // namespace epics

}  // namespace sup

namespace
{
sup::dto::uint64 ToAbsoluteTime_ns(sup::dto::uint64 seconds, sup::dto::uint64 nanoseconds)
{
  return seconds * 1000000000ul + nanoseconds;
}
chtype TypeCodeToChannelType(sup::dto::TypeCode typecode)
{
  using namespace sup::dto;
  static std::map<TypeCode, chtype> channel_type_map =
    {
      {TypeCode::Bool, DBR_CHAR},
      {TypeCode::Char8, DBR_CHAR},
      {TypeCode::Int8, DBR_CHAR},
      {TypeCode::UInt8, DBR_CHAR},
      {TypeCode::Int16, DBR_SHORT},
      {TypeCode::UInt16, DBR_SHORT},
      {TypeCode::Int32, DBR_LONG},
      {TypeCode::UInt32, DBR_LONG},
      // {TypeCode::Int64, DBR_LONG},
      // {TypeCode::UInt64, DBR_LONG},
      {TypeCode::Float32, DBR_FLOAT},
      {TypeCode::Float64, DBR_DOUBLE},
      {TypeCode::String, DBR_STRING}
    };
  auto it = channel_type_map.find(typecode);
  if (it == channel_type_map.end())
  {
    return -1;
  }
  return it->second;
}

}  // unnamed namespace
