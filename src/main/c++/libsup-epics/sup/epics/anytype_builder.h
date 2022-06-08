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

#ifndef SUP_EPICS_ANYTYPE_BUILDER_H_
#define SUP_EPICS_ANYTYPE_BUILDER_H_

#include <sup/epics/dto_types_fwd.h>

#include <memory>

namespace sup
{
namespace epics
{

//! Builds AnyType from PVXS's TypeDef.
//! Internally relies on syp::dto::AnyTypeBuilder

class AnyTypeBuilder
{
public:
  explicit AnyTypeBuilder(const pvxs::TypeDef& pvxs_type);
  ~AnyTypeBuilder();

  dto::AnyType MoveAnyType() const;

private:
  struct AnyTypeBuilderImpl;
  std::unique_ptr<AnyTypeBuilderImpl> p_impl;
};

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_ANYTYPE_BUILDER_H_
