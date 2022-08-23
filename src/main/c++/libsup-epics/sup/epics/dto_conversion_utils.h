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

#ifndef SUP_EPICS_DTO_CONVERSION_UTILS_H_
#define SUP_EPICS_DTO_CONVERSION_UTILS_H_

//! @file dto_conversion_utils.h
//! Collection of utility functions to convert sup-dto's AnyValue to PVXS values and back.

#include <sup/epics/dto_types_fwd.h>

#include <string>

namespace sup
{
namespace epics
{

//! Returns PVXS type from AnyType.
::pvxs::TypeDef BuildPVXSType(const ::sup::dto::AnyType& any_type);

//! Returns PVXS value from AnyValue.
::pvxs::Value BuildPVXSValue(const ::sup::dto::AnyValue& any_value);

//! Returns AnyValue from PVXS's value.
::sup::dto::AnyValue BuildAnyValue(const ::pvxs::Value& pvxs_value);

//! Convert scalar AnyValue to struct AnyValue with `value` field.
//! Used to publish scalars via PVXS server.
::sup::dto::AnyValue ConvertScalarToStruct(const ::sup::dto::AnyValue& any_value);

//! Convert struct AnyValue to scalar AnyValue. Struct must contain a single field named `value`.
//! Used to publish scalars via PVXS server.
::sup::dto::AnyValue ConvertStructToScalar(const ::sup::dto::AnyValue& any_value);

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_DTO_CONVERSION_UTILS_H_
