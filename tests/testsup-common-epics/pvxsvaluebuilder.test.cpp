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

#include "AnyTypeHelper.h"
#include "AnyValue.h"
#include "sup/epics/pvxstypebuilder.h"

#include <gtest/gtest.h>
#include <pvxs/data.h>
#include <pvxs/nt.h>

using namespace ::sup::epics;

class PvxsValueBuilderTest : public ::testing::Test
{
public:
  static pvxs::Value GetPVXSValue(const sup::dto::AnyValue& any_value)
  {
    PvxsTypeBuilder type_builder;
    sup::dto::SerializeAnyType(any_value.GetType(), type_builder);
    auto pvxs_type = type_builder.GetPVXSType();

    PvxsValueBuilder value_builder(pvxs_type);
    sup::dto::SerializeAnyValue(any_value, value_builder);
    return value_builder.GetPVXSValue();
  }
};

//! Investigating PVXS value itself.

TEST_F(PvxsValueBuilderTest, PVXSValueBasics)
{
  // default constructed
  pvxs::Value pvxs_default1;
  EXPECT_FALSE(pvxs_default1.valid());
  EXPECT_EQ(pvxs_default1.type(), ::pvxs::TypeCode::Null);

  pvxs::Value pvxs_default2;
  EXPECT_TRUE(pvxs_default1.equalType(pvxs_default2));
  EXPECT_TRUE(pvxs_default1.equalInst(pvxs_default2));  // Shouldn't be false?

  // from int32
  pvxs::Value pvxs_int1 = pvxs::TypeDef(pvxs::TypeCode::Int32).create();
  pvxs_int1 = 42;
  EXPECT_TRUE(pvxs_int1.valid());
  EXPECT_EQ(pvxs_int1.as<int>(), 42);

  pvxs::Value pvxs_int2 = pvxs::TypeDef(pvxs::TypeCode::Int32).create();
  EXPECT_TRUE(pvxs_int1.equalType(pvxs_int2));
  EXPECT_FALSE(pvxs_int1.equalInst(pvxs_int2));
}

//! Build PVXS value from empty AnyValue.

TEST_F(PvxsValueBuilderTest, BuildPVXSValueFromEmpty)
{
  // investigating default constructed PVXS
  pvxs::Value pvxs_default;

  // constructing from empty AnyValue
  sup::dto::AnyValue any_value;
  auto pvxs_value = GetPVXSValue(any_value);
  EXPECT_FALSE(pvxs_value.valid());
  EXPECT_TRUE(pvxs_value.equalType(pvxs_default));
}

//! Build PVXS value from scalar like AnyValue.

// TEST_F(PvxsValueBuilderTest, BuildPVXSValueFromSignedInteger32)
//{
//   sup::dto::AnyValue any_value{sup::dto::SignedInteger32};
//   any_value = 42;

//  auto pvxs_value = GetPVXSValue(any_value);
//  EXPECT_TRUE(pvxs_value.valid());
//  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Int32);
//  EXPECT_EQ(pvxs_value.as<int32_t>(), 42);

//  // other basic types are performed via DtoConversionUtilsTest::GetPVXSValueFromScalar testing
//}

////! Build PVXS value from AnyValue representing a struct with single field.

// TEST_F(PvxsValueBuilderTest, BuildPVXSValueFromStructWithSingleField)
//{
//   sup::dto::AnyValue any_value = {{{"signed", {sup::dto::SignedInteger32, 42}}}};

//  auto pvxs_value = GetPVXSValue(any_value);
//  //  EXPECT_TRUE(pvxs_value.valid());
//  //  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Int32);
//  //  EXPECT_EQ(pvxs_value.as<int32_t>(), 42);

//  // other basic types are performed via GetPVXSValueFromScalar testing
//}
