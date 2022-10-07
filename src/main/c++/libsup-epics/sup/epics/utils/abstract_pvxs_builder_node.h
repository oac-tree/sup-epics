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

#ifndef SUP_EPICS_UTILS_ABSTRACT_PVXS_BUILDER_NODE_H_
#define SUP_EPICS_UTILS_ABSTRACT_PVXS_BUILDER_NODE_H_

#include <pvxs/data.h>

namespace sup
{
namespace epics
{

class AbstractPvxsBuilderNode
{
public:
  AbstractPvxsBuilderNode(pvxs::Value pvxs_value);
  ~AbstractPvxsBuilderNode() = default;

  virtual pvxs::Value GetPvxsValue() const;

  virtual pvxs::Value& GetCurrent();

  virtual void ArrayElementSeparator();

  virtual bool IsStructArrayNode() const;

  virtual bool IsScalarArrayNode() const;

protected:
  pvxs::Value m_pvxs_value;
};

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_UTILS_ABSTRACT_PVXS_BUILDER_NODE_H_
