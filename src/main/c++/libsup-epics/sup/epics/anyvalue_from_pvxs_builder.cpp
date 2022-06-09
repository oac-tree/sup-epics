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
#include <stack>
#include <list>

namespace sup
{
namespace epics
{

struct Node
{
  ::pvxs::Value* parent;
  ::pvxs::Value* child;
};

struct AnyValueFromPVXSBuilder::AnyValueFromPVXSBuilderImpl
{
  ::pvxs::Value m_pvxs_value;
  AnyValueBuildAdapter m_builder;
  ::sup::dto::AnyValue m_result;
  std::stack<Node> m_pvxs_stack;

  std::list<::pvxs::Value*> GetChildrenReverse(const pvxs::Value& pvxs_value)
  {
    std::list<::pvxs::Value*> result;
    for(auto fld : pvxs_value.ichildren())
    {
      result.push_front(&fld);
    }
    return result;
  }

  void ProcessPvxsValue(const pvxs::Value& pvxs_value)
  {
    m_pvxs_value = pvxs_value;

    auto code = GetAnyTypeCode(m_pvxs_value.type());

    if (::sup::dto::IsScalarTypeCode(code))
    {
      m_result = ::sup::dto::AnyValue(::sup::dto::AnyType(code));
      AssignPVXSValueToAnyValueScalar(m_pvxs_value, m_result);
    }
    else if(::sup::dto::IsStructTypeCode(code))
    {
      m_builder.StartStruct();
      for(auto child : GetChildrenReverse(m_pvxs_value))
      {
        m_pvxs_stack.push({&m_pvxs_value, child});
      }
      ProcessStack();
      m_builder.EndStruct();
    }

  }

  void ProcessStack()
  {
    ::pvxs::Value* current_parent(&m_pvxs_value);
    while(!m_pvxs_stack.empty())
    {
      auto node = m_pvxs_stack.top();
      m_pvxs_stack.pop();



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
