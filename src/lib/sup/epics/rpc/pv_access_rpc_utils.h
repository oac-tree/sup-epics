/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2024 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 *****************************************************************************/

#ifndef SUP_EPICS_PV_ACCESS_RPC_UTILS_H_
#define SUP_EPICS_PV_ACCESS_RPC_UTILS_H_

#include <sup/epics/pv_access_rpc_client_config.h>

#include <sup/dto/anyvalue.h>
#include <sup/dto/any_functor.h>

#include <pvxs/client.h>

#include <memory>

namespace sup
{
namespace epics
{
namespace utils
{

sup::dto::AnyValue ClientRPCCall(std::shared_ptr<pvxs::client::Context> context,
                                 const PvAccessRPCClientConfig& config,
                                 const sup::dto::AnyValue& request);

pvxs::Value HandleRPCCall(sup::dto::AnyFunctor& handler, const pvxs::Value& pvxs_request);

}  // namespace utils

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_PV_ACCESS_RPC_UTILS_H_
