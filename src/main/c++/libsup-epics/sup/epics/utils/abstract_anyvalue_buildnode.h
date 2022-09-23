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

#ifndef SUP_EPICS_UTILS_ABSTRACT_ANYVALUE_BUILDNODE_H_
#define SUP_EPICS_UTILS_ABSTRACT_ANYVALUE_BUILDNODE_H_

#include <sup/dto/anyvalue.h>

#include <memory>
#include <stack>
#include <string>

namespace sup
{
namespace epics
{

//! The node for AnyValueBuildAdapter to build AnyValue.

class AbstractAnyValueBuildNode
{
public:
  using node_t = std::unique_ptr<AbstractAnyValueBuildNode>;

  enum class NodeType
  {
    kValue,
    kStartStruct,
    kEndStruct,
    kStartField,
    kEndField,
    kStartArray,
    kEndArray,
    kStartArrayElement,
    kEndArrayElement
  };

  AbstractAnyValueBuildNode() = default;
  AbstractAnyValueBuildNode(const sup::dto::AnyValue& value);

  virtual NodeType GetNodeType() const = 0;

  //! Performs manipulations with the stack, if necessary.
  //! @return True if nodes wants it to be saved in a stack.
  virtual bool Process(std::stack<node_t>& stack) = 0;

  virtual void Consume(sup::dto::AnyValue&& value);

  sup::dto::AnyValue MoveAnyValue() const;

  std::string GetFieldName() const;
  void SetFieldName(const std::string& name);

  virtual void AddMember(const std::string& name, const sup::dto::AnyValue& value);

  virtual void AddElement(const sup::dto::AnyValue& value);

protected:
  sup::dto::AnyValue m_value;
  std::string m_field_name;
};

}  // namespace epics
}  // namespace sup

#endif  // SUP_EPICS_UTILS_ABSTRACT_ANYVALUE_BUILDNODE_H_
