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

#ifndef SUP_EPICS_UTILS_ANYVALUE_BUILDNODES_UTILS_H_
#define SUP_EPICS_UTILS_ANYVALUE_BUILDNODES_UTILS_H_

//! @file anyvalue_buildnode_utils.h
//! Collection of utility functions to support AbstractAnyValueBuildNode and Co.

#include <sup/epics/utils/abstract_anyvalue_buildnode.h>

namespace sup
{
namespace epics
{

//! Returns true if it is possible to add value node. This will be the case in one of three cases:
//! 1) the stack is empty 2) last node denotes the start of the structure's field
//! 3) last node denotes denotes the start of the array's element.
bool CanAddValueNode(const std::stack<AbstractAnyValueBuildNode::node_t>& stack);

//! Validates if adding of value node is possible for this stack configuration, and throws if it is
//! not.
void ValidateAddValueNode(const std::stack<AbstractAnyValueBuildNode::node_t>& stack);

//! Validate if the last value in a stack has given type, will throw if not.
void ValidateLastNode(const std::stack<AbstractAnyValueBuildNode::node_t>& stack,
                      AbstractAnyValueBuildNode::NodeType node_type);

//! Returns true if the last node corresponds to a completed value node.
void ValidateIfValueNodeIsComplete(const std::stack<AbstractAnyValueBuildNode::node_t>& stack);

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_UTILS_ANYVALUE_BUILDNODES_UTILS_H_
