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

#ifndef SUP_EPICS_PV_ACCESS_CONTEXT_UTILS_H_
#define SUP_EPICS_PV_ACCESS_CONTEXT_UTILS_H_

//! @file pv_access_context_utils.h
//! Collection of utility functions related to the configuration of PVXS client/server.
//! The main purpose of this file is to hide PVXS internals as much as possible.

#include <sup/epics/dto_types_fwd.h>

#include <memory>

namespace sup
{
namespace epics
{

//! Creates PVXS server suitable for unit tests.
std::unique_ptr<pvxs::server::Server> CreateIsolatedServer();

//! Creates PVXS server based on configuration from $EPICS_PVA* environment variables.
std::unique_ptr<pvxs::server::Server> CreateServerFromEnv();

//! Create client context based on configuration from $EPICS_PVA* environment variables.
std::shared_ptr<pvxs::client::Context> CreateClientContextFromEnv();

}  // namespace epics
}  // namespace sup

#endif  // SUP_EPICS_PV_ACCESS_CONTEXT_UTILS_H_
