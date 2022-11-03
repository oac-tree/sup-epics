/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Walter Van Herck (IO)
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
