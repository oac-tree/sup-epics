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

#include "pv_access_utils.h"

#include <mutex>

namespace sup
{
namespace epics
{
namespace utils
{

std::shared_ptr<pvxs::client::Context> GetSharedClientContext()
{
  static std::shared_ptr<pvxs::client::Context> context =
    std::make_shared<pvxs::client::Context>(pvxs::client::Context::fromEnv());
  return context;
}

std::mutex g_server_mtx;

std::unique_ptr<pvxs::server::Server> CreateIsolatedServer()
{
  std::lock_guard<std::mutex> lk{g_server_mtx};
  return std::make_unique<pvxs::server::Server>(pvxs::server::Config::isolated());
}

std::unique_ptr<pvxs::server::Server> CreateServerFromEnv()
{
  std::lock_guard<std::mutex> lk{g_server_mtx};
  return std::make_unique<pvxs::server::Server>(pvxs::server::Config::fromEnv());
}

}  // namespace utils

}  // namespace epics

}  // namespace sup
