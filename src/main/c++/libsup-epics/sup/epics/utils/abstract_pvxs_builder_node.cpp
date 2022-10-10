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

#include "abstract_pvxs_builder_node.h"

namespace sup
{
namespace epics
{

AbstractPvxsBuilderNode::AbstractPvxsBuilderNode(pvxs::Value pvxs_value) : m_pvxs_value(pvxs_value)
{
}

pvxs::Value AbstractPvxsBuilderNode::GetPvxsValue() const
{
  return m_pvxs_value;
}

pvxs::Value &AbstractPvxsBuilderNode::GetPvxsValueRef()
{
  return m_pvxs_value;
}

void AbstractPvxsBuilderNode::ArrayElementSeparator()
{
  throw std::runtime_error("Not implemented");
}

bool AbstractPvxsBuilderNode::IsStructArrayNode() const
{
  return false;
}

bool AbstractPvxsBuilderNode::IsScalarArrayNode() const
{
  return false;
}

void AbstractPvxsBuilderNode::Freeze()
{
  // do nothing
}

}  // namespace epics
}  // namespace sup
