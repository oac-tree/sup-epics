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

#ifndef SUP_EPICS_PV_ACCESS_RPC_UTILS_H_
#define SUP_EPICS_PV_ACCESS_RPC_UTILS_H_

#include <sup/epics/pv_access_rpc_client_config.h>

#include <sup/dto/anyvalue.h>
#include <sup/rpc/protocol_result.h>

#include <pvxs/client.h>

#include <memory>

namespace sup
{
namespace epics
{
namespace utils
{
namespace constants
{
static const std::string REQUEST_TYPE_NAME = "sup::RPCRequest/v1.0";
static const std::string REQUEST_TIMESTAMP = "timestamp";
static const std::string REQUEST_PAYLOAD = "query";

static const std::string REPLY_TYPE_NAME = "sup::RPCReply/v1.0";
static const std::string REPLY_RESULT = "result";
static const std::string REPLY_TIMESTAMP = "timestamp";
static const std::string REPLY_REASON = "reason";
static const std::string REPLY_PAYLOAD = "reply";
}  // namespace constants

sup::dto::uint64 GetTimestamp();

bool CheckReplyFormat(const sup::dto::AnyValue& reply);

sup::dto::AnyValue CreateRPCRequest(const sup::dto::AnyValue& payload);

sup::dto::AnyValue CreateRPCReply(const sup::rpc::ProtocolResult& result,
                                  const std::string& reason,
                                  const sup::dto::AnyValue& payload);

sup::dto::AnyValue CreateRPCBaseReply(const sup::rpc::ProtocolResult& result,
                                      const sup::dto::uint64 timestamp = 0,
                                      const std::string& reason = {});

sup::dto::AnyValue ClientRPCCall(std::shared_ptr<pvxs::client::Context> context,
                                 const PvAccessRPCClientConfig& config,
                                 const sup::dto::AnyValue& request);

}  // namespace utils

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_PV_ACCESS_RPC_UTILS_H_
