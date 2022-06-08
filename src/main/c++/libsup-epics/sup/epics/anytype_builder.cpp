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

#include "anytype_builder.h"

#include <pvxs/data.h>
#include <sup/dto/anytype.h>
#include <sup/dto/anytype_registry.h>
#include <sup/dto/parse/anyvalue_builder.h>
#include <sup/epics/dto_typecode_conversion_utils.h>

#include <iostream>
#include <stdexcept>

namespace sup
{
namespace epics
{

struct AnyTypeBuilder::AnyTypeBuilderImpl
{
  ::pvxs::TypeDef m_pvxs_type;
  ::pvxs::Value m_pvxs_value;
  ::sup::dto::AnyTypeRegistry m_registry;
  std::unique_ptr<::sup::dto::AnyValueBuilder> m_builder;

  void MakeAnyValue()
  {
    m_builder.reset(new ::sup::dto::AnyValueBuilder(&m_registry));
    m_builder->StartObject();
    ProcessPvxsValue(m_pvxs_value);
    m_builder->EndObject(1);
  }

  void ProcessPvxsValue(const ::pvxs::Value& value)
  {
    for (auto it : value.ichildren())
    {
      std::cout << it << std::endl;
    }
  }

  AnyTypeBuilderImpl() = default;
};

AnyTypeBuilder::AnyTypeBuilder(const pvxs::TypeDef& pvxs_type) : p_impl(new AnyTypeBuilderImpl)
{
  p_impl->m_pvxs_type = pvxs_type;
  p_impl->m_pvxs_value = pvxs_type.create();
}

dto::AnyType AnyTypeBuilder::MoveAnyType() const
{
  // there is no way to get TypeCode from TypeDef without construction of Value
  auto value = p_impl->m_pvxs_type.create();

  auto code = GetAnyTypeCode(value.type());

  if (::sup::dto::IsScalarTypeCode(code))
  {
    return dto::AnyType(code);
  }

  if (::sup::dto::IsStructTypeCode(code))
  {
    p_impl->MakeAnyValue();
  }

  return {};
}

AnyTypeBuilder::~AnyTypeBuilder() = default;

}  // namespace epics
}  // namespace sup
