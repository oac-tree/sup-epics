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
#include <sup/dto/anytype_registry.h>
#include <sup/dto/parse/anytype_builder.h>
#include <sup/epics/dto_typecode_conversion_utils.h>

namespace sup
{
namespace epics
{

struct AnyTypeBuilder::AnyTypeBuilderImpl
{
  ::pvxs::TypeDef m_pvxs_type;
  ::sup::dto::AnyTypeRegistry m_registry;
  ::sup::dto::AnyTypeBuilder m_builder;

  AnyTypeBuilderImpl() : m_builder(&m_registry) {}
};

AnyTypeBuilder::AnyTypeBuilder(const pvxs::TypeDef &pvxs_type) : p_impl(new AnyTypeBuilderImpl)
{
  p_impl->m_pvxs_type = pvxs_type;
}

dto::AnyType AnyTypeBuilder::MoveAnyType() const
{
  // there is no way to get TypeCode from TypeDef without construction of Value
  auto value = p_impl->m_pvxs_type.create();

  return dto::AnyType(GetAnyTypeCode(value.type()));
}

AnyTypeBuilder::~AnyTypeBuilder() = default;

}  // namespace epics
}  // namespace sup
