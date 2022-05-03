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

#include "AnyType.h"
#include "sup/epics/dtoconversionutils.h"

#include <pvxs/data.h>

#include <iostream>
#include <stack>
#include <stdexcept>

namespace sup::epics
{

struct PvxsTypeBuilder::PvxsTypeBuilderImpl
{
  ::pvxs::TypeDef m_result;
  std::stack<::pvxs::TypeDef> m_struct_def;
  ::pvxs::TypeDef m_last_struct;

  //  std::stack<::pvxs::Member> m_members;

  bool IsAtTop() const { return m_struct_def.empty(); }
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
  std::cout << "StructProlog() value:" << anytype << std::endl;
  p_impl->m_struct_def.push(::pvxs::TypeDef(GetPVXSTypeCode(*anytype), anytype->GetTypeName(), {}));
  std::cout << "top_address" << &p_impl->m_struct_def.top() << std::endl;
  std::cout << "xxx " << p_impl->m_struct_def.size() << std::endl;
}

void PvxsTypeBuilder::StructMemberSeparator()
{
  std::cout << "StructMemberSeparator() " << std::endl;
}

void PvxsTypeBuilder::StructEpilog(const sup::dto::AnyType* anytype)
{
  std::cout << "StructEpilog() value:" << anytype << std::endl;
  p_impl->m_result = p_impl->m_struct_def.top();
  p_impl->m_last_struct = p_impl->m_struct_def.top();
  p_impl->m_struct_def.pop();
}

void PvxsTypeBuilder::MemberProlog(const sup::dto::AnyType* anytype, const std::string& member_name)
{
  std::cout << "MemberProlog() " << anytype << " " << member_name << std::endl;
}

void PvxsTypeBuilder::MemberEpilog(const sup::dto::AnyType* anytype, const std::string& member_name)
{
  std::cout << "MemberEpilog() " << anytype << " " << member_name << " "
            << GetPVXSTypeCode(*anytype) << std::endl;
  std::cout << "top_address" << &p_impl->m_struct_def.top() << std::endl;
  auto& top = p_impl->m_struct_def.top();

  if (anytype->GetTypeCode() == ::sup::dto::TypeCode::Struct)
  {
    // We use DefType::as method which the developer of PVXS package has luckily provided.
    // But frankly speaking, we find such a way of constructing an object tree a bit cumbersome.
    top += {p_impl->m_last_struct.as(member_name)};
  }
  else
  {
    top += {::pvxs::Member(GetPVXSTypeCode(*anytype), member_name)};
  }

  std::cout << "xxx " << p_impl->m_struct_def.size() << std::endl;
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
  std::cout << "ScalarProlog() value:" << anytype << std::endl;
}

void PvxsTypeBuilder::ScalarEpilog(const sup::dto::AnyType* anytype)
{
  std::cout << "ScalarEpilog() value:" << anytype << std::endl;

  if (p_impl->IsAtTop())
  {
    // If there was no structure created, than AnyType is a scalar-based.
    // We assume that TypeDef corresponding to our scalar is the main result.
    p_impl->m_result = GetPVXSTypeCode(*anytype);
    //    p_impl->m_struct_def.push(GetPVXSTypeCode(*anytype));
  }
}

}  // namespace sup::epics
