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
#include <stdexcept>

namespace
{

//! Creates pvxs::Value from given type definition.
pvxs::Value CreateValueFromType(const ::pvxs::TypeDef &type_def)
{
  // We want to create pvxs::Value from given PVXS TypeDef. However, if TypeDef is an empty type,
  // we want to return default constructed pvxs::Value.

  // The method is just an equivalent of
  // return type_def.SsEmpty() ? pvxs::Value() : type_def.create();
  // when no IsEmpty() method was provided by the developer.

  const std::string kEmptyTypeDefWhat("Empty TypeDef");

  pvxs::Value result;

  // The strange try/catch block is because we can't check if TypeDef is empty before trying to
  // construct pvxs::Value from it.
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
  //  pvxs::Value m_scalar;            //!< last processed scalar value
  //  pvxs::Value *m_parent{nullptr};  //! current parent
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
}

void PvxsValueBuilder::StructMemberSeparator()
{
  std::cout << "StructMemberSeparator() " << std::endl;
}

void PvxsValueBuilder::StructEpilog(const sup::dto::AnyValue *anyvalue)
{
  std::cout << "StructEpilog() value:" << anyvalue << std::endl;
}

void PvxsValueBuilder::MemberProlog(const sup::dto::AnyValue *anyvalue,
                                    const std::string &member_name)
{
  std::cout << "MemberProlog() " << anyvalue << " " << member_name << std::endl;

  //  ::pvxs::Value val = (*p_impl->m_current)[member_name];

  p_impl->m_current = p_impl->m_current[member_name];
}

void PvxsValueBuilder::MemberEpilog(const sup::dto::AnyValue *anyvalue,
                                    const std::string &member_name)
{
  std::cout << "MemberEpilog() " << anyvalue << " " << member_name << std::endl;
}

void PvxsValueBuilder::ArrayProlog(const sup::dto::AnyValue *anyvalue)
{
  std::cout << "ArrayProlog() value:" << anyvalue << std::endl;
}

void PvxsValueBuilder::ArrayElementSeparator()
{
  std::cout << "AddArrayElementSeparator() " << std::endl;
}

void PvxsValueBuilder::ArrayEpilog(const sup::dto::AnyValue *anyvalue)
{
  std::cout << "AddArrayEpilog() value:" << anyvalue << std::endl;
}

void PvxsValueBuilder::ScalarProlog(const sup::dto::AnyValue *anyvalue)
{
  std::cout << "ScalarProlog() value:" << anyvalue << std::endl;
  //  p_impl->m_scalar = GetPVXSValueFromScalar(*anyvalue);
}

void PvxsValueBuilder::ScalarEpilog(const sup::dto::AnyValue *anyvalue)
{
  std::cout << "ScalarEpilog() value:" << anyvalue << std::endl;
  //  if (!p_impl->m_parent)
  //  {
  //    // If no parent exists, then we are processing scalar based AnyValue.
  //    // We asume that this should be our result.
  //    p_impl->m_result = p_impl->m_scalar;
  //  }
  //  p_impl->m_current = GetPVXSValueFromScalar(*anyvalue);
  AssignPVXSValueFromScalar(*anyvalue, p_impl->m_current);
}

}  // namespace sup::epics
