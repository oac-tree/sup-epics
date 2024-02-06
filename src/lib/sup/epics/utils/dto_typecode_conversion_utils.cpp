/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Gennady Pospelov (IO)
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

#include "dto_typecode_conversion_utils.h"

#include <pvxs/data.h>
#include <pvxs/nt.h>
#include <sup/dto/anytype.h>
#include <sup/dto/anytype_helper.h>
#include <sup/dto/anyvalue.h>
#include <sup/dto/anyvalue_helper.h>
#include <sup/dto/basic_scalar_types.h>
#include <sup/epics/utils/pvxs_type_builder.h>
#include <sup/epics/utils/pvxs_utils.h>
#include <sup/epics/utils/pvxs_value_builder.h>

#include <functional>
#include <iostream>
#include <map>
#include <stdexcept>

namespace
{

//! Correspondance of AnyValue type code to PVXS TypeCode (base types).
const std::map<sup::dto::TypeCode, pvxs::TypeCode> kTypeCodeMap = {
    {sup::dto::TypeCode::Empty, pvxs::TypeCode::Null},
    {sup::dto::TypeCode::Bool, pvxs::TypeCode::Bool},
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

//! Correspondance of AnyValue array element type code to PVXS TypeCode (arrays).
const std::map<sup::dto::TypeCode, pvxs::TypeCode> kArrayElementTypeCodeMap = {
    {sup::dto::TypeCode::Bool, pvxs::TypeCode::BoolA},
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

//! Finds pvxs::TypeCode corresponding to the given AnyType. Use provided container.
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

//! Finds pvxs::TypeCode corresponding to the given AnyType. Use provided container.
template <typename T>
sup::dto::TypeCode FindAnyTypeCode(const T& container, const pvxs::TypeCode& type_code)
{
  auto on_element = [type_code](const typename T::value_type& pair)
  { return pair.second == type_code; };
  auto it = std::find_if(container.begin(), container.end(), on_element);
  if (it == container.end())
  {
    throw std::runtime_error("Unknown TypeCode");
  }
  return it->first;
}

}  // namespace

namespace sup
{
namespace epics
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
  return any_type.GetTypeCode() == sup::dto::TypeCode::Char8 ? pvxs::TypeCode::UInt8
                                                             : FindTypeCode(kTypeCodeMap, any_type);
}

pvxs::TypeCode GetPVXSArrayTypeCode(const dto::AnyType& any_type)
{
  return any_type.GetTypeCode() == sup::dto::TypeCode::Char8
             ? pvxs::TypeCode::UInt8A
             : FindTypeCode(kArrayElementTypeCodeMap, any_type);
}

dto::TypeCode GetAnyTypeCode(const pvxs::TypeCode& pvxs_type)
{
  if (IsScalarArray(pvxs_type))
  {
    return FindAnyTypeCode(kArrayElementTypeCodeMap, pvxs_type);
  }

  return FindAnyTypeCode(kTypeCodeMap, pvxs_type);
}

}  // namespace epics

}  // namespace sup
