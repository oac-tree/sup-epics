/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP common components for EPICS network protocol
 *
 * Author        : Gennady Pospelov (IO)
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

#include "sup/epics/dtoconversionutils.h"

#include "AnyType.h"
#include "AnyTypeHelper.h"
#include "AnyValue.h"
#include "AnyValueHelper.h"
#include "BasicScalarTypes.h"
#include "pvxs/data.h"
#include "sup/epics/pvxstypebuilder.h"
#include "sup/epics/pvxsvaluebuilder.h"

#include <map>
#include <stdexcept>

namespace
{

//! Assigns scalar value from AnyValue to PVXS value.
template <typename T>
void AssignScalar(const sup::dto::AnyValue& any_value, pvxs::Value& pvxs_value)
{
  pvxs_value = any_value.As<T>();
}

using function_t = std::function<void(const sup::dto::AnyValue& anyvalue, pvxs::Value& pvxs_value)>;

//! Correspondance of AnyValue type code to PVXS TypeCode.
const std::map<sup::dto::TypeCode, pvxs::TypeCode> kTypeCodeMap = {
    {sup::dto::TypeCode::Empty, pvxs::TypeCode::Null},
    {sup::dto::TypeCode::Bool, pvxs::TypeCode::Bool},
    {sup::dto::TypeCode::Char8, pvxs::TypeCode::UInt8},  // is it Ok?
    {sup::dto::TypeCode::Int8, pvxs::TypeCode::Int8},
    {sup::dto::TypeCode::UInt8, pvxs::TypeCode::UInt8},
    {sup::dto::TypeCode::Int16, pvxs::TypeCode::Int16},
    {sup::dto::TypeCode::UInt16, pvxs::TypeCode::UInt16},
    {sup::dto::TypeCode::Int32, pvxs::TypeCode::Int32},
    {sup::dto::TypeCode::UInt32, pvxs::TypeCode::UInt32},
    {sup::dto::TypeCode::Int64, pvxs::TypeCode::Int64},
    {sup::dto::TypeCode::UInt64, pvxs::TypeCode::UInt64},
    {sup::dto::TypeCode::Float32, pvxs::TypeCode::Float32},
    {sup::dto::TypeCode::Float64, pvxs::TypeCode::Float64},
    {sup::dto::TypeCode::String, pvxs::TypeCode::String},
    {sup::dto::TypeCode::Struct, pvxs::TypeCode::Struct},
};

//! Correspondance of AnyValue element type code to PVXS TypeCode (arrays).
const std::map<sup::dto::TypeCode, pvxs::TypeCode> kArrayTypeCodeMap = {
    {sup::dto::TypeCode::Bool, pvxs::TypeCode::BoolA},
    {sup::dto::TypeCode::Char8, pvxs::TypeCode::UInt8A},  // is it Ok?
    {sup::dto::TypeCode::Int8, pvxs::TypeCode::Int8A},
    {sup::dto::TypeCode::UInt8, pvxs::TypeCode::UInt8A},
    {sup::dto::TypeCode::Int16, pvxs::TypeCode::Int16A},
    {sup::dto::TypeCode::UInt16, pvxs::TypeCode::UInt16A},
    {sup::dto::TypeCode::Int32, pvxs::TypeCode::Int32A},
    {sup::dto::TypeCode::UInt32, pvxs::TypeCode::UInt32A},
    {sup::dto::TypeCode::Int64, pvxs::TypeCode::Int64A},
    {sup::dto::TypeCode::UInt64, pvxs::TypeCode::UInt64A},
    {sup::dto::TypeCode::Float32, pvxs::TypeCode::Float32A},
    {sup::dto::TypeCode::Float64, pvxs::TypeCode::Float64A},
    {sup::dto::TypeCode::String, pvxs::TypeCode::StringA},
    {sup::dto::TypeCode::Struct, pvxs::TypeCode::StructA},
};

//! Correspondance of AnyValue type code to PVXS value assign function.
const std::map<sup::dto::TypeCode, function_t> kAssignScalarMap = {
    {sup::dto::TypeCode::Bool, AssignScalar<sup::dto::boolean>},
    {sup::dto::TypeCode::Char8, AssignScalar<sup::dto::uint8>},  // is it Ok?
    {sup::dto::TypeCode::Int8, AssignScalar<sup::dto::int8>},
    {sup::dto::TypeCode::UInt8, AssignScalar<sup::dto::uint8>},
    {sup::dto::TypeCode::Int16, AssignScalar<sup::dto::int16>},
    {sup::dto::TypeCode::UInt16, AssignScalar<sup::dto::uint16>},
    {sup::dto::TypeCode::Int32, AssignScalar<sup::dto::int32>},
    {sup::dto::TypeCode::UInt32, AssignScalar<sup::dto::uint32>},
    {sup::dto::TypeCode::Int64, AssignScalar<sup::dto::int64>},
    {sup::dto::TypeCode::UInt64, AssignScalar<sup::dto::uint64>},
    {sup::dto::TypeCode::Float32, AssignScalar<sup::dto::float32>},
    {sup::dto::TypeCode::Float64, AssignScalar<sup::dto::float64>},
    {sup::dto::TypeCode::String, AssignScalar<std::string>}};

template <typename T>
pvxs::TypeCode FindTypeCode(const T& container, const sup::dto::AnyType& any_type)
{
  auto it = container.find(any_type.GetTypeCode());
  if (it == container.end())
  {
    throw std::runtime_error("Unknown AnyType code");
  }
  return it->second;
}

}  // namespace

namespace sup::epics
{

pvxs::TypeCode GetPVXSTypeCode(const dto::AnyType& any_type)
{
  if (::sup::dto::IsArrayType(any_type))
  {
    return GetPVXSArrayTypeCode(any_type.ElementType());
  }
  return GetPVXSBaseTypeCode(any_type);
}

pvxs::TypeCode GetPVXSBaseTypeCode(const dto::AnyType& any_type)
{
  return FindTypeCode(kTypeCodeMap, any_type);
}

pvxs::TypeCode GetPVXSArrayTypeCode(const dto::AnyType& any_type)
{
  return FindTypeCode(kArrayTypeCodeMap, any_type);
}

// FIXME method is not used, consider removal
pvxs::Value GetPVXSValueFromScalar(const dto::AnyValue& any_value)
{
  if (!sup::dto::IsScalarValue(any_value))
  {
    throw std::runtime_error("Method is intended for scalar AnyValue");
  }

  auto it = kAssignScalarMap.find(any_value.GetTypeCode());
  if (it == kAssignScalarMap.end())
  {
    throw std::runtime_error("Not a known AnyValue scalar type code");
  }

  auto pvxs_typecode = GetPVXSBaseTypeCode(any_value.GetType());
  auto result = pvxs::TypeDef(pvxs_typecode).create();

  it->second(any_value, result);  // calling assign function

  return result;
};

void AssignPVXSValueFromScalar(const dto::AnyValue& any_value, pvxs::Value& pvxs_value)
{
  if (GetPVXSBaseTypeCode(any_value.GetType()) != pvxs_value.type())
  {
    throw std::runtime_error("Given AnyValue type doesn't match PVXS value type");
  }

  auto it = kAssignScalarMap.find(any_value.GetTypeCode());
  if (it == kAssignScalarMap.end())
  {
    throw std::runtime_error("Not a known AnyValue scalar type code");
  }

  it->second(any_value, pvxs_value);  // calling assign function
}

void AssignPVXSValueFromScalarArray(const dto::AnyValue& any_value, pvxs::Value& pvxs_value)
{
  if (!sup::dto::IsArrayValue(any_value))
  {
    throw std::runtime_error("Method is intended for array like AnyValues");
  }

  if (GetPVXSArrayTypeCode(any_value.GetType()) != pvxs_value.type())
  {
    throw std::runtime_error("Type of AnyValue array element doesn't match type of PVXS value");
  }

  for (size_t i = 0; i < any_value.NumberOfElements(); ++i)
  {
  }
}

pvxs::TypeDef BuildPVXSType(const dto::AnyType& any_type)
{
  PvxsTypeBuilder type_builder;
  sup::dto::SerializeAnyType(any_type, type_builder);
  return type_builder.GetPVXSType();
}

pvxs::Value BuildPVXSValue(const dto::AnyValue& any_value)
{
  PvxsValueBuilder builder(BuildPVXSType(any_value.GetType()));
  sup::dto::SerializeAnyValue(any_value, builder);
  return builder.GetPVXSValue();
}

}  // namespace sup::epics
