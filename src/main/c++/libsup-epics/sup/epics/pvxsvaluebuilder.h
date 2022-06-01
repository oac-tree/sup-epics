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

#ifndef SUP_EPICS_PVXSVALUEBUILDER_H
#define SUP_EPICS_PVXSVALUEBUILDER_H

#include "sup/dto/IAnyVisitor.h"
#include "sup/epics/dtotypes_fwd.h"

#include <memory>

namespace sup::epics
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

  void UnboundedArrayProlog(const sup::dto::AnyValue* anytype) override;
  void UnboundedArrayEpilog(const sup::dto::AnyValue* anytype) override;

private:
  struct PvxsValueBuilderImpl;
  std::unique_ptr<PvxsValueBuilderImpl> p_impl;
};

}  // namespace sup::epics

#endif  // SUP_EPICS_PVXSVALUEBUILDER_H
