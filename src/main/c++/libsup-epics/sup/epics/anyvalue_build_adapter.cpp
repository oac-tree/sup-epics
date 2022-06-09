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

#include <iostream>
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

  ::sup::dto::AnyValue *GetTopStruct()
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

  void AddMember(const std::string &name, const ::sup::dto::AnyValue &anyvalue)
  {
    std::cout << "AddMember " << name << " top:" << GetTopStruct() << std::endl;
    ValidateTop();
    GetTopStruct()->AddMember(name, anyvalue);
  }
};

AnyValueBuildAdapter::AnyValueBuildAdapter() : p_impl(new AnyValueBuildAdapterImpl) {}

dto::AnyValue AnyValueBuildAdapter::MoveAnyValue() const
{
  return std::move(p_impl->m_result);
}

void AnyValueBuildAdapter::Bool(const std::string &member_name, dto::boolean value)
{
  p_impl->AddMember(member_name, ::sup::dto::AnyValue(value));
}

void AnyValueBuildAdapter::Int8(const std::string &member_name, dto::int8 value)
{
  p_impl->AddMember(member_name, ::sup::dto::AnyValue(value));
}

void AnyValueBuildAdapter::UInt8(const std::string &member_name, dto::uint8 value)
{
  p_impl->AddMember(member_name, ::sup::dto::AnyValue(value));
}

void AnyValueBuildAdapter::Int16(const std::string &member_name, dto::int16 value)
{
  p_impl->AddMember(member_name, ::sup::dto::AnyValue(value));
}

void AnyValueBuildAdapter::UInt16(const std::string &member_name, dto::uint16 value)
{
  p_impl->AddMember(member_name, ::sup::dto::AnyValue(value));
}

void AnyValueBuildAdapter::Int32(const std::string &member_name, dto::int32 value)
{
  p_impl->AddMember(member_name, ::sup::dto::AnyValue(value));
}

void AnyValueBuildAdapter::UInt32(const std::string &member_name, dto::uint32 value)
{
  p_impl->AddMember(member_name, ::sup::dto::AnyValue(value));
}

void AnyValueBuildAdapter::Int64(const std::string &member_name, dto::int64 value)
{
  p_impl->AddMember(member_name, ::sup::dto::AnyValue(value));
}

void AnyValueBuildAdapter::UInt64(const std::string &member_name, dto::uint64 value)
{
  p_impl->AddMember(member_name, ::sup::dto::AnyValue(value));
}

void AnyValueBuildAdapter::Float32(const std::string &member_name, dto::float32 value)
{
  p_impl->AddMember(member_name, ::sup::dto::AnyValue(value));
}

void AnyValueBuildAdapter::Float64(const std::string &member_name, dto::float64 value)
{
  p_impl->AddMember(member_name, ::sup::dto::AnyValue(value));
}

void AnyValueBuildAdapter::String(const std::string &member_name, const std::string &value)
{
  p_impl->AddMember(member_name, ::sup::dto::AnyValue(value));
}

void AnyValueBuildAdapter::StartStruct(const std::string &struct_name)
{
  BuildNode node{{}, ::sup::dto::EmptyStruct(struct_name)};
  p_impl->m_struct_stack.emplace(node);
}

//! Finalise current struct and add it as a member to the previous struct.
void AnyValueBuildAdapter::EndStruct(const std::string &member_name)
{
  ::sup::dto::AnyValue top_struct = p_impl->m_struct_stack.top().value;
  p_impl->m_struct_stack.pop();

  if (member_name.empty())
  {
    p_impl->m_result = top_struct;
  }
  else
  {
    p_impl->AddMember(member_name, top_struct);
  }
}

AnyValueBuildAdapter::~AnyValueBuildAdapter() = default;

}  // namespace epics
}  // namespace sup
