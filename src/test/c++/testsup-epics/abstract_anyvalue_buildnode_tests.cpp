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

#include "sup/epics/utils/abstract_anyvalue_buildnode.h"

#include <gtest/gtest.h>
#include <sup/dto/anytype.h>
#include <sup/dto/anytype_helper.h>
#include <sup/dto/anyvalue.h>
#include <sup/dto/anyvalue_helper.h>

#include <stdexcept>

using namespace sup::epics;

class AbstractAnyValueBuildNodeTests : public ::testing::Test
{
public:
  class TestNode : public AbstractAnyValueBuildNode
  {
  public:
    TestNode() = default;
    TestNode(sup::dto::AnyValue&& value)
        : AbstractAnyValueBuildNode(std::move(value))
    {
    }

    NodeType GetNodeType() const override { return NodeType::kValue; }
    bool Process(std::stack<node_t>&) override { return false; }
  };
};

//! Checking initial state of TestNode class.

TEST_F(AbstractAnyValueBuildNodeTests, InitialState)
{
  TestNode node;
  EXPECT_TRUE(node.GetFieldName().empty());

  std::stack<AbstractAnyValueBuildNode::node_t> stack;
  EXPECT_FALSE(node.Process(stack));

  EXPECT_THROW(node.AddMember("name", sup::dto::AnyValue()), std::runtime_error);
  EXPECT_THROW(node.AddElement(sup::dto::AnyValue()), std::runtime_error);
}

TEST_F(AbstractAnyValueBuildNodeTests, MoveAnyValue)
{
  TestNode node(sup::dto::AnyValue{sup::dto::SignedInteger32Type, 42});

  auto result = node.MoveAnyValue();

  sup::dto::AnyValue expected{sup::dto::SignedInteger32Type, 42};
  EXPECT_EQ(result, expected);
}

TEST_F(AbstractAnyValueBuildNodeTests, SetFieldName)
{
  TestNode node;
  node.SetFieldName("field_name");
  EXPECT_EQ(node.GetFieldName(), std::string("field_name"));
}
