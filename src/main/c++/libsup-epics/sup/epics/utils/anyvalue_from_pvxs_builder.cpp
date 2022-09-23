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

#include "anyvalue_from_pvxs_builder.h"

#include <pvxs/data.h>
#include <sup/dto/anytype.h>
#include <sup/dto/anyvalue.h>
#include <sup/epics/utils/anyvalue_build_adapter.h>
#include <sup/epics/utils/dto_scalar_conversion_utils.h>
#include <sup/epics/utils/pvxs_utils.h>

#include <list>
#include <sstream>
#include <stack>
#include <stdexcept>

namespace sup
{
namespace epics
{

enum NodeContext
{
  kRoot,
  kStructField,
  kArrayElement
};

struct Node
{
  //! Current PVXS value in the hierarchy. Please not that PVXS has implicit sharing so the
  //! object basically behaves as a pointer.
  ::pvxs::Value m_value;

  //!< The name under which the value is known to its parent.
  std::string m_name;

  //! Will be true if `value` is a struct and all children are processed.
  bool m_is_visited{false};

  NodeContext m_context;

  Node(::pvxs::Value value, NodeContext context = kRoot) : m_value(value), m_context(context) {}

  bool IsArrayContext() const { return m_context == kArrayElement; }

  bool IsStructContext() const { return m_context == kStructField; }
};

struct AnyValueFromPVXSBuilder::AnyValueFromPVXSBuilderImpl
{
  AnyValueBuildAdapter m_builder;
  std::stack<Node> m_stack;

  void ProcessPvxsValue(const pvxs::Value& pvxs_value)
  {
    if (IsEmptyValue(pvxs_value))
    {
      return;  // by default AnyValueBuildAdapter will generate empty AnyValue
    }

    m_stack.push({pvxs_value});
    ProcessStack();
  }

  void ProcessStack()
  {
    while (!m_stack.empty())
    {
      auto& node = m_stack.top();

      if (IsStruct(node.m_value))
      {
        ProcessStructNode(node);
      }
      else if (IsScalar(node.m_value))
      {
        ProcessScalarNode(node);
      }
      else if (IsScalarArray(node.m_value))
      {
        ProcessScalarArrayNode(node);
      }
      else
      {
        std::ostringstream ostr;
        ostr << "AnyValueFromPVXSBuilder: unsupported PVXS value \n" << node.m_value;
        throw std::runtime_error(ostr.str());
      }
    }
  }
  //! Process PVXS value representing a struct.
  void ProcessStructNode(Node& node)
  {
    if (node.m_is_visited)
    {
      // All children have been already added to the struct. It's time to tell the builder
      // that the struct has to be added to its own parent.
      m_builder.EndStruct(node.m_name);
      m_stack.pop();  // we don't need the node anymore
    }
    else
    {
      // We found a struct which we haven't seen before. Let's tell the builder to create
      // underlying AnyValue, and let's add children to the stack.
      // We are not poping struct node, we will get back to it later.
      m_builder.StartStruct(node.m_value.id());
      node.m_is_visited = true;

      auto children = GetChildren(node.m_value);
      // iteration in reverse order
      for (auto it = children.rbegin(); it != children.rend(); ++it)
      {
        Node child_node{*it, kStructField};
        child_node.m_name = node.m_value.nameOf(*it);
        m_stack.push(child_node);
      }
    }
  }

  //! Process PVXS value representing a scalar.
  void ProcessScalarNode(Node& node)
  {
    // It's a scalar field. Let's add corresponding field to the AnyValue and remove node from
    // stack. We don't need it anymore.
    m_builder.AddMember(node.m_name, GetAnyValueFromScalar(node.m_value));
    m_stack.pop();
  }

  //! Process PVXS value representing a scalar array.
  void ProcessScalarArrayNode(Node& node)
  {
    // It's a scalar array field. Let's add corresponding field to the AnyValue and remove node from
    // stack. We don't need it anymore.
    m_builder.AddMember(node.m_name, GetAnyValueFromScalarArray(node.m_value));
    m_stack.pop();
  }

  AnyValueFromPVXSBuilderImpl() = default;
};

AnyValueFromPVXSBuilder::AnyValueFromPVXSBuilder(const pvxs::Value& pvxs_value)
    : p_impl(new AnyValueFromPVXSBuilderImpl)
{
  p_impl->ProcessPvxsValue(pvxs_value);
}

dto::AnyValue AnyValueFromPVXSBuilder::MoveAnyType() const
{
  return std::move(p_impl->m_builder.MoveAnyValue());
}

AnyValueFromPVXSBuilder::~AnyValueFromPVXSBuilder() = default;

}  // namespace epics
}  // namespace sup
