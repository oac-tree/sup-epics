/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Gennady Pospelov (IO)
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

#ifndef SUP_EPICS_UTILS_DTO_TYPECODE_CONVERSION_UTILS_H_
#define SUP_EPICS_UTILS_DTO_TYPECODE_CONVERSION_UTILS_H_

//! @file dto_typecode_conversion_utils.h
//! Collection of utility functions to convert TypeCode of AnyValue to the TypeCode of PVXS value.

#include <sup/epics/utils/dto_types_fwd.h>

#include <string>

namespace sup
{
namespace epics
{

//! Returns PVXS TypeCode corresponding to the given AnyType.
//! Distinguishes base types and array elements.
pvxs::TypeCode GetPVXSTypeCode(const ::sup::dto::AnyType& any_type);

//! Returns PVXS TypeCode corresponding to the given AnyType (for base types).
pvxs::TypeCode GetPVXSBaseTypeCode(const ::sup::dto::AnyType& any_type);

//! Returns PVXS TypeCode corresponding to the given AnyType (for array elements).
pvxs::TypeCode GetPVXSArrayTypeCode(const ::sup::dto::AnyType& any_type);

//! Returns AnyType corresponding to the given PVXS's TypeCode.
::sup::dto::TypeCode GetAnyTypeCode(const ::pvxs::TypeCode& pvxs_type);

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_UTILS_DTO_TYPECODE_CONVERSION_UTILS_H_
