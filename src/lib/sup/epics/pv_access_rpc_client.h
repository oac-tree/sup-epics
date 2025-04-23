/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2025 ITER Organization,
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

#ifndef SUP_EPICS_PV_ACCESS_RPC_CLIENT_H_
#define SUP_EPICS_PV_ACCESS_RPC_CLIENT_H_

#include "pv_access_rpc_client_config.h"

#include <sup/dto/any_functor.h>

#include <memory>

namespace sup
{
namespace epics
{
class PvAccessRPCClientImpl;

/** @brief PvAccess based implementation of an RPC client
 *
 * @details This PvAccess based implementation of an RPC client implements the AnyFunctor
 * interface (see sup-dto). It also implicitly assumes the transport protocol defined in sup-protocol:
 * although one can send (and receive) AnyValues that are not conforming to the transport protocol,
 * all error messages are implemented according to that protocol, meaning they contain a status
 * field, a timestamp and a reason field.
  */
class PvAccessRPCClient : public sup::dto::AnyFunctor
{
public:
  explicit PvAccessRPCClient(const PvAccessRPCClientConfig& config);

  explicit PvAccessRPCClient(std::unique_ptr<PvAccessRPCClientImpl>&& impl);

  ~PvAccessRPCClient();

  PvAccessRPCClient(PvAccessRPCClient&& other);
  PvAccessRPCClient& operator=(PvAccessRPCClient&& other);

  PvAccessRPCClient(const PvAccessRPCClient&) = delete;
  PvAccessRPCClient& operator=(const PvAccessRPCClient&) = delete;

  sup::dto::AnyValue operator()(const sup::dto::AnyValue& request) override;
private:
  std::unique_ptr<PvAccessRPCClientImpl> m_impl;
};

PvAccessRPCClientConfig GetDefaultRPCClientConfig(const std::string& service_name);

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_PV_ACCESS_RPC_CLIENT_H_
