/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
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

#include "pvxs_utils.h"

#include <pvxs/data.h>

#include <algorithm>
#include <vector>

namespace
{

const std::vector<::pvxs::TypeCode> kPvxsScalarCodes = {
    pvxs::TypeCode::Bool,    pvxs::TypeCode::Int8,    pvxs::TypeCode::UInt8,
    pvxs::TypeCode::Int16,   pvxs::TypeCode::UInt16,  pvxs::TypeCode::Int32,
    pvxs::TypeCode::UInt32,  pvxs::TypeCode::Int64,   pvxs::TypeCode::UInt64,
    pvxs::TypeCode::Float32, pvxs::TypeCode::Float64, pvxs::TypeCode::String};
}

namespace sup
{
namespace epics
{

bool IsScalar(const pvxs::Value &value)
{
  auto on_element = [&value](const ::pvxs::TypeCode &element) { return value.type() == element; };
  auto it = std::find_if(kPvxsScalarCodes.begin(), kPvxsScalarCodes.end(), on_element);
  return it != kPvxsScalarCodes.end();
}

bool IsStruct(const pvxs::Value &value)
{
  return value.type() == pvxs::TypeCode::Struct;
}

std::vector<pvxs::Value> GetChildren(const pvxs::Value &pvxs_value)
{
  std::vector<pvxs::Value> result;
  for (auto fld : pvxs_value.ichildren())
  {
    result.push_back(fld);
  }
  return result;
}

bool IsScalarArray(const pvxs::Value &value)
{
  return value.type().isarray();
}

}  // namespace epics

}  // namespace sup
