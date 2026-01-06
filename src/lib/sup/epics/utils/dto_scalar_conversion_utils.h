/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Gennady Pospelov (IO)
 *
 * Copyright (c) : 2010-2026 ITER Organization,
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

#ifndef SUP_EPICS_DTO_UTILS_ANYVALUE_SCALAR_CONVERSION_UTILS_H_
#define SUP_EPICS_DTO_UTILS_ANYVALUE_SCALAR_CONVERSION_UTILS_H_

//! @file dto_scalar_conversion_utils.h
//! Collection of utility functions to convert sup-dto's AnyValue scalars to PVXS values.

#include <sup/epics/utils/dto_types_fwd.h>

#include <string>

namespace sup
{
namespace epics
{

//! Returns PVXS value from scalar like AnyValue.
::pvxs::Value GetPVXSValueFromScalar(const ::sup::dto::AnyValue& any_value);

//! Assigns value of the given scalar-like AnyValue to PVXS value.
//! It is expected that AnyValue type matches PVXS type.
void AssignAnyValueToPVXSValueScalar(const ::sup::dto::AnyValue& any_value,
                                     ::pvxs::Value& pvxs_value);

//! Assigns elements of the given array-like AnyValue to PVXS value.
//! It is expected that AnyValue type matches PVXS type.
void AssignAnyValueToPVXSValueScalarArray(const ::sup::dto::AnyValue& any_value,
                                          ::pvxs::Value& pvxs_value);

//! Assigns value of the given scalar-like PVXS value to AnyValue.
//! It is expected that AnyValue type matches PVXS type.
void AssignPVXSValueToAnyValueScalar(const ::pvxs::Value& pvxs_value,
                                     ::sup::dto::AnyValue& any_value);

//! Returns AnyValue constructed and initialised from scalar-like PVXS Value.
::sup::dto::AnyValue GetAnyValueFromScalar(const ::pvxs::Value& pvxs_value);

//! Returns AnyValue constructed and initialised from PVXS Value containing scalar array.
::sup::dto::AnyValue GetAnyValueFromScalarArray(const ::pvxs::Value& pvxs_value);

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_DTO_UTILS_ANYVALUE_SCALAR_CONVERSION_UTILS_H_
