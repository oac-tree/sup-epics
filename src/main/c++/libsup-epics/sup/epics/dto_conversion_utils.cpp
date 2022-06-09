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

#include "dto_conversion_utils.h"

#include <pvxs/nt.h>
#include <sup/dto/anytype.h>
#include <sup/dto/anytype_helper.h>
#include <sup/dto/anyvalue.h>
#include <sup/dto/anyvalue_helper.h>
#include <sup/epics/anyvalue_from_pvxs_builder.h>
#include <sup/epics/pvxs_type_builder.h>
#include <sup/epics/pvxs_value_builder.h>

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

dto::AnyType BuildAnyValue(const pvxs::Value& pvxs_value)
{
  AnyValueFromPVXSBuilder builder(pvxs_value);
  return builder.MoveAnyType();
}

}  // namespace epics

}  // namespace sup
