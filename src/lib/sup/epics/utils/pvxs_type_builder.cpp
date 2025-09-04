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
 * SPDX-License-Identifier: MIT
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file LICENSE located in the top level directory
 * of the distribution package.
 *****************************************************************************/

#include "pvxs_type_builder.h"

#include <sup/dto/anytype.h>
#include <sup/epics/utils/dto_scalar_conversion_utils.h>
#include <sup/epics/utils/dto_typecode_conversion_utils.h>

#include <pvxs/data.h>

#include <stack>

namespace sup
{
namespace epics
{

namespace
{

struct Node
{
  ::pvxs::TypeDef pvxs_typedef;
  ::pvxs::TypeCode pvxs_typecode;  //!< saving TypeCode since TypeDef has no getter for TypeCode
};

}  // namespace

struct PvxsTypeBuilder::PvxsTypeBuilderImpl
{
  ::pvxs::TypeDef m_last_processed;
  std::stack<Node> m_struct_stack;

  bool IsInArrayOfStructMode() const
  {
    return !m_struct_stack.empty()
           && (m_struct_stack.top().pvxs_typecode == ::pvxs::TypeCode::StructA);
  }
};

PvxsTypeBuilder::PvxsTypeBuilder() : p_impl(std::make_unique<PvxsTypeBuilderImpl>()) {}

PvxsTypeBuilder::~PvxsTypeBuilder() = default;

pvxs::TypeDef PvxsTypeBuilder::GetPVXSType() const
{
  return p_impl->m_last_processed;
}

void PvxsTypeBuilder::EmptyProlog(const sup::dto::AnyType* anytype)
{
  (void)anytype;
  // std::cout << "EmptyProlog() value:" << anytype << std::endl;
}

void PvxsTypeBuilder::EmptyEpilog(const sup::dto::AnyType* anytype)
{
  (void)anytype;
  // std::cout << "EmptyEpilog() value:" << anytype << std::endl;
}

void PvxsTypeBuilder::StructProlog(const sup::dto::AnyType* anytype)
{
  (void)anytype;

  // std::cout << "StructProlog() value:" << anytype << std::endl;

  if (p_impl->IsInArrayOfStructMode())
  {
    // no need to start a Struct, StructA was already started on Array's prologue

    // NOTE Please note that here we have a limitation of our builder. StructA was already created
    // and we can't change its name,
    // see also TEST_F(PvxsValueBasicsTests, CreateTypeDefForArrayOfStructsNamed)
    return;
  }

  const auto type_code = GetPVXSTypeCode(*anytype);
  p_impl->m_struct_stack.push({::pvxs::TypeDef(type_code, anytype->GetTypeName(), {}), type_code});
}

void PvxsTypeBuilder::StructMemberSeparator()
{
  // std::cout << "StructMemberSeparator() " << std::endl;
}

void PvxsTypeBuilder::StructEpilog(const sup::dto::AnyType* anytype)
{
  (void)anytype;

  if (p_impl->IsInArrayOfStructMode())
  {
    // no need to finalize a struct, have to finalize array (StructA) on the next iteration
    return;
  }

  // std::cout << "StructEpilog() value:" << anytype << std::endl;
  p_impl->m_last_processed = p_impl->m_struct_stack.top().pvxs_typedef;
  p_impl->m_struct_stack.pop();
}

void PvxsTypeBuilder::MemberProlog(const sup::dto::AnyType* anytype, const std::string& member_name)
{
  (void)anytype;
  (void)member_name;
  // std::cout << "MemberProlog() " << anytype << " " << member_name << std::endl;
}

void PvxsTypeBuilder::MemberEpilog(const sup::dto::AnyType* anytype, const std::string& member_name)
{
  (void)anytype;
  (void)member_name;
  // std::cout << "MemberEpilog() " << anytype << " " << member_name << " " << std::endl;
  auto& top = p_impl->m_struct_stack.top();
  top.pvxs_typedef += {p_impl->m_last_processed.as(member_name)};
}

void PvxsTypeBuilder::ArrayProlog(const sup::dto::AnyType* anytype)
{
  // std::cout << "ArrayProlog() value:" << anytype << " " << GetPVXSTypeCode(*anytype) << " "
  //           << std::endl;
  const auto type_code = GetPVXSTypeCode(*anytype);
  p_impl->m_struct_stack.push({::pvxs::TypeDef(type_code), type_code});
}

void PvxsTypeBuilder::ArrayElementSeparator()
{
  // std::cout << "AddArrayElementSeparator() " << std::endl;
}

void PvxsTypeBuilder::ArrayEpilog(const sup::dto::AnyType* anytype)
{
  (void)anytype;
  // std::cout << "AddArrayEpilog() value:" << anytype << std::endl;
  p_impl->m_last_processed = p_impl->m_struct_stack.top().pvxs_typedef;
  p_impl->m_struct_stack.pop();
}

void PvxsTypeBuilder::ScalarProlog(const sup::dto::AnyType* anytype)
{
  (void)anytype;
  // std::cout << "ScalarProlog() value:" << anytype << std::endl;
}

void PvxsTypeBuilder::ScalarEpilog(const sup::dto::AnyType* anytype)
{
  // std::cout << "ScalarEpilog() value:" << anytype << std::endl;

  p_impl->m_last_processed = GetPVXSTypeCode(*anytype);
}

}  // namespace epics

}  // namespace sup
