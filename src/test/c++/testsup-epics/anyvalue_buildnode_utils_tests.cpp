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

#include <gtest/gtest.h>
#include <sup/epics/utils/anyvalue_buildnodes.h>

#include <stdexcept>

using namespace sup::epics;

class AnyValueBuildNodeUtilsTests : public ::testing::Test
{
public:
  //! Utility function that creates a stack and put there one build node of given type.
  //! It returns the value reported by CanAddValueNode for this stack.
  template <typename T, typename... Args>
  bool CheckAddValueNode(Args&&... args)
  {
    std::stack<AbstractAnyValueBuildNode::node_t> stack;
    stack.push(std::unique_ptr<T>(new T((args)...)));
    return CanAddValueNode(stack);
  }

  template <typename T, typename... Args>
  void ValidateCompletedValueNode(Args&&... args)
  {
    std::stack<AbstractAnyValueBuildNode::node_t> stack;
    stack.push(std::unique_ptr<T>(new T((args)...)));
    ValidateIfValueNodeIsComplete(stack);
  }
};

//! Unit tests for CanAddValueNode utility function.

TEST_F(AnyValueBuildNodeUtilsTests, CanAddValueNode)
{
  {  // it is possible to add value node to empty stack
    std::stack<AbstractAnyValueBuildNode::node_t> stack;
    EXPECT_TRUE(CanAddValueNode(stack));
  }

  // it is possible to add value node if the last node is one of the following
  EXPECT_TRUE(CheckAddValueNode<StartFieldBuildNode>("name"));
  EXPECT_TRUE(CheckAddValueNode<StartArrayElementBuildNode>());

  // it is not possible to add value node if the last node in the stack is one of the following
  EXPECT_FALSE(CheckAddValueNode<StartStructBuildNode>("name"));
  EXPECT_FALSE(CheckAddValueNode<StartArrayBuildNode>("name"));
  EXPECT_FALSE(CheckAddValueNode<EndStructBuildNode>());
  EXPECT_FALSE(CheckAddValueNode<EndArrayBuildNode>());
  EXPECT_FALSE(CheckAddValueNode<EndFieldBuildNode>());
  EXPECT_FALSE(CheckAddValueNode<EndArrayElementBuildNode>());
}

//! Unit tests for ValidateAddValueNode utility function.

TEST_F(AnyValueBuildNodeUtilsTests, ValidateAddValueNode)
{
  {  // it is possible to add value node to empty stack
    std::stack<AbstractAnyValueBuildNode::node_t> stack;
    EXPECT_NO_THROW(ValidateAddValueNode(stack));
  }

  {  // it is not possible to add value node if stack contains StartStructBuildNode
    std::stack<AbstractAnyValueBuildNode::node_t> stack;
    stack.push(std::unique_ptr<StartStructBuildNode>(new StartStructBuildNode("name")));
    EXPECT_THROW(ValidateAddValueNode(stack), std::runtime_error);
  }

  // More tests above in AnyValueBuildNodeUtilsTests::CanAddValueNode.
}

//! Unit tests for ValidateAddValueNode utility function.

TEST_F(AnyValueBuildNodeUtilsTests, ValidateLastNode)
{
  std::stack<AbstractAnyValueBuildNode::node_t> stack;
  EXPECT_THROW(ValidateLastNode(stack, AbstractAnyValueBuildNode::NodeType::kStartArray),
               std::runtime_error);

  stack.push(std::unique_ptr<StartStructBuildNode>(new StartStructBuildNode("name")));
  EXPECT_NO_THROW(ValidateLastNode(stack, AbstractAnyValueBuildNode::NodeType::kStartStruct));
  EXPECT_THROW(ValidateLastNode(stack, AbstractAnyValueBuildNode::NodeType::kStartArray),
               std::runtime_error);
}

//! Unit tests for ValidateIfValueNodeIsComplete utility function.

TEST_F(AnyValueBuildNodeUtilsTests, ValidateIfValueNodeIsComplete)
{
  std::stack<AbstractAnyValueBuildNode::node_t> stack;
  EXPECT_THROW(ValidateIfValueNodeIsComplete(stack), std::runtime_error);

  EXPECT_NO_THROW(CheckAddValueNode<EndStructBuildNode>());
  EXPECT_NO_THROW(CheckAddValueNode<EndArrayBuildNode>());
}
