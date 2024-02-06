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

#ifndef SUP_EPICS_PVXS_VALUE_BUILDER_H_
#define SUP_EPICS_PVXS_VALUE_BUILDER_H_

#include <sup/dto/i_any_visitor.h>
#include <sup/epics/utils/dto_types_fwd.h>

#include <memory>

namespace sup
{
namespace epics
{

//! Visitor to construct pvxs::Value from sup::dto::AnyValue.
//! Requires propery constructed ::pvxs::TypeDef in the constructor.

class PvxsValueBuilder : public sup::dto::IAnyVisitor<const sup::dto::AnyValue>
{
public:
  explicit PvxsValueBuilder(::pvxs::TypeDef type_def);
  ~PvxsValueBuilder() override;

  pvxs::Value GetPVXSValue() const;

  void EmptyProlog(const sup::dto::AnyValue* anyvalue) override;
  void EmptyEpilog(const sup::dto::AnyValue* anyvalue) override;

  void StructProlog(const sup::dto::AnyValue* anyvalue) override;
  void StructMemberSeparator() override;
  void StructEpilog(const sup::dto::AnyValue* anyvalue) override;

  void MemberProlog(const sup::dto::AnyValue* anyvalue, const std::string& member_name) override;
  void MemberEpilog(const sup::dto::AnyValue* anyvalue, const std::string& member_name) override;

  void ArrayProlog(const sup::dto::AnyValue* anyvalue) override;
  void ArrayElementSeparator() override;
  void ArrayEpilog(const sup::dto::AnyValue* anyvalue) override;

  void ScalarProlog(const sup::dto::AnyValue* anyvalue) override;
  void ScalarEpilog(const sup::dto::AnyValue* anyvalue) override;

private:
  struct PvxsValueBuilderImpl;
  std::unique_ptr<PvxsValueBuilderImpl> p_impl;
};

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_PVXS_VALUE_BUILDER_H_
