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

#include "sup/epics/pvxstypebuilder.h"

#include "sup/epics/dtoconversionutils.h"

#include <pvxs/data.h>

#include <iostream>
#include <stack>
#include <stdexcept>

namespace sup::epics
{

struct PvxsTypeBuilder::PvxsTypeBuilderImpl
{
  pvxs::TypeDef m_result;
  std::stack<pvxs::TypeCode> m_type_code;
};

PvxsTypeBuilder::PvxsTypeBuilder() : p_impl(new PvxsTypeBuilderImpl) {}

PvxsTypeBuilder::~PvxsTypeBuilder() = default;

pvxs::TypeDef PvxsTypeBuilder::GetPVXSType() const
{
  return p_impl->m_result;
}

void PvxsTypeBuilder::EmptyProlog(const sup::dto::AnyType* anytype)
{
  std::cout << "EmptyProlog() value:" << anytype << std::endl;
}

void PvxsTypeBuilder::EmptyEpilog(const sup::dto::AnyType* anytype)
{
  std::cout << "EmptyEpilog() value:" << anytype << std::endl;
}

void PvxsTypeBuilder::StructProlog(const sup::dto::AnyType* anytype)
{
  std::cout << "StructProlog() value:" << anytype << " item:" << std::endl;
}

void PvxsTypeBuilder::StructMemberSeparator()
{
  std::cout << "StructMemberSeparator() " << std::endl;
}

void PvxsTypeBuilder::StructEpilog(const sup::dto::AnyType* anytype)
{
  std::cout << "StructEpilog() value:" << anytype << std::endl;
}

void PvxsTypeBuilder::MemberProlog(const sup::dto::AnyType* anytype, const std::string& member_name)
{
  std::cout << "MemberProlog() " << anytype << " " << member_name << std::endl;
}

void PvxsTypeBuilder::MemberEpilog(const sup::dto::AnyType* anytype, const std::string& member_name)
{
  std::cout << "MemberEpilog() " << anytype << " " << member_name << std::endl;
}

void PvxsTypeBuilder::ArrayProlog(const sup::dto::AnyType* anytype)
{
  std::cout << "ArrayProlog() value:" << anytype << std::endl;
}

void PvxsTypeBuilder::ArrayElementSeparator()
{
  std::cout << "AddArrayElementSeparator() " << std::endl;
}

void PvxsTypeBuilder::ArrayEpilog(const sup::dto::AnyType* anytype)
{
  std::cout << "AddArrayEpilog() value:" << anytype << std::endl;
}

void PvxsTypeBuilder::ScalarProlog(const sup::dto::AnyType* anytype)
{
  p_impl->m_type_code.push(GetPVXSTypeCode(*anytype));
  std::cout << "ScalarProlog() value:" << anytype << "typecode:" << p_impl->m_type_code.top()
            << std::endl;
}

void PvxsTypeBuilder::ScalarEpilog(const sup::dto::AnyType* anytype)
{
  std::cout << "ScalarEpilog() value:" << anytype << std::endl;

  auto type_code = p_impl->m_type_code.top();
  p_impl->m_type_code.pop();

  // Empty stack means that the given scalar was the single constituent of AnyType.
  // So this must be our result.
  if (p_impl->m_type_code.empty())
  {
    p_impl->m_result = pvxs::TypeDef(type_code);
  }
}

}  // namespace sup::epics
