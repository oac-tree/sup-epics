/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Gennady Pospelov (IO)
 *
 * Copyright (c) : 2010-2026 ITER Organization,
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
  bool m_struct_array_mode_request{false};

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
}

void PvxsTypeBuilder::EmptyEpilog(const sup::dto::AnyType* anytype)
{
  (void)anytype;
}

void PvxsTypeBuilder::StructProlog(const sup::dto::AnyType* anytype)
{
  (void)anytype;

  if (p_impl->m_struct_array_mode_request)
  {
    p_impl->m_struct_array_mode_request = false;
    // We are here because sup-dto wants to create a struct inside an array.

    // Create StructA which will play the role of array + struct simultaneously
    p_impl->m_struct_stack.push(
        {::pvxs::TypeDef(::pvxs::TypeCode::StructA, anytype->GetTypeName(), {}),
         ::pvxs::TypeCode::StructA});

    return;
  }

  const auto type_code = GetPVXSTypeCode(*anytype);
  p_impl->m_struct_stack.push({::pvxs::TypeDef(type_code, anytype->GetTypeName(), {}), type_code});
}

void PvxsTypeBuilder::StructMemberSeparator() {}

void PvxsTypeBuilder::StructEpilog(const sup::dto::AnyType* anytype)
{
  (void)anytype;

  if (p_impl->IsInArrayOfStructMode())
  {
    // no need to finalize a struct, have to finalize array (StructA) on the next iteration
    return;
  }

  p_impl->m_last_processed = p_impl->m_struct_stack.top().pvxs_typedef;
  p_impl->m_struct_stack.pop();
}

void PvxsTypeBuilder::MemberProlog(const sup::dto::AnyType* anytype, const std::string& member_name)
{
  (void)anytype;
  (void)member_name;
}

void PvxsTypeBuilder::MemberEpilog(const sup::dto::AnyType* anytype, const std::string& member_name)
{
  (void)anytype;
  (void)member_name;
  auto& top = p_impl->m_struct_stack.top();
  top.pvxs_typedef += {p_impl->m_last_processed.as(member_name)};
}

void PvxsTypeBuilder::ArrayProlog(const sup::dto::AnyType* anytype)
{
  const auto type_code = GetPVXSTypeCode(*anytype);
  if (type_code == pvxs::TypeCode::StructA)
  {
    // Just a reminder that PVXS doesn't have a "struct" inside an "array", it has separate
    // StructA. object. We will create it later, on StructPrologue() section, to be able to plugin
    // the name of a struct object into StructA.
    p_impl->m_struct_array_mode_request = true;
    return;
  }

  p_impl->m_struct_stack.push({::pvxs::TypeDef(type_code), type_code});
}

void PvxsTypeBuilder::ArrayElementSeparator() {}

void PvxsTypeBuilder::ArrayEpilog(const sup::dto::AnyType* anytype)
{
  (void)anytype;

  p_impl->m_last_processed = p_impl->m_struct_stack.top().pvxs_typedef;
  p_impl->m_struct_stack.pop();
}

void PvxsTypeBuilder::ScalarProlog(const sup::dto::AnyType* anytype)
{
  (void)anytype;
}

void PvxsTypeBuilder::ScalarEpilog(const sup::dto::AnyType* anytype)
{
  p_impl->m_last_processed = GetPVXSTypeCode(*anytype);
}

}  // namespace epics

}  // namespace sup
