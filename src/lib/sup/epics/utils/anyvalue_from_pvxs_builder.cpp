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
#include <sup/dto/anyvalue_composer.h>
#include <sup/epics/utils/dto_scalar_conversion_utils.h>
#include <sup/epics/utils/pvxs_utils.h>

#include <cassert>
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
  sup::dto::AnyValueComposer m_composer;
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
      else if (IsStructArray(node.m_value))
      {
        ProcessArrayNode(node);
      }
      else
      {
        std::ostringstream ostr;
        ostr << "AnyValueFromPVXSBuilder: unsupported PVXS value \n" << node.m_value;
        throw std::runtime_error(ostr.str());
      }
    }
  }

  void ProcessStructNode(Node& node)
  {
    if (node.m_is_visited)
    {
      ProcessVisitedStructNode(node);
    }
    else
    {
      ProcessNewStructNode(node);
    }
  }

  void ProcessArrayNode(Node& node)
  {
    if (node.m_is_visited)
    {
      ProcessVisitedArrayNode(node);
    }
    else
    {
      ProcessNewArrayNode(node);
    }
  }

  void ProcessNewStructNode(Node& node)
  {
    StartComposite(node);
    m_composer.StartStruct(node.m_value.id());
    AddChildren(node, NodeContext::kStructField);
  }

  void ProcessVisitedStructNode(Node& node)
  {
    m_composer.EndStruct();
    EndComposite(node);
  }

  void ProcessNewArrayNode(Node& node)
  {
    StartComposite(node);
    m_composer.StartArray(node.m_value.id());
    AddChildren(node, NodeContext::kArrayElement);
  }

  void ProcessVisitedArrayNode(Node& node)
  {
    m_composer.EndArray();
    EndComposite(node);
  }

  void StartComposite(Node& node)
  {
    assert(node.m_is_visited == false);
    node.m_is_visited = true;

    if (node.IsStructContext())
    {
      m_composer.StartField(node.m_name);
    }
    else if (node.IsArrayContext())
    {
      m_composer.StartArrayElement();
    }

    // this is top level object
  }

  void EndComposite(Node& node)
  {
    assert(node.m_is_visited);
    if (node.IsStructContext())
    {
      m_composer.EndField();
    }
    else if (node.IsArrayContext())
    {
      m_composer.EndArrayElement();
    }
    m_stack.pop();  // we don't need the node anymore
  }

  void AddChildren(Node& node, NodeContext context)
  {
    auto children = GetChildren(node.m_value);
    // iteration in reverse order
    for (auto it = children.rbegin(); it != children.rend(); ++it)
    {
      Node child_node{*it, context};
      child_node.m_name = GetFieldNameOfChild(node.m_value, *it);
      m_stack.push(child_node);
    }
  }

  //! Process PVXS value representing a scalar.
  void ProcessScalarNode(Node& node)
  {
    StartComposite(node);
    m_composer.AddValue(GetAnyValueFromScalar(node.m_value));
    EndComposite(node);
  }

  //! Process PVXS value representing a scalar array.
  void ProcessScalarArrayNode(Node& node)
  {
    StartComposite(node);
    m_composer.AddValue(GetAnyValueFromScalarArray(node.m_value));
    EndComposite(node);
  }

  AnyValueFromPVXSBuilderImpl() = default;
};

AnyValueFromPVXSBuilder::AnyValueFromPVXSBuilder(const pvxs::Value& pvxs_value)
    : p_impl(new AnyValueFromPVXSBuilderImpl)
{
  p_impl->ProcessPvxsValue(pvxs_value);
}

dto::AnyValue AnyValueFromPVXSBuilder::MoveAnyValue() const
{
  return p_impl->m_composer.MoveAnyValue();
}

AnyValueFromPVXSBuilder::~AnyValueFromPVXSBuilder() = default;

}  // namespace epics
}  // namespace sup
