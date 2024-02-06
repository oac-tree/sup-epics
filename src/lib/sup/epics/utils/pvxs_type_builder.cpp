/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Gennady Pospelov (IO)
 *
 * Copyright (c) : 2010-2024 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 *****************************************************************************/

#include "pvxs_type_builder.h"

#include <pvxs/data.h>
#include <sup/dto/anytype.h>
#include <sup/epics/utils/dto_scalar_conversion_utils.h>
#include <sup/epics/utils/dto_typecode_conversion_utils.h>

#include <stack>
#include <stdexcept>

namespace sup
{
namespace epics
{

struct PvxsTypeBuilder::PvxsTypeBuilderImpl
{
  ::pvxs::TypeDef m_last_processed;
  std::stack<::pvxs::TypeDef> m_struct_stack;
};

PvxsTypeBuilder::PvxsTypeBuilder() : p_impl(new PvxsTypeBuilderImpl) {}

PvxsTypeBuilder::~PvxsTypeBuilder() = default;

pvxs::TypeDef PvxsTypeBuilder::GetPVXSType() const
{
  return p_impl->m_last_processed;
}

void PvxsTypeBuilder::EmptyProlog(const sup::dto::AnyType* anytype)
{
  (void)anytype;
  //  std::cout << "EmptyProlog() value:" << anytype << std::endl;
}

void PvxsTypeBuilder::EmptyEpilog(const sup::dto::AnyType* anytype)
{
  (void)anytype;
  //  std::cout << "EmptyEpilog() value:" << anytype << std::endl;
}

void PvxsTypeBuilder::StructProlog(const sup::dto::AnyType* anytype)
{
  (void)anytype;
  //  std::cout << "StructProlog() value:" << anytype << std::endl;
  p_impl->m_struct_stack.push(
      ::pvxs::TypeDef(GetPVXSTypeCode(*anytype), anytype->GetTypeName(), {}));
}

void PvxsTypeBuilder::StructMemberSeparator()
{
  //  std::cout << "StructMemberSeparator() " << std::endl;
}

void PvxsTypeBuilder::StructEpilog(const sup::dto::AnyType* anytype)
{
  (void)anytype;
  //  std::cout << "StructEpilog() value:" << anytype << std::endl;
  p_impl->m_last_processed = p_impl->m_struct_stack.top();
  p_impl->m_struct_stack.pop();
}

void PvxsTypeBuilder::MemberProlog(const sup::dto::AnyType* anytype, const std::string& member_name)
{
  (void)anytype;
  (void)member_name;
  //  std::cout << "MemberProlog() " << anytype << " " << member_name << std::endl;
}

void PvxsTypeBuilder::MemberEpilog(const sup::dto::AnyType* anytype, const std::string& member_name)
{
  (void)anytype;
  (void)member_name;
  //  std::cout << "MemberEpilog() " << anytype << " " << member_name << " " << std::endl;
  auto& top = p_impl->m_struct_stack.top();
  top += {p_impl->m_last_processed.as(member_name)};
}

void PvxsTypeBuilder::ArrayProlog(const sup::dto::AnyType* anytype)
{
  //  std::cout << "ArrayProlog() value:" << anytype << " " << GetPVXSTypeCode(*anytype) << " " <<
  //  std::endl;
  p_impl->m_struct_stack.push(GetPVXSTypeCode(*anytype));
}

void PvxsTypeBuilder::ArrayElementSeparator()
{
  //  std::cout << "AddArrayElementSeparator() " << std::endl;
}

void PvxsTypeBuilder::ArrayEpilog(const sup::dto::AnyType* anytype)
{
  (void)anytype;
  //  std::cout << "AddArrayEpilog() value:" << anytype << std::endl;
  p_impl->m_last_processed = p_impl->m_struct_stack.top();
  p_impl->m_struct_stack.pop();
}

void PvxsTypeBuilder::ScalarProlog(const sup::dto::AnyType* anytype)
{
  (void)anytype;
  //  std::cout << "ScalarProlog() value:" << anytype << std::endl;
}

void PvxsTypeBuilder::ScalarEpilog(const sup::dto::AnyType* anytype)
{
  //  std::cout << "ScalarEpilog() value:" << anytype << std::endl;

  p_impl->m_last_processed = GetPVXSTypeCode(*anytype);
}

}  // namespace epics

}  // namespace sup
