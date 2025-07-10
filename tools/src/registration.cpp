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

#include "registered_names.h"
#include "rpc_sniff_server.h"

#include <sup/di/object_manager.h>
#include <sup/dto/json_value_parser.h>

// Register Robot configurationInterface
const bool RPCSniffServer_Registered =
    sup::di::GlobalObjectManager().RegisterFactoryFunction(
        RPC_SNIFF_SERVER_NAME, sup::di::ForwardingInstanceFactoryFunction<
            sup::dto::AnyFunctor, RPCSniffServer, const sup::dto::AnyValue&>);

// AnyValueFromJSONFile Factory Function
std::unique_ptr<sup::dto::AnyValue> AnyValueFromJSONFileFactoryFunction(const std::string& filepath)
{
  sup::dto::JSONAnyValueParser parser;
  parser.ParseFile(filepath);
  std::unique_ptr<sup::dto::AnyValue> result(new sup::dto::AnyValue(parser.MoveAnyValue()));
  return result;
}

// Register AnyValueFromJSONFile
const bool AnyValueFromJSONFile_Registered = sup::di::GlobalObjectManager().RegisterFactoryFunction(
    ANYVALUE_FROM_JSON_FILE_NAME, AnyValueFromJSONFileFactoryFunction);

