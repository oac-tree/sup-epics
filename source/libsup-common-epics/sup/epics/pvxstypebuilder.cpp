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

#include "sup/epics/pvxstypebuilder.h"

#include <pvxs/data.h>

#include <iostream>
#include <stdexcept>

namespace sup::epics
{

struct PvxsTypeBuilder::PvxsTypeBuilderImpl
{
};

PvxsTypeBuilder::PvxsTypeBuilder() : p_impl(new PvxsTypeBuilderImpl) {}

PvxsTypeBuilder::~PvxsTypeBuilder() = default;

pvxs::TypeDef PvxsTypeBuilder::GetPVXSType() const
{
  return {};
}

void PvxsTypeBuilder::EmptyProlog(const sup::dto::AnyType* anytype)
{
  std::cout << "EmptyProlog() value:" << anytype << std::endl;
}

void PvxsTypeBuilder::EmptyEpilog(const sup::dto::AnyType* anytype)
{
  std::cout << "EmptyEpilog() value:" << anytype << std::endl;
}

void PvxsTypeBuilder::StructProlog(const sup::dto::AnyType* anytype)
{
  std::cout << "StructProlog() value:" << anytype << " item:" << std::endl;
}

void PvxsTypeBuilder::StructMemberSeparator()
{
  std::cout << "StructMemberSeparator() " << std::endl;
}

void PvxsTypeBuilder::StructEpilog(const sup::dto::AnyType* anytype)
{
  std::cout << "StructEpilog() value:" << anytype << std::endl;
}

void PvxsTypeBuilder::MemberProlog(const sup::dto::AnyType* anytype, const std::string& member_name)
{
  std::cout << "MemberProlog() " << anytype << " " << member_name << std::endl;
}

void PvxsTypeBuilder::MemberEpilog(const sup::dto::AnyType* anytype, const std::string& member_name)
{
  std::cout << "MemberEpilog() " << anytype << " " << member_name << std::endl;
}

void PvxsTypeBuilder::ArrayProlog(const sup::dto::AnyType* anytype)
{
  std::cout << "ArrayProlog() value:" << anytype << std::endl;
}

void PvxsTypeBuilder::ArrayElementSeparator()
{
  std::cout << "AddArrayElementSeparator() " << std::endl;
}

void PvxsTypeBuilder::ArrayEpilog(const sup::dto::AnyType* anytype)
{
  std::cout << "AddArrayEpilog() value:" << anytype << std::endl;
}

void PvxsTypeBuilder::ScalarProlog(const sup::dto::AnyType* anytype)
{
  std::cout << "ScalarProlog() value:" << anytype << std::endl;
}

void PvxsTypeBuilder::ScalarEpilog(const sup::dto::AnyType* anytype) {}

}  // namespace sup::epics
