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

#ifndef SUP_EPICS_ANYVALUE_BUILD_ADAPTER_H_
#define SUP_EPICS_ANYVALUE_BUILD_ADAPTER_H_

#include <sup/epics/dto_types_fwd.h>
#include <sup/dto/basic_scalar_types.h>

#include <memory>

namespace sup
{
namespace epics
{

//! Builds AnyValue in step-wise manner by calling methods to add fields and structs.

class AnyValueBuildAdapter
{
public:
  AnyValueBuildAdapter();
  ~AnyValueBuildAdapter();

  dto::AnyValue MoveAnyValue() const;

  void Int32(const std::string& name, ::sup::dto::int32 value);

  void StartStruct();

  void EndStruct();

private:
  struct AnyValueBuildAdapterImpl;
  std::unique_ptr<AnyValueBuildAdapterImpl> p_impl;
};

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_ANYVALUE_BUILD_ADAPTER_H_
