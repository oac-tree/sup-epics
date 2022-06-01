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

#ifndef SUP_EPICS_CAHelper_H
#define SUP_EPICS_CAHelper_H

#include <sup/dto/AnyType.h>
#include <sup/dto/BasicScalarTypes.h>
#include <cadef.h>

namespace sup
{
namespace epics
{
namespace cahelper
{

void* GetValueFieldReference(event_handler_args args);

sup::dto::int16 GetStatusField(event_handler_args args);

sup::dto::int16 GetSeverityField(event_handler_args args);

sup::dto::uint64 GetTimestampField(event_handler_args args);

chtype ChannelType(const sup::dto::AnyType& anytype);

unsigned long ChannelMultiplicity(const sup::dto::AnyType& anytype);

}  // namespace cahelper

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_CAHelper_H
