/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Walter Van Herck (IO)
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

#ifndef SUP_EPICS_TEST_UNIT_TEST_HELPER_H_
#define SUP_EPICS_TEST_UNIT_TEST_HELPER_H_

#include <sup/dto/anyvalue.h>
#include <sup/dto/any_functor.h>

#include <functional>

namespace sup
{
namespace epics
{
namespace test
{

static const std::string RETURN_EMPTY_FIELD = "return_empty";

bool BusyWaitFor(double timeout_sec, std::function<bool()> predicate);

void EchoFunction(const sup::dto::AnyValue& request, sup::dto::AnyValue& reply);

class FixedReplyFunctor : public sup::dto::AnyFunctor
{
public:
  explicit FixedReplyFunctor(sup::dto::AnyValue fixed_reply);

  sup::dto::AnyValue operator()(const sup::dto::AnyValue& request) override;

  sup::dto::AnyValue GetLastRequest() const;

private:
  sup::dto::AnyValue m_fixed_reply;
  sup::dto::AnyValue m_last_request;
};

class FunctionFunctor : public sup::dto::AnyFunctor
{
public:
  FunctionFunctor(std::function<void(const sup::dto::AnyValue&, sup::dto::AnyValue&)> func);
  ~FunctionFunctor();

  sup::dto::AnyValue operator()(const sup::dto::AnyValue& request) override;
private:
  std::function<void(const sup::dto::AnyValue&, sup::dto::AnyValue&)> m_func;
};

}  // namespace test

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_TEST_UNIT_TEST_HELPER_H_
