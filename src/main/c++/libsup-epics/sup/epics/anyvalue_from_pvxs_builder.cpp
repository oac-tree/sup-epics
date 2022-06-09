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

#include "anyvalue_from_pvxs_builder.h"

#include <pvxs/data.h>
#include <sup/dto/anytype.h>
#include <sup/dto/anyvalue.h>
#include <sup/epics/anyvalue_build_adapter.h>
#include <sup/epics/dto_scalar_conversion_utils.h>
#include <sup/epics/dto_typecode_conversion_utils.h>

#include <iostream>
#include <stdexcept>

namespace sup
{
namespace epics
{

struct AnyValueFromPVXSBuilder::AnyValueFromPVXSBuilderImpl
{
  ::pvxs::Value m_pvxs_value;
  AnyValueBuildAdapter m_builder;
  ::sup::dto::AnyValue m_result;

  void ProcessPvxsValue(const pvxs::Value& pvxs_value)
  {
    m_pvxs_value = pvxs_value;

    auto code = GetAnyTypeCode(m_pvxs_value.type());

    if (::sup::dto::IsScalarTypeCode(code))
    {
      m_result = ::sup::dto::AnyValue(::sup::dto::AnyType(code));
      AssignPVXSValueToAnyValueScalar(m_pvxs_value, m_result);
    }
  }

  AnyValueFromPVXSBuilderImpl() = default;
};

AnyValueFromPVXSBuilder::AnyValueFromPVXSBuilder(const pvxs::Value& pvxs_value)
    : p_impl(new AnyValueFromPVXSBuilderImpl)
{
  p_impl->ProcessPvxsValue(pvxs_value);
}

dto::AnyValue AnyValueFromPVXSBuilder::MoveAnyType() const
{
  return std::move(p_impl->m_result);
}

AnyValueFromPVXSBuilder::~AnyValueFromPVXSBuilder() = default;

}  // namespace epics
}  // namespace sup
