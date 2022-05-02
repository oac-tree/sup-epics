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

#include "AnyTypeHelper.h"
#include "AnyValue.h"

#include <gtest/gtest.h>
#include <pvxs/data.h>
#include <pvxs/nt.h>

#include <iostream>

using namespace ::sup::epics;

//! Testing PvxsTypeBuilder class to build pvxs::TypeDef from AnyType's.

class PvxsTypeBuilderTest : public ::testing::Test
{
public:
  //! Creates pvxs::TypeDef from sup::dto::AnyType. Main helper method to test the builder.
  static pvxs::TypeDef GetPVXSType(const sup::dto::AnyType& any_type)
  {
    PvxsTypeBuilder builder;
    sup::dto::SerializeAnyType(any_type, builder);
    return builder.GetPVXSType();
  }

  std::vector<std::string> GetMemberNames(const ::pvxs::Value& pvxs_value)
  {
    std::vector<std::string> result;
    for (auto fld : pvxs_value.ichildren())
    {
      result.push_back(pvxs_value.nameOf(fld));
    }
    return result;
  }
};

//! Testing GetPVXSType method to build pvxs::TypeDef from scalar-like types.

TEST_F(PvxsTypeBuilderTest, BuildPVXSTypeFromScalarType)
{
  sup::dto::AnyType any_type(sup::dto::SignedInteger32);

  // The only way to check if pvxs::TypeDef is correctly created is to create
  // a pvxs::Value from it, and check it's type code.
  auto pvxs_value = GetPVXSType(any_type).create();

  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value.nmembers(), 0);

  // tests for other basic scalars are done in DtoConversionUtilsTest::GetPVXSTypeCode
}

//! Build PVXS value from AnyValue representing a struct with single field.

TEST_F(PvxsTypeBuilderTest, BuildPVXSTypeFromStructWithSingleField)
{
  sup::dto::AnyType any_type = {{"signed", {sup::dto::SignedInteger32}}};
  auto pvxs_value = GetPVXSType(any_type).create();

  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Struct);
  EXPECT_EQ(pvxs_value.nmembers(), 1);

  auto names = GetMemberNames(pvxs_value);
  EXPECT_EQ(names, std::vector<std::string>({"signed"}));
  EXPECT_EQ(pvxs_value["signed"].type(), ::pvxs::TypeCode::Int32);
}

//! Build PVXS value from AnyValue representing a struct with two fields.

TEST_F(PvxsTypeBuilderTest, BuildPVXSTypeFromStructWithTwoFields)
{
  sup::dto::AnyType any_type = {{"signed", {sup::dto::SignedInteger32}},
                                {"bool", {sup::dto::Boolean}}};
  auto pvxs_value = GetPVXSType(any_type).create();

  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Struct);
  EXPECT_EQ(pvxs_value.nmembers(), 2);

  auto names = GetMemberNames(pvxs_value);
  EXPECT_EQ(names, std::vector<std::string>({"signed", "bool"}));

  EXPECT_EQ(pvxs_value["signed"].type(), ::pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value["bool"].type(), ::pvxs::TypeCode::Bool);
}
