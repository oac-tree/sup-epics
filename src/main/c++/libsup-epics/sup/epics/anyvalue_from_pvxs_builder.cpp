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
#include <sup/epics/anyvalue_build_adapter.h>
#include <sup/epics/dto_scalar_conversion_utils.h>
#include <sup/epics/dto_typecode_conversion_utils.h>
#include <sup/epics/pvxs_utils.h>

#include <iostream>
#include <list>
#include <stack>
#include <stdexcept>

namespace sup
{
namespace epics
{

struct Node
{
  ::pvxs::Value m_parent;
  ::pvxs::Value m_child;
  std::string m_name;  //! name under which the child is known to its parent
  bool m_is_visited{false};
  Node(::pvxs::Value parent, ::pvxs::Value child, const std::string& name, bool is_visited)
      : m_parent(parent), m_child(child), m_name(name), m_is_visited(is_visited)
  {
  }
};

struct AnyValueFromPVXSBuilder::AnyValueFromPVXSBuilderImpl
{
  ::pvxs::Value m_pvxs_value;
  AnyValueBuildAdapter m_builder;
  ::sup::dto::AnyValue m_result;
  std::stack<Node> m_pvxs_stack;

  void ProcessPvxsValue(const pvxs::Value& pvxs_value)
  {
    m_pvxs_value = pvxs_value;

    auto code = GetAnyTypeCode(m_pvxs_value.type());

    if (::sup::dto::IsScalarTypeCode(code))
    {
      m_result = ::sup::dto::AnyValue(::sup::dto::AnyType(code));
      AssignPVXSValueToAnyValueScalar(m_pvxs_value, m_result);
    }
    else if (::sup::dto::IsStructTypeCode(code))
    {
      std::cout << "aaaa " << m_pvxs_value << " " << m_pvxs_value.id();
      m_builder.StartStruct(m_pvxs_value.id());
      auto children = GetChildren(m_pvxs_value);
      // reverse iteration
      for (auto it = children.rbegin(); it != children.rend(); ++it)
      {
        m_pvxs_stack.push({m_pvxs_value, *it, m_pvxs_value.nameOf(*it), false});
      }
      ProcessStack();
      m_builder.EndStruct();

      m_result = m_builder.MoveAnyValue();
    }
  }

  void ProcessStack()
  {
    ::pvxs::Value current_parent(m_pvxs_value);
    int nloop{0};
    while (!m_pvxs_stack.empty())
    {
      auto& node = m_pvxs_stack.top();
      std::cout << "aaa 1.1 " << m_pvxs_stack.size() << " visited " << node.m_is_visited << std::endl;

      if (IsStruct(node.m_child))
      {
        if (node.m_is_visited)
        {
          std::cout << "aaa 1.2 EndStruct" << std::endl;
          m_builder.EndStruct(node.m_name);
          m_pvxs_stack.pop();
        }
        else
        {
          std::cout << "aaa 1.2 StartStruct " << node.m_child.id() << std::endl;
          m_builder.StartStruct(node.m_child.id());
          node.m_is_visited = true;
          auto children = GetChildren(node.m_child);
          for (auto it = children.rbegin(); it != children.rend(); ++it)
          {
            m_pvxs_stack.push({current_parent, *it, node.m_child.nameOf(*it), false});
          }
        }
      }

      if (IsScalar(node.m_child))
      {
        std::cout << "aaa 1.3 Iscalar" << m_pvxs_stack.size() << std::endl;
        m_builder.AddScalar(node.m_name, GetAnyValueFromScalar(node.m_child));
        m_pvxs_stack.pop();
      }



//      auto child = node.m_child;
//      auto member_name = current_parent.nameOf(child);

//      if (IsStruct(child))
//      {
//        m_builder.StartStruct();
//        current_parent = child;
//      }
//      else if (IsScalar(child))
//      {
//        m_builder.AddScalar(member_name, GetAnyValueFromScalar(child));
//      }
//      else
//      {
//        throw std::runtime_error("Unexpected PVXS type");
//      }

//      auto children = GetChildren(child);
//      // reverse iteration
//      for (auto it = children.rbegin(); it != children.rend(); ++it)
//      {
//        m_pvxs_stack.push({current_parent, *it, child.nameOf(*it), false});
//      }

      nloop++;
      if(nloop>10)
        throw 1;
    }
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
  return std::move(p_impl->m_result);
}

AnyValueFromPVXSBuilder::~AnyValueFromPVXSBuilder() = default;

}  // namespace epics
}  // namespace sup
