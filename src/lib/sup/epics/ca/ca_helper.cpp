/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2024 ITER Organization,
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
#include <sup/dto/anyvalue_helper.h>
#include <sup/dto/anyvalue_exceptions.h>
#include <sup/dto/json_value_parser.h>

#include <cadef.h>

#include <map>
#include <vector>

namespace
{
sup::dto::uint64 ToAbsoluteTime_ns(sup::dto::uint64 seconds, sup::dto::uint64 nanoseconds);
chtype ScalarTypeCodeToChannelType(sup::dto::TypeCode typecode);
chtype ArrayTypeCodeToChannelType(sup::dto::TypeCode typecode);
sup::dto::AnyValue ParseFromStringTypes(const sup::dto::AnyType& anytype, char* ref,
                                        unsigned long multiplicity);
sup::dto::AnyValue ParseFromStringType(const sup::dto::AnyType& anytype, char* ref);
sup::dto::AnyValue ParseFromUnsignedEnumTypes(const sup::dto::AnyType& anytype, char* ref,
                                          unsigned long multiplicity);
sup::dto::AnyValue ParseFromUnsignedType(const sup::dto::AnyType& anytype, char* ref);
sup::dto::AnyValue ParseNumericFromString(const sup::dto::AnyType& anytype, const std::string& str);
sup::dto::AnyValue ParseBooleans(const sup::dto::AnyType& anytype, char* ref,
                                 unsigned long multiplicity);
sup::dto::AnyValue ParseBoolean(char* ref);
sup::dto::AnyValue ParseFromBytes(const sup::dto::AnyType& anytype, chtype channeltype, char* ref,
                                  unsigned long multiplicity);
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
  return sup::dto::IsArrayType(anytype) ?
    ArrayTypeCodeToChannelType(anytype.ElementType().GetTypeCode()) :
    ScalarTypeCodeToChannelType(anytype.GetTypeCode());
}

unsigned long ChannelMultiplicity(const sup::dto::AnyType& anytype)
{
  auto result = anytype.NumberOfElements();
  return result ? result : 1u;
}

sup::dto::AnyValue ParseAnyValue(const sup::dto::AnyType& anytype, unsigned long count, char* ref)
{
  if (!ref)
  {
    return {};
  }
  auto chtype = ChannelType(anytype);
  if (chtype == DBR_STRING)
  {
    return ParseFromStringTypes(anytype, ref, count);
  }
  if (chtype == DBR_ENUM)
  {
    return ParseFromUnsignedEnumTypes(anytype, ref, count);
  }
  if (anytype == sup::dto::BooleanType
      || (sup::dto::IsArrayType(anytype) && anytype.ElementType() == sup::dto::BooleanType))
  {
    return ParseBooleans(anytype, ref, count);
  }
  try
  {
    return ParseFromBytes(anytype, chtype, ref, count);
  }
  catch(const sup::dto::MessageException&)
  {
    return {};
  }
  return {};
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
chtype ScalarTypeCodeToChannelType(sup::dto::TypeCode typecode)
{
  using namespace sup::dto;
  static std::map<TypeCode, chtype> channel_type_map =
    {
      {TypeCode::Bool, DBR_CHAR},
      {TypeCode::Char8, DBR_CHAR},
      {TypeCode::Int8, DBR_CHAR},
      {TypeCode::UInt8, DBR_ENUM},
      {TypeCode::Int16, DBR_SHORT},
      {TypeCode::UInt16, DBR_ENUM},
      {TypeCode::Int32, DBR_LONG},
      {TypeCode::UInt32, DBR_ENUM},
      {TypeCode::Int64, DBR_STRING},
      {TypeCode::UInt64, DBR_ENUM},
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

chtype ArrayTypeCodeToChannelType(sup::dto::TypeCode typecode)
{
  using namespace sup::dto;
  static std::map<TypeCode, chtype> channel_type_map =
    {
      {TypeCode::Bool, DBR_CHAR},
      {TypeCode::Char8, DBR_CHAR},
      {TypeCode::Int8, DBR_CHAR},
      {TypeCode::UInt8, DBR_ENUM},
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
  sup::dto::AnyValue result{anytype};
  if (result.NumberOfElements() > 0)
  {
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

sup::dto::AnyValue ParseFromUnsignedEnumTypes(const sup::dto::AnyType& anytype, char* ref,
                                              unsigned long multiplicity)
{
  sup::dto::AnyValue result{anytype};
  if (result.NumberOfElements() > 0)
  {
    sup::dto::AnyType el_type = anytype.ElementType();
    const std::size_t kEnumLength = dbr_size[DBR_ENUM];
    for (unsigned long idx = 0; idx < multiplicity; ++idx)
    {
      char* el_ref = ref + idx * kEnumLength;
      result[idx] = ParseFromUnsignedType(el_type, el_ref);
    }
    return result;
  }
  return ParseFromUnsignedType(anytype, ref);
}

sup::dto::AnyValue ParseFromUnsignedType(const sup::dto::AnyType& anytype, char* ref)
{
  sup::dto::AnyValue tmp{sup::dto::UnsignedInteger16Type};
  sup::dto::FromBytes(tmp, (const sup::dto::uint8*)ref, 2u);
  sup::dto::AnyValue result{anytype};
  if (!sup::dto::TryConvert(result, tmp))
  {
    return {};
  }
  return result;
}

sup::dto::AnyValue ParseNumericFromString(const sup::dto::AnyType& anytype, const std::string& str)
{
  sup::dto::JSONAnyValueParser parser;
  if (!parser.TypedParseString(anytype, str))
  {
    return {};
  }
  return parser.MoveAnyValue();
}

sup::dto::AnyValue ParseBooleans(const sup::dto::AnyType& anytype, char* ref,
                                 unsigned long multiplicity)
{
  sup::dto::AnyValue result{anytype};
  if (result.NumberOfElements() > 0)
  {
    for (unsigned long idx = 0; idx < multiplicity; ++idx)
    {
      result[idx] = ParseBoolean(ref + idx);
    }
    return result;
  }
  return ParseBoolean(ref);
}
sup::dto::AnyValue ParseBoolean(char* ref)
{
  sup::dto::AnyValue tmp{sup::dto::Character8Type};
  sup::dto::FromBytes(tmp, (const sup::dto::uint8*)ref, 1u);
  sup::dto::AnyValue result{sup::dto::BooleanType};
  result.ConvertFrom(tmp);
  return result;
}

sup::dto::AnyValue ParseFromBytes(const sup::dto::AnyType& anytype, chtype channeltype, char* ref,
                                  unsigned long multiplicity)
{
  sup::dto::AnyValue temp{anytype};
  if (sup::dto::IsArrayType(anytype))
  {
    temp = sup::dto::AnyValue{multiplicity, anytype.ElementType(), anytype.GetTypeName()};
  }
  auto size = dbr_size[channeltype] * multiplicity;
  sup::dto::FromBytes(temp, (const sup::dto::uint8*)ref, size);
  if (sup::dto::IsArrayType(anytype))
  {
    sup::dto::AnyValue result{anytype};
    for (size_t i=0; i<multiplicity; ++i)
    {
      result[i] = temp[i];
    }
    return result;
  }
  return temp;
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
