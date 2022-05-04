/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP common components for EPICS network protocol
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

#include "sup/epics/pvxsvaluebuilder.h"

#include "sup/epics/dtoconversionutils.h"

#include <pvxs/data.h>

#include <iostream>
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

namespace sup::epics
{

struct PvxsValueBuilder::PvxsValueBuilderImpl
{
  pvxs::Value m_result;   //!< place for the result
  pvxs::Value m_current;  //! current position

  std::stack<::pvxs::Value> m_struct_stack;
  bool m_array_mode{false};
};

PvxsValueBuilder::PvxsValueBuilder(::pvxs::TypeDef type_def) : p_impl(new PvxsValueBuilderImpl)
{
  std::cout << " ----------------\n";
  p_impl->m_result = CreateValueFromType(type_def);
  p_impl->m_current = p_impl->m_result;
}

PvxsValueBuilder::~PvxsValueBuilder() = default;

pvxs::Value PvxsValueBuilder::GetPVXSValue() const
{
  return p_impl->m_result;
}

void PvxsValueBuilder::EmptyProlog(const sup::dto::AnyValue *anyvalue)
{
  std::cout << "EmptyProlog() value:" << anyvalue << std::endl;
}

void PvxsValueBuilder::EmptyEpilog(const sup::dto::AnyValue *anyvalue)
{
  std::cout << "EmptyEpilog() value:" << anyvalue << std::endl;
}

void PvxsValueBuilder::StructProlog(const sup::dto::AnyValue *anyvalue)
{
  std::cout << "StructProlog() value:" << anyvalue << " item:" << std::endl;
  p_impl->m_struct_stack.push(p_impl->m_current);
}

void PvxsValueBuilder::StructMemberSeparator()
{
  std::cout << "StructMemberSeparator() " << std::endl;
}

void PvxsValueBuilder::StructEpilog(const sup::dto::AnyValue *anyvalue)
{
  std::cout << "StructEpilog() value:" << anyvalue << std::endl;
  p_impl->m_current = p_impl->m_struct_stack.top();
  p_impl->m_struct_stack.pop();
}

void PvxsValueBuilder::MemberProlog(const sup::dto::AnyValue *anyvalue,
                                    const std::string &member_name)
{
  std::cout << "MemberProlog() " << anyvalue << " " << member_name << std::endl;

  p_impl->m_current = p_impl->m_current[member_name];
}

void PvxsValueBuilder::MemberEpilog(const sup::dto::AnyValue *anyvalue,
                                    const std::string &member_name)
{
  std::cout << "MemberEpilog() " << anyvalue << " " << member_name << std::endl;
  p_impl->m_current = p_impl->m_struct_stack.top();
}

void PvxsValueBuilder::ArrayProlog(const sup::dto::AnyValue *anyvalue)
{
  std::cout << "ArrayProlog() value:" << anyvalue << std::endl;
  p_impl->m_array_mode = true;
}

void PvxsValueBuilder::ArrayElementSeparator()
{
  std::cout << "AddArrayElementSeparator() " << std::endl;
}

void PvxsValueBuilder::ArrayEpilog(const sup::dto::AnyValue *anyvalue)
{
  std::cout << "AddArrayEpilog() value:" << anyvalue << std::endl;
  p_impl->m_array_mode = false;
  AssignPVXSValueFromScalarArray(*anyvalue, p_impl->m_current);
}

void PvxsValueBuilder::ScalarProlog(const sup::dto::AnyValue *anyvalue)
{
  std::cout << "ScalarProlog() value:" << anyvalue << std::endl;
}

void PvxsValueBuilder::ScalarEpilog(const sup::dto::AnyValue *anyvalue)
{
  std::cout << "ScalarEpilog() value:" << anyvalue << std::endl;

  if (!p_impl->m_array_mode)
  {
    AssignPVXSValueFromScalar(*anyvalue, p_impl->m_current);
  }
}

}  // namespace sup::epics
