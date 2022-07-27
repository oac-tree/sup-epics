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

#ifndef SUP_EPICS_UTILS_ANYVALUE_FROM_PVXS_BUILDER_H_
#define SUP_EPICS_UTILS_ANYVALUE_FROM_PVXS_BUILDER_H_

#include <sup/epics/dto_types_fwd.h>

#include <memory>

namespace sup
{
namespace epics
{

//! Builds AnyValue from PVXS's value.

class AnyValueFromPVXSBuilder
{
public:
  explicit AnyValueFromPVXSBuilder(const pvxs::Value& pvxs_value);
  ~AnyValueFromPVXSBuilder();

  dto::AnyValue MoveAnyType() const;

private:
  struct AnyValueFromPVXSBuilderImpl;
  std::unique_ptr<AnyValueFromPVXSBuilderImpl> p_impl;
};

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_UTILS_ANYVALUE_FROM_PVXS_BUILDER_H_
