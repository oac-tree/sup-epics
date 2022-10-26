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

#include <sup/rpc/protocol.h>

namespace sup
{
namespace epics
{
class PvAccessRPCClient : public rpc::Protocol
{
public:
  explicit PvAccessRPCClient(const PvAccessRPCClientConfig& config);

  ~PvAccessRPCClient();

  rpc::ProtocolResult Invoke(const sup::dto::AnyValue& input, sup::dto::AnyValue& output) override;
private:
  PvAccessRPCClientConfig m_config;
};

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_PV_ACCESS_RPC_CLIENT_H_
