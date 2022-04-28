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

#include "sup/epics/pvxsvaluebuilder.h"

#include <pvxs/data.h>

#include <iostream>
#include <stdexcept>

namespace sup::epics
{

PvxsValueBuilder::PvxsValueBuilder() : m_value(new pvxs::Value) {}

pvxs::Value PvxsValueBuilder::GetPVXSValue() const
{
  return *m_value;
}

void PvxsValueBuilder::EmptyProlog(const sup::dto::AnyValue *anyvalue)
{
  std::cout << "EmptyProlog() value:" << anyvalue << std::endl;
}

void PvxsValueBuilder::EmptyEpilog(const sup::dto::AnyValue *anyvalue)
{
  std::cout << "EmptyEpilog() value:" << anyvalue << std::endl;
}

void PvxsValueBuilder::StructProlog(const sup::dto::AnyValue *anyvalue)
{
  std::cout << "StructProlog() value:" << anyvalue << " item:" << std::endl;
}

void PvxsValueBuilder::StructMemberSeparator()
{
  std::cout << "StructMemberSeparator() " << std::endl;
}

void PvxsValueBuilder::StructEpilog(const sup::dto::AnyValue *anyvalue)
{
  std::cout << "StructEpilog() value:" << anyvalue << std::endl;
}

void PvxsValueBuilder::MemberProlog(const sup::dto::AnyValue *anyvalue,
                                    const std::string &member_name)
{
  std::cout << "MemberProlog() " << anyvalue << " " << member_name << std::endl;
}

void PvxsValueBuilder::MemberEpilog(const sup::dto::AnyValue *anyvalue,
                                    const std::string &member_name)
{
  std::cout << "MemberEpilog() " << anyvalue << " " << member_name << std::endl;
}

void PvxsValueBuilder::ArrayProlog(const sup::dto::AnyValue *anyvalue)
{
  std::cout << "ArrayProlog() value:" << anyvalue << std::endl;
}

void PvxsValueBuilder::ArrayElementSeparator()
{
  std::cout << "AddArrayElementSeparator() " << std::endl;
}

void PvxsValueBuilder::ArrayEpilog(const sup::dto::AnyValue *anyvalue)
{
  std::cout << "AddArrayEpilog() value:" << anyvalue << std::endl;
}

void PvxsValueBuilder::ScalarProlog(const sup::dto::AnyValue *anyvalue)
{
  std::cout << "ScalarProlog() value:" << anyvalue << std::endl;
}

void PvxsValueBuilder::ScalarEpilog(const sup::dto::AnyValue *anyvalue)
{
  std::cout << "ScalarEpilog() value:" << anyvalue << std::endl;
}

}  // namespace sup::epics
