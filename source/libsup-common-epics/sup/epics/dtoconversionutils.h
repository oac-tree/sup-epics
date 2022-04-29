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

#ifndef SUP_EPICS_DTOCONVERSIONUTILS_H
#define SUP_EPICS_DTOCONVERSIONUTILS_H

//! @file dtoconversionutils.h
//! Collection of utility functions to convert sup-dto's AnyValue to PVXS values and back.

#include "sup/epics/types_fwd.h"

#include <string>

namespace sup::epics
{
//! Returns PVXS value from scalar like AnyValue.
pvxs::Value GetPVXSValueFromScalar(const ::sup::dto::AnyValue& any_value);

//! Returns PVXS value from AnyValue.
::pvxs::Value BuildPVXSValue(const ::sup::dto::AnyValue& any_value);

}  // namespace sup::epics

#endif  // SUP_EPICS_DTOCONVERSIONUTILS_H
