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

#include <pvxs/nt.h>
#include <sup/dto/anytype.h>
#include <sup/dto/anytype_helper.h>
#include <sup/dto/anyvalue.h>
#include <sup/dto/anyvalue_helper.h>
#include <sup/epics/utils/dto_conversion_utils.h>
#include <sup/epics/utils/anyvalue_from_pvxs_builder.h>
#include <sup/epics/utils/pvxs_type_builder.h>
#include <sup/epics/utils/pvxs_utils.h>
#include <sup/epics/utils/pvxs_value_builder.h>

#include <stdexcept>

namespace
{
const std::string kValueFieldName("value");
const std::vector<std::string> kExpectedMembersInScalarStruct({kValueFieldName});
}  // namespace

namespace sup
{
namespace epics
{

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

dto::AnyValue BuildAnyValue(const pvxs::Value& pvxs_value)
{
  AnyValueFromPVXSBuilder builder(pvxs_value);
  return builder.MoveAnyValue();
}

sup::dto::AnyValue ConvertScalarToStruct(const sup::dto::AnyValue& any_value)
{
  if (!sup::dto::IsScalarValue(any_value))
  {
    throw std::runtime_error("Error in ConvertScalarToStruct: given value is not a scalar");
  }

  sup::dto::AnyValue result{{{kValueFieldName, any_value}}};
  return result;
}

dto::AnyValue ConvertStructToScalar(const dto::AnyValue& any_value)
{
  if (!sup::dto::IsStructValue(any_value))
  {
    throw std::runtime_error("Error in ConvertScalarToStruct: given value is not a struct");
  }

  if (any_value.MemberNames() != kExpectedMembersInScalarStruct)
  {
    throw std::runtime_error(
        "Error in ConvertScalarToStruct: unexpected list of members in a struct");
  }

  if (!sup::dto::IsScalarValue(any_value[kValueFieldName]))
  {
    throw std::runtime_error("Error in ConvertScalarToStruct: unexpected field type");
  }

  return any_value[kValueFieldName];
}

pvxs::Value BuildScalarAwarePVXSValue(const dto::AnyValue& any_value)
{
  auto to_convert =
      sup::dto::IsScalarValue(any_value) ? ConvertScalarToStruct(any_value) : any_value;
  return BuildPVXSValue(to_convert);
}

dto::AnyValue BuildScalarAwareAnyValue(const pvxs::Value& pvxs_value)
{
  const bool is_scalar_struct = GetMemberNames(pvxs_value) == kExpectedMembersInScalarStruct
                                && IsScalar(pvxs_value[kValueFieldName]);
  auto to_convert = is_scalar_struct ? pvxs_value[kValueFieldName] : pvxs_value;
  return BuildAnyValue(to_convert);
}

}  // namespace epics

}  // namespace sup
