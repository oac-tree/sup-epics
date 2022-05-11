/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP common components for EPICS network protocol
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

#ifndef SUP_EPICS_DTOTYPES_FWD_H
#define SUP_EPICS_DTOTYPES_FWD_H

//! @file dtotypes_fwd.h
//! Forward declaration of some external types related to the data transfer.

namespace sup::dto
{
class AnyValue;
class AnyType;
}  // namespace sup::dto

namespace pvxs
{
class Value;
class TypeDef;
struct TypeCode;
}  // namespace pvxs

#endif  // SUP_EPICS_DTOTYPES_FWD_H
