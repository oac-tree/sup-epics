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

#include "sup/epics/pvxs/context_utils.h"

#include <pvxs/server.h>
#include <pvxs/client.h>

namespace sup
{
namespace epics
{

std::unique_ptr<pvxs::server::Server> CreateIsolatedServer()
{
  return std::unique_ptr<pvxs::server::Server>(
      new pvxs::server::Server(pvxs::server::Config::isolated()));
}

std::unique_ptr<pvxs::server::Server> CreateServerFromEnv()
{
  return std::unique_ptr<pvxs::server::Server>(
        new pvxs::server::Server(pvxs::server::Config::fromEnv()));
}

std::shared_ptr<pvxs::client::Context> CreateClientContextFromEnv()
{
  return std::make_shared<pvxs::client::Context>(pvxs::client::Context::fromEnv());
}

}  // namespace epics
}  // namespace sup
