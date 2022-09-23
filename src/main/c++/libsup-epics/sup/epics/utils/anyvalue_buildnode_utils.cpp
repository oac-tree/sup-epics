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

#include "sup/epics/utils/anyvalue_buildnode_utils.h"

#include <algorithm>
#include <sstream>
#include <vector>

namespace sup
{
namespace epics
{

bool CanAddValueNode(const std::stack<AbstractAnyValueBuildNode::node_t> &stack)
{
  if (stack.empty())
  {
    return true;
  }

  using NodeType = AbstractAnyValueBuildNode::NodeType;
  static const std::vector<NodeType> expected_types{NodeType::kStartArrayElement,
                                                    NodeType::kStartField};

  auto it = std::find(expected_types.begin(), expected_types.end(), stack.top()->GetNodeType());
  return it != expected_types.end();
}

void ValidateAddValueNode(const std::stack<AbstractAnyValueBuildNode::node_t> &stack)
{
  if (!CanAddValueNode(stack))
  {
    std::ostringstream ostr;
    ostr << "Error in ValidateAddValueNode(): AnyValueNode can not be added, ";
    if (stack.empty())
    {
      ostr << "stack is empty.";
    }
    else
    {
      ostr << "last NodeType=" << static_cast<int>(stack.top()->GetNodeType()) << ".\n";
    }

    throw std::runtime_error(ostr.str());
  }
}

void ValidateLastNode(const std::stack<AbstractAnyValueBuildNode::node_t> &stack,
                      AbstractAnyValueBuildNode::NodeType node_type)
{
  if (stack.empty() || stack.top()->GetNodeType() != node_type)
  {
    throw std::runtime_error("Error in ValidateLastNode(): wrong type of the last node");
  }
}

void ValidateIfValueNodeIsComplete(const std::stack<AbstractAnyValueBuildNode::node_t> &stack)
{
  if (stack.empty())
  {
    throw std::runtime_error("Error in ValidateIfValueNodeIsComplete(): stack is empty");
  }

  using NodeType = AbstractAnyValueBuildNode::NodeType;
  static const std::vector<NodeType> expected_types{NodeType::kValue, NodeType::kEndStruct,
                                                    NodeType::kEndArray};

  auto it = std::find(expected_types.begin(), expected_types.end(), stack.top()->GetNodeType());

  if (it == expected_types.end())
  {
    throw std::runtime_error("Error in ValidateIfValueNodeIsComplete(): wrong node type");
  }
}

}  // namespace epics

}  // namespace sup
