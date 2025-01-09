/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Gennady Pospelov (IO)
 *
 * Copyright (c) : 2010-2025 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 *****************************************************************************/

#include <pvxs/data.h>
#include <sup/dto/anyvalue.h>
#include <sup/epics/utils/dto_scalar_conversion_utils.h>
#include <sup/epics/utils/pvxs_builder_nodes.h>
#include <sup/epics/utils/pvxs_utils.h>
#include <sup/epics/utils/pvxs_value_builder.h>

#include <stack>
#include <stdexcept>

namespace
{

//! Creates pvxs::Value from given type definition.
pvxs::Value CreateValueFromType(const ::pvxs::TypeDef &type_def)
{
  // We want to create pvxs::Value from given PVXS TypeDef. However, if TypeDef is an empty type,
  // we want to return default constructed pvxs::Value.

  // The method is just an equivalent of
  // return type_def.IsEmpty() ? pvxs::Value() : type_def.create();

  // Unfortunately it can't be implemented like that since IsEmpty() method wasn't provided by the
  // developer.

  const std::string kEmptyTypeDefWhat("Empty TypeDef");

  pvxs::Value result;

  // We try to create pvxs::Value, and if exception was thrown, and the description denotes
  // that TypeDef is empty, we return deault constructed value.
  try
  {
    result = type_def.create();
  }
  catch (const std::logic_error &ex)
  {
    if (ex.what() != kEmptyTypeDefWhat)
    {
      throw;
    }
  }
  return result;
}

}  // namespace

namespace sup
{
namespace epics
{

struct PvxsValueBuilder::PvxsValueBuilderImpl
{
  std::stack<std::unique_ptr<PvxsBuilderNode>> m_nodes;

  bool IsScalarArrayMode() const { return IsScalarArray(GetCurrentNode()->GetPvxsValueRef()); }

  template <typename T, typename... Args>
  void ProcessComponent(Args &&...args)
  {
    auto component = std::make_unique<T>((args)...);
    m_nodes.push(std::move(component));
  }

  pvxs::Value &GetCurrent() { return m_nodes.top()->GetPvxsValueRef(); }

  PvxsBuilderNode *GetCurrentNode() const
  {
    if (m_nodes.empty())
    {
      throw std::runtime_error("Node is empty");
    }
    return m_nodes.top().get();
  }
};

PvxsValueBuilder::PvxsValueBuilder(::pvxs::TypeDef type_def) : p_impl(std::make_unique<PvxsValueBuilderImpl>())
{
  p_impl->ProcessComponent<PvxsBuilderNode>(CreateValueFromType(type_def));
}

PvxsValueBuilder::~PvxsValueBuilder() = default;

pvxs::Value PvxsValueBuilder::GetPVXSValue() const
{
  return p_impl->m_nodes.empty() ? pvxs::Value() : p_impl->m_nodes.top()->GetPvxsValue();
}

void PvxsValueBuilder::EmptyProlog(const sup::dto::AnyValue *anyvalue)
{
  (void)anyvalue;
}

void PvxsValueBuilder::EmptyEpilog(const sup::dto::AnyValue *anyvalue)
{
  (void)anyvalue;
}

void PvxsValueBuilder::StructProlog(const sup::dto::AnyValue *anyvalue)
{
  (void)anyvalue;
  p_impl->ProcessComponent<PvxsBuilderNode>(p_impl->GetCurrent());
}

void PvxsValueBuilder::StructMemberSeparator() {}

void PvxsValueBuilder::StructEpilog(const sup::dto::AnyValue *anyvalue)
{
  (void)anyvalue;
  p_impl->m_nodes.pop();
}

void PvxsValueBuilder::MemberProlog(const sup::dto::AnyValue *anyvalue,
                                    const std::string &member_name)
{
  (void)anyvalue;
  p_impl->ProcessComponent<PvxsBuilderNode>(p_impl->GetCurrent()[member_name]);
}

void PvxsValueBuilder::MemberEpilog(const sup::dto::AnyValue *anyvalue,
                                    const std::string &member_name)
{
  (void)anyvalue;
  (void)member_name;
  p_impl->m_nodes.pop();
}

void PvxsValueBuilder::ArrayProlog(const sup::dto::AnyValue *anyvalue)
{
  if (p_impl->IsScalarArrayMode())
  {
    p_impl->ProcessComponent<ScalarArrayBuilderNode>(p_impl->GetCurrent(), anyvalue);
  }
  else
  {
    p_impl->ProcessComponent<StructArrayBuilderNode>(p_impl->GetCurrent(), anyvalue);
  }
}

void PvxsValueBuilder::ArrayElementSeparator()
{
  p_impl->GetCurrentNode()->ArrayElementSeparator();
}

void PvxsValueBuilder::ArrayEpilog(const sup::dto::AnyValue *anyvalue)
{
  (void)anyvalue;
  p_impl->GetCurrentNode()->Freeze();
  p_impl->m_nodes.pop();
}

void PvxsValueBuilder::ScalarProlog(const sup::dto::AnyValue *anyvalue)
{
  if (!p_impl->GetCurrentNode()->IsScalarArrayNode())
  {
    AssignAnyValueToPVXSValueScalar(*anyvalue, p_impl->GetCurrent());
  }
}

void PvxsValueBuilder::ScalarEpilog(const sup::dto::AnyValue *anyvalue)
{
  (void)anyvalue;
}

}  // namespace epics

}  // namespace sup
