/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Gennady Pospelov (IO)
 *
 * Copyright (c) : 2010-2026 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 * SPDX-License-Identifier: MIT
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file LICENSE located in the top level directory
 * of the distribution package.
 *****************************************************************************/

#ifndef SUP_EPICS_UTILS_PVXS_BUILDER_NODES_H_
#define SUP_EPICS_UTILS_PVXS_BUILDER_NODES_H_

#include <pvxs/data.h>
#include <sup/epics/utils/dto_types_fwd.h>

namespace sup
{
namespace epics
{

class PvxsBuilderNode
{
public:
  explicit PvxsBuilderNode(pvxs::Value pvxs_value);

  ~PvxsBuilderNode() = default;

  virtual pvxs::Value GetPvxsValue() const;

  virtual pvxs::Value &GetPvxsValueRef();

  virtual void ArrayElementSeparator();

  virtual bool IsStructArrayNode() const;

  virtual bool IsScalarArrayNode() const;

  virtual void Freeze();

protected:
  pvxs::Value m_pvxs_value;
};

class ScalarArrayBuilderNode : public PvxsBuilderNode
{
public:
  ScalarArrayBuilderNode(pvxs::Value pvxs_value, const sup::dto::AnyValue *any_value);

  void ArrayElementSeparator() override;

  bool IsScalarArrayNode() const override;
};

class StructArrayBuilderNode : public PvxsBuilderNode
{
public:
  StructArrayBuilderNode(pvxs::Value pvxs_value, const sup::dto::AnyValue *any_value);

  pvxs::Value &GetPvxsValueRef() override;

  void ArrayElementSeparator() override;

  bool IsStructArrayNode() const override;

  void Freeze() override;

private:
  pvxs::shared_array<pvxs::Value> m_array;
  size_t m_current_index{0};
};

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_UTILS_PVXS_BUILDER_NODES_H_
