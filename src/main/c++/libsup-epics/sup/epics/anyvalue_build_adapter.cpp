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

#include "anyvalue_build_adapter.h"

#include <sup/dto/anytype.h>
#include <sup/dto/anytype_registry.h>
#include <sup/dto/anyvalue.h>
#include <sup/dto/parse/anyvalue_builder.h>

namespace sup
{
namespace epics
{

struct AnyValueBuildAdapter::AnyValueBuildAdapterImpl
{
  ::sup::dto::AnyTypeRegistry m_registry;
  ::sup::dto::AnyValueBuilder m_builder;

  void WriteEncoding()
  {
    const std::string key("encoding");
    const std::string value("sup-dto/v1.0/JSON");
    m_builder.StartObject();
    m_builder.Key(key.c_str(), key.size(), true);
    m_builder.String(value.c_str(), value.size(), true);
    m_builder.EndObject(1);
  }

  AnyValueBuildAdapterImpl() : m_builder(&m_registry) {}
};

AnyValueBuildAdapter::AnyValueBuildAdapter() : p_impl(new AnyValueBuildAdapterImpl) {}

dto::AnyValue AnyValueBuildAdapter::MoveAnyValue() const
{
  return p_impl->m_builder.MoveAnyValue();
}

bool AnyValueBuildAdapter::Bool(dto::boolean b, const std::string &name)
{
  return false;
}

bool AnyValueBuildAdapter::Int32(dto::int32 i, const std::string &name)
{
  p_impl->m_builder.Key(name.c_str(), name.size(), true);
  p_impl->m_builder.Int(i);
  return true;
}

bool AnyValueBuildAdapter::Uint32(dto::uint32 u, const std::string &name)
{
  return false;
}

bool AnyValueBuildAdapter::Int64(dto::int64 i, const std::string &name)
{
  return false;
}

bool AnyValueBuildAdapter::Uint64(dto::uint64 u, const std::string &name)
{
  return false;
}

bool AnyValueBuildAdapter::Double(dto::float64 d, const std::string &name)
{
  return false;
}

bool AnyValueBuildAdapter::String(const std::string &str, const std::string &name)
{
  return false;
}

void AnyValueBuildAdapter::StartStruct()
{
  p_impl->m_builder.StartArray();
  p_impl->WriteEncoding();
  p_impl->m_builder.StartObject();
}

void AnyValueBuildAdapter::EndStruct()
{
  p_impl->m_builder.EndObject(1);
  p_impl->m_builder.EndArray(1);
}

AnyValueBuildAdapter::~AnyValueBuildAdapter() = default;

}  // namespace epics
}  // namespace sup
