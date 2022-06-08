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

#include <sup/epics/dto_conversion_utils.h>

#include <sup/dto/anytype_helper.h>
#include <sup/dto/anyvalue.h>

#include <gtest/gtest.h>
#include <pvxs/data.h>
#include <pvxs/nt.h>

#include <iostream>

using namespace ::sup::epics;

//! Testing AnyTypeBuilder class to build AnyType from PVXS's TypeDef.
//! Testing is done via convenience function BuildAnyType.

class AnyTypeBuilderTests : public ::testing::Test
{
};

TEST_F(AnyTypeBuilderTests, ScalarTypes)
{
  pvxs::TypeDef type_def(::pvxs::TypeCode::Int32);

  auto anytype = BuildAnyType(type_def);
  EXPECT_EQ(anytype.GetTypeCode(), ::sup::dto::TypeCode::Int32);
  EXPECT_EQ(anytype, ::sup::dto::SignedInteger32Type);
}

