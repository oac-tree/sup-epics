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
//! Internally relies on ::sup::dto::AnyValueBuilder.

class AnyValueBuildAdapter
{
public:
  AnyValueBuildAdapter();
  ~AnyValueBuildAdapter();

  dto::AnyValue MoveAnyValue() const;

  bool Bool(::sup::dto::boolean b, const std::string& name);
  bool Int32(::sup::dto::int32 i, const std::string& name);
  bool Uint32(::sup::dto::uint32 u, const std::string& name);
  bool Int64(::sup::dto::int64 i, const std::string& name);
  bool Uint64(::sup::dto::uint64 u, const std::string& name);
  bool Double(::sup::dto::float64 d, const std::string& name);
  bool String(const std::string& str, const std::string& name);

  void StartStruct();

  void EndStruct();

private:
  struct AnyValueBuildAdapterImpl;
  std::unique_ptr<AnyValueBuildAdapterImpl> p_impl;
};

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_ANYVALUE_BUILD_ADAPTER_H_
