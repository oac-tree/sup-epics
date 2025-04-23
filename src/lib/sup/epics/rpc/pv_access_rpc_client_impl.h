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

#ifndef SUP_EPICS_PV_ACCESS_RPC_CLIENT_IMPL_H_
#define SUP_EPICS_PV_ACCESS_RPC_CLIENT_IMPL_H_

#include <sup/epics/pv_access_rpc_client_config.h>

#include <sup/protocol/protocol.h>

#include <pvxs/client.h>

#include <memory>

namespace sup
{
namespace epics
{
class PvAccessRPCClientImpl
{
public:
  PvAccessRPCClientImpl(const PvAccessRPCClientConfig& config,
                        std::shared_ptr<pvxs::client::Context> context);

  ~PvAccessRPCClientImpl();

  sup::dto::AnyValue operator()(const sup::dto::AnyValue& request);
private:
  PvAccessRPCClientConfig m_config;
  std::shared_ptr<pvxs::client::Context> m_context;
};

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_PV_ACCESS_RPC_CLIENT_IMPL_H_
