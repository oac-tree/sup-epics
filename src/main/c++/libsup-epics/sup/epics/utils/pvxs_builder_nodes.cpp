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

#include <sup/epics/utils/pvxs_builder_nodes.h>

namespace sup
{
namespace epics
{

PvxsBuilderNode::PvxsBuilderNode(pvxs::Value pvxs_value) : AbstractPvxsBuilderNode(pvxs_value) {}

StructArrayBuilderNode::StructArrayBuilderNode(pvxs::Value pvxs_value)
    : AbstractPvxsBuilderNode(pvxs_value)
{
}

}  // namespace epics

}  // namespace sup
