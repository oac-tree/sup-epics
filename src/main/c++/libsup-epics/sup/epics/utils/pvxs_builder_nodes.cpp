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

#include <sup/dto/anyvalue.h>
#include <sup/epics/utils/dto_scalar_conversion_utils.h>
#include <sup/epics/utils/pvxs_builder_nodes.h>
#include <sup/epics/utils/pvxs_utils.h>

#include <stdexcept>

namespace sup
{
namespace epics
{

PvxsBuilderNode::PvxsBuilderNode(pvxs::Value pvxs_value) : AbstractPvxsBuilderNode(pvxs_value) {}

ScalarArrayBuilderNode::ScalarArrayBuilderNode(pvxs::Value pvxs_value,
                                               const dto::AnyValue *any_value)
    : AbstractPvxsBuilderNode(pvxs_value)
{
  if (!IsScalarArray(pvxs_value))
  {
    throw std::runtime_error("Not a scalar array");
  }

  AssignAnyValueToPVXSValueScalarArray(*any_value, pvxs_value);
}

bool ScalarArrayBuilderNode::IsScalarArrayNode() const
{
  return true;
}

StructArrayBuilderNode::StructArrayBuilderNode(pvxs::Value pvxs_value,
                                               const sup::dto::AnyValue *any_value)
    : AbstractPvxsBuilderNode(pvxs_value), m_array(any_value->NumberOfElements())
{
  ::pvxs::Value tmp(pvxs_value);

  for (size_t i = 0; i < any_value->NumberOfElements(); ++i)
  {
    m_array[i] = tmp.allocMember();
  }
}

// void StructArrayBuilderNode::AddElement(pvxs::Value pvxs_value)
//{
//   if (m_current_index >= m_array.size())
//   {
//     throw std::runtime_error("Attempt to add too much elements in the array");
//   }

//  m_array[m_current_index].assign(pvxs_value);
//  ++m_current_index;

//  if (m_current_index == m_array.size())
//  {
//    m_pvxs_value = m_array.freeze().castTo<const void>();
//  }
//}

pvxs::Value &StructArrayBuilderNode::GetCurrent()
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

  if (m_current_index == m_array.size())
  {
    m_pvxs_value = m_array.freeze().castTo<const void>();
  }
}

bool StructArrayBuilderNode::IsStructArrayNode() const
{
  return true;
}

}  // namespace epics

}  // namespace sup
