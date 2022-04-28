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

#include "AnyValue.h"
#include "sup/epics/utils.h"

#include <gtest/gtest.h>
#include <pvxs/data.h>
#include <pvxs/nt.h>

using namespace ::sup::epics;

class UtilsTest : public ::testing::Test
{
};

//! Investigating PVXS value itself,

TEST_F(UtilsTest, ScalarBasedPVXSValue)
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

//! Get PVXS value from empty AnyValue.

TEST_F(UtilsTest, GetPVXSValueFromEmpty)
{
  // investigating default constructed PVXS
  pvxs::Value pvxs_default;

  // constructing from empty AnyValue
  sup::dto::AnyValue any_value;
  auto pvxs_value = GetPVXSValue(any_value);
  EXPECT_FALSE(pvxs_value.valid());
  EXPECT_TRUE(pvxs_value.equalType(pvxs_default));
  EXPECT_TRUE(pvxs_value.equalInst(pvxs_default));  // Shouldn't it be false?
}

TEST_F(UtilsTest, GetPVXSValueFromInt32)
{
  pvxs::Value pvxs_int = pvxs::TypeDef(pvxs::TypeCode::Int32).create();
  pvxs_int = 42;

  sup::dto::AnyValue any_value;
  auto result = GetPVXSValue(any_value);
//  EXPECT_TRUE(result.valid());
}
