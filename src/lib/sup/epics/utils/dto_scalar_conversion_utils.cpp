/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Gennady Pospelov (IO)
 *
 * Copyright (c) : 2010-2025 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 * SPDX-License-Identifier: MIT
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file LICENSE located in the top level directory
 * of the distribution package.
 *****************************************************************************/

#include <pvxs/data.h>
#include <pvxs/nt.h>
#include <sup/dto/anytype.h>
#include <sup/dto/anytype_helper.h>
#include <sup/dto/anyvalue.h>
#include <sup/dto/anyvalue_helper.h>
#include <sup/dto/basic_scalar_types.h>
#include <sup/epics/utils/dto_pvxs_t.h>
#include <sup/epics/utils/dto_scalar_conversion_utils.h>
#include <sup/epics/utils/dto_typecode_conversion_utils.h>
#include <sup/epics/utils/pvxs_type_builder.h>
#include <sup/epics/utils/pvxs_utils.h>
#include <sup/epics/utils/pvxs_value_builder.h>

#include <functional>
#include <iostream>
#include <map>
#include <stdexcept>

namespace
{

//! Assigns scalar value from AnyValue to pre-created PVXS value.
template <typename T>
void AssignToPVXSScalar(const sup::dto::AnyValue& any_value, pvxs::Value& pvxs_value)
{
  pvxs_value = any_value.As<T>();
}

//! Assigns scalar value from PVXS value to pre-created AnyValue value.
template <typename T>
void AssignToAnyValueScalar(const pvxs::Value& pvxs_value, sup::dto::AnyValue& any_value)
{
  any_value = pvxs_value.as<T>();
}

//! Assign array elements from AnyValue to pre-created PVXS value.
template <typename T>
void AssignToPVXSScalarArray(const sup::dto::AnyValue& any_value, pvxs::Value& pvxs_value)
{
  auto result = ::pvxs::shared_array<sup::epics::DTOToPVXSScalar_t<T>>(any_value.NumberOfElements());
  for (size_t i = 0; i < any_value.NumberOfElements(); ++i)
  {
    result[i] = any_value[i].As<T>();
  }
  pvxs_value = result.freeze();  // this is how ::pvxs wants arrays are assigned
}

//! Assign array elements from PVXS value to pre-created AnyValue.
template <typename T>
::sup::dto::AnyValue CreateAnyValueScalarArray(const pvxs::Value& pvxs_value)
{
  auto data = pvxs_value.as<::pvxs::shared_array<const sup::epics::DTOToPVXSScalar_t<T>>>();

  auto type_code = ::sup::epics::GetAnyTypeCode(pvxs_value.type());

  ::sup::dto::AnyValue result(data.size(), ::sup::dto::AnyType(type_code));

  for (size_t i = 0; i < data.size(); ++i)
  {
    result[i] = static_cast<T>(data[i]);
  }

  return result;
}

using pvxs_function_t =
    std::function<void(const sup::dto::AnyValue& anyvalue, pvxs::Value& pvxs_value)>;

//! Correspondance of AnyValue type code to PVXS value function to assign scalars.
const std::map<sup::dto::TypeCode, pvxs_function_t> kAssignToPVXSScalarMap = {
    {sup::dto::TypeCode::Bool, AssignToPVXSScalar<sup::dto::boolean>},
    {sup::dto::TypeCode::Char8, AssignToPVXSScalar<sup::dto::uint8>},  // is it Ok?
    {sup::dto::TypeCode::Int8, AssignToPVXSScalar<sup::dto::int8>},
    {sup::dto::TypeCode::UInt8, AssignToPVXSScalar<sup::dto::uint8>},
    {sup::dto::TypeCode::Int16, AssignToPVXSScalar<sup::dto::int16>},
    {sup::dto::TypeCode::UInt16, AssignToPVXSScalar<sup::dto::uint16>},
    {sup::dto::TypeCode::Int32, AssignToPVXSScalar<sup::dto::int32>},
    {sup::dto::TypeCode::UInt32, AssignToPVXSScalar<sup::dto::uint32>},
    {sup::dto::TypeCode::Int64, AssignToPVXSScalar<sup::dto::int64>},
    {sup::dto::TypeCode::UInt64, AssignToPVXSScalar<sup::dto::uint64>},
    {sup::dto::TypeCode::Float32, AssignToPVXSScalar<sup::dto::float32>},
    {sup::dto::TypeCode::Float64, AssignToPVXSScalar<sup::dto::float64>},
    {sup::dto::TypeCode::String, AssignToPVXSScalar<std::string>}};

//! Correspondance of AnyValue type code to PVXS value function to assign scalar arrays.
const std::map<sup::dto::TypeCode, pvxs_function_t> kAssignToPVXSScalarArrayMap = {
    {sup::dto::TypeCode::Bool, AssignToPVXSScalarArray<sup::dto::boolean>},
    {sup::dto::TypeCode::Char8, AssignToPVXSScalarArray<sup::dto::uint8>},  // is it Ok?
    {sup::dto::TypeCode::Int8, AssignToPVXSScalarArray<sup::dto::int8>},
    {sup::dto::TypeCode::UInt8, AssignToPVXSScalarArray<sup::dto::uint8>},
    {sup::dto::TypeCode::Int16, AssignToPVXSScalarArray<sup::dto::int16>},
    {sup::dto::TypeCode::UInt16, AssignToPVXSScalarArray<sup::dto::uint16>},
    {sup::dto::TypeCode::Int32, AssignToPVXSScalarArray<sup::dto::int32>},
    {sup::dto::TypeCode::UInt32, AssignToPVXSScalarArray<sup::dto::uint32>},
    {sup::dto::TypeCode::Int64, AssignToPVXSScalarArray<sup::dto::int64>},
    {sup::dto::TypeCode::UInt64, AssignToPVXSScalarArray<sup::dto::uint64>},
    {sup::dto::TypeCode::Float32, AssignToPVXSScalarArray<sup::dto::float32>},
    {sup::dto::TypeCode::Float64, AssignToPVXSScalarArray<sup::dto::float64>},
    {sup::dto::TypeCode::String, AssignToPVXSScalarArray<std::string>}};

using anyvalue_function_t =
    std::function<void(const pvxs::Value& pvxs_value, sup::dto::AnyValue& anyvalue)>;

//! Correspondance of AnyValue type code to PVXS value function to assign scalars.
const std::map<sup::dto::TypeCode, anyvalue_function_t> kAssignToAnyValueScalarMap = {
    {sup::dto::TypeCode::Bool, AssignToAnyValueScalar<sup::dto::boolean>},
    {sup::dto::TypeCode::Char8, AssignToAnyValueScalar<sup::dto::uint8>},  // is it Ok?
    {sup::dto::TypeCode::Int8, AssignToAnyValueScalar<sup::dto::int8>},
    {sup::dto::TypeCode::UInt8, AssignToAnyValueScalar<sup::dto::uint8>},
    {sup::dto::TypeCode::Int16, AssignToAnyValueScalar<sup::dto::int16>},
    {sup::dto::TypeCode::UInt16, AssignToAnyValueScalar<sup::dto::uint16>},
    {sup::dto::TypeCode::Int32, AssignToAnyValueScalar<sup::dto::int32>},
    {sup::dto::TypeCode::UInt32, AssignToAnyValueScalar<sup::dto::uint32>},
    {sup::dto::TypeCode::Int64, AssignToAnyValueScalar<sup::dto::int64>},
    {sup::dto::TypeCode::UInt64, AssignToAnyValueScalar<sup::dto::uint64>},
    {sup::dto::TypeCode::Float32, AssignToAnyValueScalar<sup::dto::float32>},
    {sup::dto::TypeCode::Float64, AssignToAnyValueScalar<sup::dto::float64>},
    {sup::dto::TypeCode::String, AssignToAnyValueScalar<std::string>}};

using create_anyvalue_t = std::function<sup::dto::AnyValue(const pvxs::Value& pvxs_value)>;

//! Correspondance of AnyValue type code to PVXS value function to assign scalars.
const std::map<sup::dto::TypeCode, create_anyvalue_t> kCreateAnyValueScalarArrayMap = {
    {sup::dto::TypeCode::Bool, CreateAnyValueScalarArray<sup::dto::boolean>},
    {sup::dto::TypeCode::Char8, CreateAnyValueScalarArray<sup::dto::uint8>},  // is it Ok?
    {sup::dto::TypeCode::Int8, CreateAnyValueScalarArray<sup::dto::int8>},
    {sup::dto::TypeCode::UInt8, CreateAnyValueScalarArray<sup::dto::uint8>},
    {sup::dto::TypeCode::Int16, CreateAnyValueScalarArray<sup::dto::int16>},
    {sup::dto::TypeCode::UInt16, CreateAnyValueScalarArray<sup::dto::uint16>},
    {sup::dto::TypeCode::Int32, CreateAnyValueScalarArray<sup::dto::int32>},
    {sup::dto::TypeCode::UInt32, CreateAnyValueScalarArray<sup::dto::uint32>},
    {sup::dto::TypeCode::Int64, CreateAnyValueScalarArray<sup::dto::int64>},
    {sup::dto::TypeCode::UInt64, CreateAnyValueScalarArray<sup::dto::uint64>},
    {sup::dto::TypeCode::Float32, CreateAnyValueScalarArray<sup::dto::float32>},
    {sup::dto::TypeCode::Float64, CreateAnyValueScalarArray<sup::dto::float64>},
    {sup::dto::TypeCode::String, CreateAnyValueScalarArray<std::string>}};

//! Finds pvxs::TypeCode corresponding to the given AnyType. Use provided container.
template <typename T>
typename T::mapped_type FindContentForType(const T& container, sup::dto::TypeCode type_code)
{
  auto it = container.find(type_code);
  if (it == container.end())
  {
    throw std::runtime_error("Unknown AnyType code");
  }
  return it->second;
}

}  // namespace

namespace sup
{
namespace epics
{

pvxs::Value GetPVXSValueFromScalar(const dto::AnyValue& any_value)
{
  auto pvxs_typecode = GetPVXSBaseTypeCode(any_value.GetType());
  auto result = pvxs::TypeDef(pvxs_typecode).create();
  AssignAnyValueToPVXSValueScalar(any_value, result);
  return result;
}

void AssignAnyValueToPVXSValueScalar(const dto::AnyValue& any_value, pvxs::Value& pvxs_value)
{
  if (!sup::dto::IsScalarValue(any_value))
  {
    throw std::runtime_error("Method is intended for scalar like AnyValues");
  }

  if (GetPVXSBaseTypeCode(any_value.GetType()) != pvxs_value.type())
  {
    throw std::runtime_error("Given AnyValue type doesn't match PVXS value type");
  }

  auto assign_func = FindContentForType(kAssignToPVXSScalarMap, any_value.GetTypeCode());
  assign_func(any_value, pvxs_value);
}

void AssignAnyValueToPVXSValueScalarArray(const dto::AnyValue& any_value, pvxs::Value& pvxs_value)
{
  if (!sup::dto::IsArrayValue(any_value))
  {
    throw std::runtime_error("Method is intended for array like AnyValues");
  }

  auto element_type = any_value.GetType().ElementType();

  if (GetPVXSArrayTypeCode(element_type) != pvxs_value.type())
  {
    throw std::runtime_error("Type of AnyValue array element doesn't match type of PVXS value");
  }

  auto assign_func = FindContentForType(kAssignToPVXSScalarArrayMap, element_type.GetTypeCode());
  assign_func(any_value, pvxs_value);
}

void AssignPVXSValueToAnyValueScalar(const ::pvxs::Value& pvxs_value,
                                     ::sup::dto::AnyValue& any_value)
{
  auto type_code = GetAnyTypeCode(pvxs_value.type());

  if (any_value.GetTypeCode() != type_code)
  {
    throw std::runtime_error("Given PVXS value type doesn't match AnyValue type");
  }

  auto assign_func = FindContentForType(kAssignToAnyValueScalarMap, type_code);
  assign_func(pvxs_value, any_value);
}

dto::AnyValue GetAnyValueFromScalar(const pvxs::Value& pvxs_value)
{
  if (!IsScalar(pvxs_value))
  {
    throw std::runtime_error("Given PVXS Value is not a scalar");
  }

  ::sup::dto::AnyValue result(::sup::dto::AnyType(GetAnyTypeCode(pvxs_value.type())));
  AssignPVXSValueToAnyValueScalar(pvxs_value, result);

  return result;
}

dto::AnyValue GetAnyValueFromScalarArray(const pvxs::Value& pvxs_value)
{
  if (!IsScalarArray(pvxs_value))
  {
    throw std::runtime_error("Given PVXS Value is not a scalar array");
  }

  auto type_code = GetAnyTypeCode(pvxs_value.type());

  auto assign_func = FindContentForType(kCreateAnyValueScalarArrayMap, type_code);
  return assign_func(pvxs_value);
}

}  // namespace epics

}  // namespace sup
