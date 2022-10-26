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

#include "pv_access_rpc_utils.h"

namespace sup
{
namespace epics
{

sup::dto::uint64 GetTimestamp()
{
  //TODO: use real time
  return 0;
}

sup::dto::AnyValue CreateRPCRequest(const sup::dto::AnyValue& payload)
{
  if (sup::dto::IsEmptyValue(payload))
  {
    return {};
  }
  sup::dto::AnyValue request = {{
    { "timestamp", {sup::dto::UnsignedInteger64Type, GetTimestamp()} },
    { "query", payload }
  }, "sup::RPCRequest/v1.0"};
  return request;
}

}  // namespace epics

}  // namespace sup
