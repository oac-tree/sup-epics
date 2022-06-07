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

#ifndef SUP_EPICS_PVXS_TYPE_BUILDER_H_
#define SUP_EPICS_PVXS_TYPE_BUILDER_H_

#include <sup/dto/i_any_visitor.h>
#include <sup/epics/dto_types_fwd.h>

#include <memory>

namespace sup
{
namespace epics
{

//! Visitor to construct pvxs::TypeDef from sup::dto::AnyType.

class PvxsTypeBuilder : public sup::dto::IAnyVisitor<const sup::dto::AnyType>
{
public:
  PvxsTypeBuilder();
  ~PvxsTypeBuilder() override;

  pvxs::TypeDef GetPVXSType() const;

  void EmptyProlog(const sup::dto::AnyType* anytype) override;
  void EmptyEpilog(const sup::dto::AnyType* anytype) override;

  void StructProlog(const sup::dto::AnyType* anytype) override;
  void StructMemberSeparator() override;
  void StructEpilog(const sup::dto::AnyType* anytype) override;

  void MemberProlog(const sup::dto::AnyType* anytype, const std::string& member_name) override;
  void MemberEpilog(const sup::dto::AnyType* anytype, const std::string& member_name) override;

  void ArrayProlog(const sup::dto::AnyType* anytype) override;
  void ArrayElementSeparator() override;
  void ArrayEpilog(const sup::dto::AnyType* anytype) override;

  void ScalarProlog(const sup::dto::AnyType* anytype) override;
  void ScalarEpilog(const sup::dto::AnyType* anytype) override;

  void UnboundedArrayProlog(const sup::dto::AnyType* anytype) override;
  void UnboundedArrayEpilog(const sup::dto::AnyType* anytype) override;

private:
  struct PvxsTypeBuilderImpl;
  std::unique_ptr<PvxsTypeBuilderImpl> p_impl;
};

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_PVXS_TYPE_BUILDER_H_
