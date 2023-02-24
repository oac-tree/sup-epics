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

#include "pv_access_rpc_server_impl.h"

#include <sup/epics/rpc/pv_access_rpc_utils.h>
#include <sup/epics/utils/dto_conversion_utils.h>

#include <sup/dto/anyvalue.h>

#include <pvxs/sharedpv.h>

namespace sup
{
namespace epics
{
PvAccessRPCServerImpl::PvAccessRPCServerImpl(std::unique_ptr<pvxs::server::Server>&& server,
                                             const PvAccessRPCServerConfig& config,
                                             sup::dto::AnyFunctor& handler)
  : m_server{std::move(server)}
  , m_config{config}
  , m_handler{handler}
  , m_client_context{}
{
  if (!m_server)
  {
    throw std::runtime_error("PvAccessRPCServer created without server or handler");
  }
  Initialise();
}

PvAccessRPCServerImpl::~PvAccessRPCServerImpl()
{
  m_server->stop();
}

std::shared_ptr<pvxs::client::Context> PvAccessRPCServerImpl::GetClientContext()
{
  if (!m_client_context)
  {
    m_client_context = std::make_shared<pvxs::client::Context>(m_server->clientConfig().build());
  }
  return m_client_context;
}

void PvAccessRPCServerImpl::Initialise()
{
  auto shared_pv = pvxs::server::SharedPV::buildMailbox();
  m_server->addPV(m_config.service_name, shared_pv);

  shared_pv.onRPC(
    [this](pvxs::server::SharedPV&, std::unique_ptr<pvxs::server::ExecOp>&& op,
           pvxs::Value&& pvxs_request)
    {
      auto pvxs_reply = utils::HandleRPCCall(m_handler, pvxs_request);
      op->reply(pvxs_reply);
    }
  );
  m_server->start();
}

std::unique_ptr<PvAccessRPCServerImpl> CreateIsolatedRPCServerImpl(
  const PvAccessRPCServerConfig& config, sup::dto::AnyFunctor& handler)
{
  auto server = std::unique_ptr<pvxs::server::Server>(
      new pvxs::server::Server(pvxs::server::Config::isolated()));
  return std::unique_ptr<PvAccessRPCServerImpl>(
    new PvAccessRPCServerImpl(std::move(server), config, handler));
}

std::unique_ptr<PvAccessRPCServerImpl> CreateRPCServerImplFromEnv(
  const PvAccessRPCServerConfig& config, sup::dto::AnyFunctor& handler)
{
  auto server = std::unique_ptr<pvxs::server::Server>(
      new pvxs::server::Server(pvxs::server::Config::fromEnv()));
  return std::unique_ptr<PvAccessRPCServerImpl>(
    new PvAccessRPCServerImpl(std::move(server), config, handler));
}

}  // namespace epics

}  // namespace sup
