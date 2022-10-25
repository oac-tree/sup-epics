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

#ifndef SUP_EPICS_PV_ACCESS_SERVER_IMPL_H_
#define SUP_EPICS_PV_ACCESS_SERVER_IMPL_H_

#include <sup/epics/pv_access_server.h>
#include <sup/epics/pvxs/pv_access_server_pv.h>

#include <map>
#include <memory>

namespace sup
{
namespace epics
{
class PvAccessServerImpl
{
public:
  PvAccessServerImpl(std::unique_ptr<pvxs::server::Server>&& context,
                     PvAccessServer::VariableChangedCallback callback);

  //! Adds channel with given name to the map of channels.
  void AddVariable(const std::string& name, const dto::AnyValue& any_value);

  std::vector<std::string> GetVariableNames() const;

  const std::map<std::string, std::unique_ptr<PvAccessServerPV>>& GetVariables() const;

  std::unique_ptr<pvxs::server::Server>& GetContext();

  std::shared_ptr<pvxs::client::Context> GetClientContext();

private:
  void OnVariableChanged(const std::string& name, const sup::dto::AnyValue& any_value);
  std::unique_ptr<pvxs::server::Server> m_context;
  PvAccessServer::VariableChangedCallback m_callback;
  std::map<std::string, std::unique_ptr<PvAccessServerPV>> m_variables;
  std::shared_ptr<pvxs::client::Context> m_client_context;
};

//! Creates PvAccess server implementation suitable for unit tests.
std::unique_ptr<PvAccessServerImpl> CreateIsolatedServerImpl(
  PvAccessServer::VariableChangedCallback cb);

//! Creates PvAccess server implementation from $EPICS_PVA* environment variables.
std::unique_ptr<PvAccessServerImpl> CreateServerImplFromEnv(
  PvAccessServer::VariableChangedCallback cb);

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_PV_ACCESS_SERVER_IMPL_H_
