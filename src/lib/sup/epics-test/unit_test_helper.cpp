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

#include "unit_test_helper.h"

#include <sup/protocol/protocol_rpc.h>

#include <chrono>
#include <cmath>
#include <thread>

namespace sup
{
namespace epics
{
namespace test
{
bool BusyWaitFor(double timeout_sec, std::function<bool()> predicate)
{
  auto timedelta = std::chrono::duration<double>(timeout_sec);
  auto time_end = std::chrono::system_clock::now() + timedelta;
  while (!predicate() && std::chrono::system_clock::now() < time_end)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }
  return predicate();
}

void EchoFunction(const sup::dto::AnyValue& request, sup::dto::AnyValue& reply)
{
  reply = request;
}

FixedReplyFunctor::FixedReplyFunctor(sup::dto::AnyValue fixed_reply)
      : m_fixed_reply(std::move(fixed_reply))
{}

sup::dto::AnyValue FixedReplyFunctor::operator()(const sup::dto::AnyValue& request)
{
  m_last_request = request;
  return m_fixed_reply;
}

sup::dto::AnyValue FixedReplyFunctor::GetLastRequest() const
{
  return m_last_request;
}

FunctionFunctor::FunctionFunctor(
  std::function<void(const sup::dto::AnyValue&, sup::dto::AnyValue&)> func)
  : m_func{std::move(func)}
{}

FunctionFunctor::~FunctionFunctor() = default;

sup::dto::AnyValue FunctionFunctor::operator()(const sup::dto::AnyValue& request)
{
  sup::dto::AnyValue reply;
  if (!request.HasField(RETURN_EMPTY_FIELD) || request[RETURN_EMPTY_FIELD].As<bool>() == false)
  {
    reply = sup::protocol::utils::CreateRPCReply(sup::protocol::Success);
  }
  m_func(request, reply);
  return reply;
}

}  // namespace test

}  // namespace epics

}  // namespace sup
