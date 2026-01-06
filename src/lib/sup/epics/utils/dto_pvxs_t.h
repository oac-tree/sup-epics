/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Walter Van Herck (IO)
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

#ifndef SUP_EPICS_UTILS_DTO_PVXS_T_H_
#define SUP_EPICS_UTILS_DTO_PVXS_T_H_

#include <sup/dto/basic_scalar_types.h>

#include <cstdint>

namespace sup
{
namespace epics
{
template <typename T>
struct DTOToPVXSScalar
{
  using Type = T;
};

template <>
struct DTOToPVXSScalar<sup::dto::int64>
{
  using Type = std::int64_t;
};

template <>
struct DTOToPVXSScalar<sup::dto::uint64>
{
  using Type = std::uint64_t;
};

template <typename T>
using DTOToPVXSScalar_t = typename DTOToPVXSScalar<T>::Type;

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_UTILS_DTO_PVXS_T_H_
