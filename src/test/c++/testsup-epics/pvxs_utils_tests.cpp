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

#include "sup/epics/pvxs_utils.h"

#include <gtest/gtest.h>
#include <pvxs/data.h>
#include <pvxs/nt.h>

using namespace ::sup::epics;

class PvxsUtilsTests : public ::testing::Test
{
};

TEST_F(PvxsUtilsTests, IsScalar)
{
  using pvxs::TypeCode;
  using pvxs::TypeDef;

  EXPECT_TRUE(IsScalar(TypeDef(TypeCode::Bool).create()));
  EXPECT_TRUE(IsScalar(TypeDef(TypeCode::Int8).create()));
  EXPECT_TRUE(IsScalar(TypeDef(TypeCode::UInt8).create()));
  EXPECT_TRUE(IsScalar(TypeDef(TypeCode::Int16).create()));
  EXPECT_TRUE(IsScalar(TypeDef(TypeCode::UInt16).create()));
  EXPECT_TRUE(IsScalar(TypeDef(TypeCode::Int32).create()));
  EXPECT_TRUE(IsScalar(TypeDef(TypeCode::UInt32).create()));
  EXPECT_TRUE(IsScalar(TypeDef(TypeCode::Int64).create()));
  EXPECT_TRUE(IsScalar(TypeDef(TypeCode::UInt64).create()));
  EXPECT_TRUE(IsScalar(TypeDef(TypeCode::Float32).create()));
  EXPECT_TRUE(IsScalar(TypeDef(TypeCode::Float64).create()));
  EXPECT_TRUE(IsScalar(TypeDef(TypeCode::String).create()));

  EXPECT_FALSE(IsScalar(TypeDef(TypeCode::BoolA).create()));
  EXPECT_FALSE(IsScalar(TypeDef(TypeCode::Int8A).create()));
  EXPECT_FALSE(IsScalar(TypeDef(TypeCode::UInt8A).create()));
  EXPECT_FALSE(IsScalar(TypeDef(TypeCode::Int16A).create()));
  EXPECT_FALSE(IsScalar(TypeDef(TypeCode::UInt16A).create()));
  EXPECT_FALSE(IsScalar(TypeDef(TypeCode::Int32A).create()));
  EXPECT_FALSE(IsScalar(TypeDef(TypeCode::UInt32A).create()));
  EXPECT_FALSE(IsScalar(TypeDef(TypeCode::Int64A).create()));
  EXPECT_FALSE(IsScalar(TypeDef(TypeCode::UInt64A).create()));
  EXPECT_FALSE(IsScalar(TypeDef(TypeCode::Float32A).create()));
  EXPECT_FALSE(IsScalar(TypeDef(TypeCode::Float64A).create()));
  EXPECT_FALSE(IsScalar(TypeDef(TypeCode::StringA).create()));

  EXPECT_FALSE(IsScalar(TypeDef(TypeCode::Struct).create()));
  EXPECT_FALSE(IsScalar(TypeDef(TypeCode::Union).create()));
  EXPECT_FALSE(IsScalar(TypeDef(TypeCode::UnionA).create()));
}

TEST_F(PvxsUtilsTests, IsStruct)
{
  using pvxs::TypeCode;
  using pvxs::TypeDef;

  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::Bool).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::Int8).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::UInt8).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::Int16).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::UInt16).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::Int32).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::UInt32).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::Int64).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::UInt64).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::Float32).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::Float64).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::String).create()));

  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::BoolA).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::Int8A).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::UInt8A).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::Int16A).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::UInt16A).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::Int32A).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::UInt32A).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::Int64A).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::UInt64A).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::Float32A).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::Float64A).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::StringA).create()));

  EXPECT_TRUE(IsStruct(TypeDef(TypeCode::Struct).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::Union).create()));
  EXPECT_FALSE(IsStruct(TypeDef(TypeCode::UnionA).create()));
}
