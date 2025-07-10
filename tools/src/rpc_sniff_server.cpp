/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2025 ITER Organization,
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

#include "rpc_sniff_server.h"

#include <sup/dto/anyvalue_helper.h>

#include <iostream>

RPCSniffServer::RPCSniffServer(const sup::dto::AnyValue& reply)
  : m_reply{reply}
{}

RPCSniffServer::~RPCSniffServer() = default;

sup::dto::AnyValue RPCSniffServer::operator()(const sup::dto::AnyValue& input)
{
  std::cout << sup::dto::PrintAnyValue(input) << std::endl;
  return m_reply;
}
