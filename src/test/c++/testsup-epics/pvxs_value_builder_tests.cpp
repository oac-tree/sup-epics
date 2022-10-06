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

#include "sup/epics/utils/pvxs_value_builder.h"

#include <gtest/gtest.h>
#include <pvxs/data.h>
#include <pvxs/nt.h>
#include <sup/dto/anyvalue.h>
#include <sup/epics/dto_conversion_utils.h>
#include <sup/epics/utils/pvxs_utils.h>

using namespace ::sup::epics;

//! Testing various methods of PVXSValueBuilder.
//! See integration tests in `pvxs_value_builder_extended_tests.cpp`.

class PvxsValueBuilderTests : public ::testing::Test
{
};

//! Build PVXS value from empty AnyValue.

TEST_F(PvxsValueBuilderTests, FromEmptyType)
{
  pvxs::TypeDef empty_type;

  PvxsValueBuilder builder(empty_type);
  auto pvxs_value = builder.GetPVXSValue();

  EXPECT_FALSE(pvxs_value.valid());
  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Null);
}

TEST_F(PvxsValueBuilderTests, FromScalar)
{
  auto pvxs_type = ::pvxs::TypeDef(::pvxs::TypeCode::Int32);
  PvxsValueBuilder builder(pvxs_type);

  sup::dto::AnyValue anyvalue{sup::dto::SignedInteger32Type, 42};

  builder.ScalarProlog(&anyvalue);
  builder.ScalarEpilog(&anyvalue);

  auto pvxs_value = builder.GetPVXSValue();

  EXPECT_TRUE(pvxs_value.valid());
  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value.as<int32_t>(), 42);
}
