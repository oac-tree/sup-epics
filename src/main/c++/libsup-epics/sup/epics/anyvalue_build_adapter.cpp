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

#include "anyvalue_build_adapter.h"

#include <sup/dto/anytype.h>
#include <sup/dto/anyvalue.h>

#include <stack>
#include <stdexcept>

namespace sup
{
namespace epics
{

struct BuildNode
{
  std::string name;
  ::sup::dto::AnyValue value;
};

struct AnyValueBuildAdapter::AnyValueBuildAdapterImpl
{
  ::sup::dto::AnyValue m_result;
  std::stack<BuildNode> m_struct_stack;

  ::sup::dto::AnyValue* GetTopStruct()
  {
    return m_struct_stack.empty() ? nullptr : &m_struct_stack.top().value;
  }

  void ValidateTop()
  {
    if (!GetTopStruct())
    {
      throw std::runtime_error("Stack is empty");
    }
  }

  void AddMember(const std::string& name, const ::sup::dto::AnyValue& anyvalue)
  {
    ValidateTop();
    GetTopStruct()->AddMember(name, anyvalue);
  }
};

AnyValueBuildAdapter::AnyValueBuildAdapter() : p_impl(new AnyValueBuildAdapterImpl) {}

dto::AnyValue AnyValueBuildAdapter::MoveAnyValue() const
{
  return std::move(p_impl->m_result);
}

void AnyValueBuildAdapter::Bool(const std::string& name, dto::boolean value)
{
  p_impl->AddMember(name, ::sup::dto::AnyValue(value));
}

void AnyValueBuildAdapter::Int32(const std::string& name, dto::int32 value)
{
  p_impl->AddMember(name, ::sup::dto::AnyValue(value));
}

void AnyValueBuildAdapter::StartStruct()
{
  BuildNode node{std::string(), ::sup::dto::AnyValue({})};
  p_impl->m_struct_stack.emplace(node);
}

void AnyValueBuildAdapter::EndStruct()
{
  p_impl->m_result = p_impl->m_struct_stack.top().value;
  p_impl->m_struct_stack.pop();
}

AnyValueBuildAdapter::~AnyValueBuildAdapter() = default;

}  // namespace epics
}  // namespace sup
