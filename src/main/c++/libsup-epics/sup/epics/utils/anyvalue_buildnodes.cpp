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

#include "sup/epics/utils/anyvalue_buildnodes.h"

#include <sup/epics/utils/anyvalue_buildnode_utils.h>

#include <iostream>
#include <stdexcept>

namespace
{
const bool kKeepInStackRequest{true};
const bool kDoNotKeepInStackRequest{false};
}  // namespace

namespace sup
{
namespace epics
{

// ----------------------------------------------------------------------------
// AnyValueBuildNode
// ----------------------------------------------------------------------------

AnyValueBuildNode::AnyValueBuildNode(const sup::dto::AnyValue &value)
    : AbstractAnyValueBuildNode(value)
{
}

AbstractAnyValueBuildNode::NodeType AnyValueBuildNode::GetNodeType() const
{
  return NodeType::kValue;
}

bool AnyValueBuildNode::Process(std::stack<node_t> &stack)
{
  ValidateAddValueNode(stack);
  return kKeepInStackRequest;
}

// ----------------------------------------------------------------------------
// StartStructBuildNode
// ----------------------------------------------------------------------------

StartStructBuildNode::StartStructBuildNode(const std::string &struct_name)
    : AbstractAnyValueBuildNode(::sup::dto::EmptyStruct(struct_name))

{
}

AbstractAnyValueBuildNode::NodeType StartStructBuildNode::GetNodeType() const
{
  return NodeType::kStartStruct;
}

bool StartStructBuildNode::Process(std::stack<node_t> &stack)
{
  ValidateAddValueNode(stack);
  return kKeepInStackRequest;
}

void StartStructBuildNode::AddMember(const std::string &name, const sup::dto::AnyValue &value)
{
  m_value.AddMember(name, value);
}

// ----------------------------------------------------------------------------
// EndStructBuildNode
// ----------------------------------------------------------------------------

AbstractAnyValueBuildNode::NodeType EndStructBuildNode::GetNodeType() const
{
  return NodeType::kEndStruct;
}

bool EndStructBuildNode::Process(std::stack<node_t> &stack)
{
  ValidateLastNode(stack, NodeType::kStartStruct);

  // saving the value and removing StartStructBuildNode
  Consume(stack.top()->MoveAnyValue());
  stack.pop();

  return kKeepInStackRequest;
}

// ----------------------------------------------------------------------------
// StartFieldBuildNode
// ----------------------------------------------------------------------------

StartFieldBuildNode::StartFieldBuildNode(const std::string &field_name)
    : AbstractAnyValueBuildNode()
{
  SetFieldName(field_name);
}

AbstractAnyValueBuildNode::NodeType StartFieldBuildNode::GetNodeType() const
{
  return NodeType::kStartField;
}

bool StartFieldBuildNode::Process(std::stack<node_t> &stack)
{
  ValidateLastNode(stack, NodeType::kStartStruct);

  if (GetFieldName().empty())
  {
    throw std::runtime_error("Error in StartFieldBuildNode::Process(): fieldname is not defined");
  }

  return kKeepInStackRequest;
}

// ----------------------------------------------------------------------------
// EndFieldBuildNode
// ----------------------------------------------------------------------------

AbstractAnyValueBuildNode::NodeType EndFieldBuildNode::GetNodeType() const
{
  return NodeType::kEndField;
}

//! Processes the stack, finalizes the adding of the field to StartStructBuildNode.
//! @note It will remove two last nodes (with the value, and with the field name) and then
//! create a field in the remaining StartStructBuildNode.
bool EndFieldBuildNode::Process(std::stack<node_t> &stack)
{
  ValidateIfValueNodeIsComplete(stack);

  // removing value node (scalar, struct or array), keeping the value for later reuse
  auto value = stack.top()->MoveAnyValue();
  stack.pop();

  ValidateLastNode(stack, NodeType::kStartField);

  // removing StartFieldNode, keeping the name for later reuse
  auto field_name = stack.top()->GetFieldName();
  stack.pop();

  ValidateLastNode(stack, NodeType::kStartStruct);

  // adding a new member to StartStructBuildNode
  stack.top()->AddMember(field_name, value);

  return kDoNotKeepInStackRequest;
}

// ----------------------------------------------------------------------------
// StartArrayBuildNode
// ----------------------------------------------------------------------------

StartArrayBuildNode::StartArrayBuildNode(const std::string &array_name) : m_array_name(array_name)
{
}

AbstractAnyValueBuildNode::NodeType StartArrayBuildNode::GetNodeType() const
{
  return NodeType::kStartArray;
}

bool StartArrayBuildNode::Process(std::stack<node_t> &stack)
{
  ValidateAddValueNode(stack);
  return kKeepInStackRequest;
}

//! Adds element to the array. If array doesn't exist, it will be initialised using the type of the
//! given value.
void StartArrayBuildNode::AddElement(const sup::dto::AnyValue &value)
{
  if (sup::dto::IsEmptyValue(m_value))
  {
    m_value = sup::dto::AnyValue(0, value.GetType(), m_array_name);
    m_value.AddElement(value);
  }
  else
  {
    m_value.AddElement(value);
  }
}

// ----------------------------------------------------------------------------
// EndArrayBuildNode
// ----------------------------------------------------------------------------

AbstractAnyValueBuildNode::NodeType EndArrayBuildNode::GetNodeType() const
{
  return NodeType::kEndArray;
}

bool EndArrayBuildNode::Process(std::stack<node_t> &stack)
{
  ValidateLastNode(stack, NodeType::kStartArray);

  // replacing StartArrayBuildNode with EndArrayBuildNode
  Consume(stack.top()->MoveAnyValue());
  stack.pop();

  return kKeepInStackRequest;
}

// ----------------------------------------------------------------------------
// StartArrayElementBuildNode
// ----------------------------------------------------------------------------

AbstractAnyValueBuildNode::NodeType StartArrayElementBuildNode::GetNodeType() const
{
  return NodeType::kStartArrayElement;
}

bool StartArrayElementBuildNode::Process(std::stack<node_t> &stack)
{
  ValidateLastNode(stack, NodeType::kStartArray);
  return kKeepInStackRequest;
}

// ----------------------------------------------------------------------------
// EndArrayElementBuildNode
// ----------------------------------------------------------------------------

AbstractAnyValueBuildNode::NodeType EndArrayElementBuildNode::GetNodeType() const
{
  return NodeType::kEndArrayElement;
}

//! Processes the stack, finalizes the adding of the element to StartArrayBuildNode.
//! @note It will remove two last nodes (with the value, and StartArrayElementNode) and then
//! create a field in the remaining StartArrayBuildNode.

bool EndArrayElementBuildNode::Process(std::stack<node_t> &stack)
{
  ValidateIfValueNodeIsComplete(stack);

  // removing value node (scalar, struct or array), keeping the value for later reuse
  auto value = stack.top()->MoveAnyValue();
  stack.pop();

  ValidateLastNode(stack, NodeType::kStartArrayElement);
  stack.pop();

  // adding a new element to StartArrayBuildNode
  stack.top()->AddElement(value);

  return kDoNotKeepInStackRequest;
}

}  // namespace epics

}  // namespace sup
