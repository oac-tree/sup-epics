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

#ifndef SUP_EPICS_UTILS_PVXS_UTILS_H_
#define SUP_EPICS_UTILS_PVXS_UTILS_H_

//! @file pvxs_utils.h
//! Collection of utils for PVXS's types and values.

#include <sup/epics/dto_types_fwd.h>

#include <vector>

namespace sup
{
namespace epics
{

//! Returns true if given type_code is referring to a scalar.
bool IsScalar(const ::pvxs::TypeCode& type_code);

//! Returns true if given value contains a scalar.
bool IsScalar(const ::pvxs::Value& value);

//! Returns true if given type_code is referring to a scalar array..
bool IsScalarArray(const ::pvxs::TypeCode& type_code);

//! Returns true if given value contains a scalar array.
bool IsScalarArray(const ::pvxs::Value& value);

//! Returns true if given value is a struct.
bool IsStruct(const ::pvxs::Value& value);

//! Returns vector of children.
std::vector<::pvxs::Value> GetChildren(const pvxs::Value& pvxs_value);

}  // namespace epics
}  // namespace sup

#endif  // SUP_EPICS_UTILS_PVXS_UTILS_H_
