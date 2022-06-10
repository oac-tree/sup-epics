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

struct AnyValueBuildAdapter::AnyValueBuildAdapterImpl
{
  ::sup::dto::AnyValue m_result;
  std::stack<::sup::dto::AnyValue> m_struct_stack;

  ::sup::dto::AnyValue *GetTopStruct()
  {
    return m_struct_stack.empty() ? nullptr : &m_struct_stack.top();
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
    if (name.empty())
    {
      m_result = anyvalue;
    }
    else
    {
      ValidateTop();
      GetTopStruct()->AddMember(name, anyvalue);
    }
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

void AnyValueBuildAdapter::AddScalar(const std::string &member_name, const dto::AnyValue &value)
{
  if (!::sup::dto::IsScalarValue(value))
  {
    throw std::runtime_error("Given AnyValue is not a scalar");
  }
  p_impl->AddMember(member_name, value);
}

void AnyValueBuildAdapter::StartStruct(const std::string &struct_name)
{
  p_impl->m_struct_stack.emplace(::sup::dto::EmptyStruct(struct_name));
}

//! Finalise current struct. If `member_name` is not empty it is assumed that the struct should
//! be added to the previos struct as a field.
void AnyValueBuildAdapter::EndStruct(const std::string &member_name)
{
  auto top_struct = p_impl->m_struct_stack.top();
  p_impl->m_struct_stack.pop();
  p_impl->AddMember(member_name, top_struct);
}

AnyValueBuildAdapter::~AnyValueBuildAdapter() = default;

}  // namespace epics
}  // namespace sup
