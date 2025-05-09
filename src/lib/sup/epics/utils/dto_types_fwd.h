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
 * SPDX-License-Identifier: MIT
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file LICENSE located in the top level directory
 * of the distribution package.
 *****************************************************************************/

#ifndef SUP_EPICS_DTO_TYPES_FWD_H_
#define SUP_EPICS_DTO_TYPES_FWD_H_

#include <sup/dto/basic_scalar_types.h>

//! @file dto_types_fwd.h
//! Forward declaration of some external types related to the data transfer.

namespace sup
{
namespace dto
{
class AnyValue;
class AnyType;
enum class TypeCode : sup::dto::uint32;
}  // namespace dto
}  // namespace sup

namespace pvxs
{
class Value;
class TypeDef;
struct TypeCode;

namespace client
{
class Context;
}

namespace server
{
class Server;
}
}  // namespace pvxs

#endif  // SUP_EPICS_DTO_TYPES_FWD_H_
