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

#include <sup/epics/ca/ca_helper.h>

#include <sup/dto/anytype.h>
#include <sup/dto/anyvalue_exceptions.h>
#include <sup/dto/json_value_parser.h>

#include <cadef.h>

#include <map>
#include <vector>

namespace
{
const std::string kTrueString = "TRUE";
const std::string kFalseString = "FALSE";
sup::dto::uint64 ToAbsoluteTime_ns(sup::dto::uint64 seconds, sup::dto::uint64 nanoseconds);
chtype TypeCodeToChannelType(sup::dto::TypeCode typecode);
sup::dto::AnyValue ParseFromStringTypes(const sup::dto::AnyType& anytype, char* ref,
                                        unsigned long multiplicity);
sup::dto::AnyValue ParseFromStringType(const sup::dto::AnyType& anytype, char* ref);
sup::dto::AnyValue ParseNumericFromString(const sup::dto::AnyType& anytype, const std::string& str);
std::string GetEPICSString(char* ref);

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

sup::dto::AnyValue ParseAnyValue(const sup::dto::AnyType& anytype, char* ref)
{
  if (!ref)
  {
    return {};
  }
  auto chtype = ChannelType(anytype);
  auto multiplicity = ChannelMultiplicity(anytype);
  if (chtype == DBR_STRING)
  {
    return ParseFromStringTypes(anytype, ref, multiplicity);
  }
  sup::dto::AnyValue result{anytype};
  auto size = dbr_size[chtype] * multiplicity;
  try
  {
    sup::dto::FromBytes(result, (const sup::dto::uint8*)ref, size);
  }
  catch(const sup::dto::MessageException&)
  {
    return {};
  }
  return result;
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
      {TypeCode::UInt8, DBR_STRING},
      {TypeCode::Int16, DBR_SHORT},
      {TypeCode::UInt16, DBR_ENUM},
      {TypeCode::Int32, DBR_LONG},
      {TypeCode::UInt32, DBR_STRING},
      {TypeCode::Int64, DBR_STRING},
      {TypeCode::UInt64, DBR_STRING},
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

sup::dto::AnyValue ParseFromStringTypes(const sup::dto::AnyType& anytype, char* ref,
                                        unsigned long multiplicity)
{
  if (multiplicity > 1)
  {
    sup::dto::AnyValue result{anytype};
    sup::dto::AnyType el_type = anytype.ElementType();
    const std::size_t kEpicsStringLength = dbr_size[DBR_STRING];
    for (unsigned long idx = 0; idx < multiplicity; ++idx)
    {
      char* el_ref = ref + idx * kEpicsStringLength;
      result[idx] = ParseFromStringType(el_type, el_ref);
    }
    return result;
  }
  return ParseFromStringType(anytype, ref);
}

sup::dto::AnyValue ParseFromStringType(const sup::dto::AnyType& anytype, char* ref)
{
  auto str = GetEPICSString(ref);
  if (anytype == sup::dto::StringType)
  {
    return str;
  }
  return ParseNumericFromString(anytype, str);
}

sup::dto::AnyValue ParseNumericFromString(const sup::dto::AnyType& anytype, const std::string& str)
{
  // Handle bool conversions (TRUE/FALSE) separately
  if (str == kTrueString)
  {
    return sup::dto::AnyValue(anytype, 1);
  }
  if (str == kFalseString)
  {
    return sup::dto::AnyValue(anytype, 0);
  }
  sup::dto::JSONAnyValueParser parser;
  if (!parser.TypedParseString(anytype, str))
  {
    return {};
  }
  return parser.MoveAnyValue();
}

std::string GetEPICSString(char* ref)
{
  const std::size_t kEpicsStringLength = dbr_size[DBR_STRING];
  auto buffer = std::vector<char>(kEpicsStringLength+1);
  buffer[kEpicsStringLength] = 0;
  strncpy(buffer.data(), ref, kEpicsStringLength);
  return std::string(buffer.data());
}

}  // unnamed namespace
