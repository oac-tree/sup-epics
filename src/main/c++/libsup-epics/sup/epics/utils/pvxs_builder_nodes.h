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

#ifndef SUP_EPICS_UTILS_PVXS_BUILDER_NODES_H_
#define SUP_EPICS_UTILS_PVXS_BUILDER_NODES_H_

#include <sup/epics/dto_types_fwd.h>
#include <sup/epics/utils/abstract_pvxs_builder_node.h>

namespace sup
{
namespace epics
{

class PvxsBuilderNode : public AbstractPvxsBuilderNode
{
public:
  PvxsBuilderNode(pvxs::Value pvxs_value);
};

class ScalarArrayBuilderNode : public AbstractPvxsBuilderNode
{
public:
  ScalarArrayBuilderNode(pvxs::Value pvxs_value, const sup::dto::AnyValue *any_value);

  void ArrayElementSeparator() override;

  bool IsScalarArrayNode() const override;
};

class StructArrayBuilderNode : public AbstractPvxsBuilderNode
{
public:
  StructArrayBuilderNode(pvxs::Value pvxs_value, const sup::dto::AnyValue *any_value);

  //  void AddElement(pvxs::Value pvxs_value);

  virtual pvxs::Value &GetCurrent() override;

  void ArrayElementSeparator() override;

  bool IsStructArrayNode() const override;

private:
  pvxs::shared_array<pvxs::Value> m_array;
  size_t m_current_index{0};
};

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_UTILS_PVXS_BUILDER_NODES_H_
