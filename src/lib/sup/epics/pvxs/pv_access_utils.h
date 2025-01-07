/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Gennady Pospelov (IO)
 *
 * Copyright (c) : 2010-2025 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 *****************************************************************************/

#ifndef SUP_EPICS_PV_ACCESS_UTILS_H_
#define SUP_EPICS_PV_ACCESS_UTILS_H_

#include <pvxs/client.h>
#include <pvxs/server.h>

#include <memory>

namespace sup
{
namespace epics
{
namespace utils
{

std::shared_ptr<pvxs::client::Context> GetSharedClientContext();

std::unique_ptr<pvxs::server::Server> CreateIsolatedServer();

std::unique_ptr<pvxs::server::Server> CreateServerFromEnv();

}  // namespace utils

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_PV_ACCESS_UTILS_H_
