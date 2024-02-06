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

#include <sup/dto/anyvalue.h>
#include <sup/epics/utils/dto_scalar_conversion_utils.h>
#include <sup/epics/utils/pvxs_builder_nodes.h>
#include <sup/epics/utils/pvxs_utils.h>

#include <stdexcept>

namespace sup
{
namespace epics
{

// ----------------------------------------------------------------------------
// PvxsBuilderNode
// ----------------------------------------------------------------------------

PvxsBuilderNode::PvxsBuilderNode(pvxs::Value pvxs_value) : m_pvxs_value(pvxs_value) {}

pvxs::Value PvxsBuilderNode::GetPvxsValue() const
{
  return m_pvxs_value;
}

pvxs::Value &PvxsBuilderNode::GetPvxsValueRef()
{
  return m_pvxs_value;
}

void PvxsBuilderNode::ArrayElementSeparator()
{
  throw std::runtime_error("Not implemented");
}

bool PvxsBuilderNode::IsStructArrayNode() const
{
  return false;
}

bool PvxsBuilderNode::IsScalarArrayNode() const
{
  return false;
}

void PvxsBuilderNode::Freeze()
{
  // do nothing
}

// ----------------------------------------------------------------------------
// ScalarArrayBuilderNode
// ----------------------------------------------------------------------------

ScalarArrayBuilderNode::ScalarArrayBuilderNode(pvxs::Value pvxs_value,
                                               const dto::AnyValue *any_value)
    : PvxsBuilderNode(pvxs_value)
{
  if (!IsScalarArray(pvxs_value))
  {
    throw std::runtime_error("Not a scalar array");
  }

  AssignAnyValueToPVXSValueScalarArray(*any_value, pvxs_value);
}

void ScalarArrayBuilderNode::ArrayElementSeparator()
{
  // do nothing, array was already filled in constructor
}

bool ScalarArrayBuilderNode::IsScalarArrayNode() const
{
  return true;
}

// ----------------------------------------------------------------------------
// StructArrayBuilderNode
// ----------------------------------------------------------------------------

StructArrayBuilderNode::StructArrayBuilderNode(pvxs::Value pvxs_value,
                                               const sup::dto::AnyValue *any_value)
    : PvxsBuilderNode(pvxs_value), m_array(any_value->NumberOfElements())
{
  for (size_t i = 0; i < any_value->NumberOfElements(); ++i)
  {
    m_array[i] = pvxs_value.allocMember();
  }
}

pvxs::Value &StructArrayBuilderNode::GetPvxsValueRef()
{
  if (m_current_index >= m_array.size())
  {
    throw std::runtime_error("Out of bounds");
  }
  return m_array[m_current_index];
}

void StructArrayBuilderNode::ArrayElementSeparator()
{
  m_current_index++;
}

bool StructArrayBuilderNode::IsStructArrayNode() const
{
  return true;
}

void StructArrayBuilderNode::Freeze()
{
  m_pvxs_value = m_array.freeze().castTo<const void>();
}

}  // namespace epics

}  // namespace sup
