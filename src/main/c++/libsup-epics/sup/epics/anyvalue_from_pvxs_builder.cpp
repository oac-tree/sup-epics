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

  AnyValueFromPVXSBuilderImpl() = default;
};

AnyValueFromPVXSBuilder::AnyValueFromPVXSBuilder(const pvxs::Value& pvxs_value)
    : p_impl(new AnyValueFromPVXSBuilderImpl)
{
}

dto::AnyType AnyValueFromPVXSBuilder::MoveAnyType() const
{
  return {};
}

AnyValueFromPVXSBuilder::~AnyValueFromPVXSBuilder() = default;

}  // namespace epics
}  // namespace sup
