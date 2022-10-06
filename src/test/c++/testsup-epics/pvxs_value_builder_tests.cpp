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

//! Build PVXS value from a scalar AnyValue.

TEST_F(PvxsValueBuilderTests, FromScalar)
{
  // original any_value
  sup::dto::AnyValue any_value{sup::dto::SignedInteger32Type, 42};

  // initializing builder
  auto pvxs_type = ::pvxs::TypeDef(::pvxs::TypeCode::Int32);
  PvxsValueBuilder builder(pvxs_type);

  // building PVXS value
  builder.ScalarProlog(&any_value);
  builder.ScalarEpilog(&any_value);

  auto pvxs_value = builder.GetPVXSValue();

  // validating pvxs_value
  EXPECT_TRUE(pvxs_value.valid());
  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value.as<int32_t>(), 42);
}

//! Build PVXS value from a structure with single field.

TEST_F(PvxsValueBuilderTests, StructWithSingleField)
{
  // original any_value
  sup::dto::AnyValue any_value = {{{"signed", {sup::dto::SignedInteger32Type, 42}}}};

  // initializing builder
  auto pvxs_type = ::pvxs::TypeDef(::pvxs::TypeCode::Struct, {pvxs::members::Int32("signed")});
  PvxsValueBuilder builder(pvxs_type);

  // building PVXS value
  builder.StructProlog(&any_value);
  builder.MemberProlog(&any_value["signed"], "signed");
  builder.ScalarProlog(&any_value["signed"]);
  builder.ScalarEpilog(&any_value["signed"]);
  builder.MemberEpilog(&any_value["signed"], "signed");
  builder.StructEpilog(&any_value);

  auto pvxs_value = builder.GetPVXSValue();

  // validating pvxs_value
  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Struct);
  EXPECT_EQ(pvxs_value.nmembers(), 1);

  auto names = GetMemberNames(pvxs_value);
  EXPECT_EQ(names, std::vector<std::string>({"signed"}));
  EXPECT_EQ(pvxs_value["signed"].type(), ::pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value["signed"].as<int32_t>(), 42);
}

//! Build PVXS value from a structure with two fields.

TEST_F(PvxsValueBuilderTests, StructWithTwoFields)
{
  // original any_value
  sup::dto::AnyValue any_value = {{"signed", {sup::dto::SignedInteger32Type, 42}},
                                  {"bool", {sup::dto::BooleanType, true}}};

  // initializing builder
  auto pvxs_type = ::pvxs::TypeDef(::pvxs::TypeCode::Struct,
                                   {pvxs::members::Int32("signed"), pvxs::members::Bool("bool")});
  PvxsValueBuilder builder(pvxs_type);

  // building PVXS value
  builder.StructProlog(&any_value);
  builder.MemberProlog(&any_value["signed"], "signed");
  builder.ScalarProlog(&any_value["signed"]);
  builder.ScalarEpilog(&any_value["signed"]);
  builder.MemberEpilog(&any_value["signed"], "signed");
  builder.StructMemberSeparator();
  builder.MemberProlog(&any_value["bool"], "bool");
  builder.ScalarProlog(&any_value["bool"]);
  builder.ScalarEpilog(&any_value["bool"]);
  builder.MemberEpilog(&any_value["bool"], "bool");
  builder.StructEpilog(&any_value);

  auto pvxs_value = builder.GetPVXSValue();

  // validating pvxs_value
  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Struct);
  EXPECT_EQ(pvxs_value.nmembers(), 2);

  auto names = GetMemberNames(pvxs_value);
  EXPECT_EQ(names, std::vector<std::string>({"signed", "bool"}));

  EXPECT_EQ(pvxs_value["signed"].type(), ::pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value["signed"].as<int32_t>(), 42);
  EXPECT_EQ(pvxs_value["bool"].type(), ::pvxs::TypeCode::Bool);
  EXPECT_EQ(pvxs_value["bool"].as<bool>(), true);
}

//! Build PVXS value from a structure with two fields inside parent structure.

TEST_F(PvxsValueBuilderTests, NestedStruct)
{
  // original any_value
  sup::dto::AnyValue two_scalars = {{"signed", {sup::dto::SignedInteger32Type, 42}},
                                    {"bool", {sup::dto::BooleanType, true}}};
  sup::dto::AnyValue any_value = {{"scalars", two_scalars}};

  // initialising builder
  auto pvxs_type = ::pvxs::TypeDef(::pvxs::TypeCode::Struct,
                              {pvxs::members::Struct("scalars", {pvxs::members::Int32("signed"),                                                                pvxs::members::Bool("bool")})});
  PvxsValueBuilder builder(pvxs_type);

  // building PVXS value
  builder.StructProlog(&any_value);
  builder.MemberProlog(&any_value["scalars"], "scalars");

  builder.StructProlog(&any_value["scalars"]);
  builder.MemberProlog(&any_value["scalars.signed"], "signed");
  builder.ScalarProlog(&any_value["scalars.signed"]);
  builder.ScalarEpilog(&any_value["scalars.signed"]);
  builder.MemberEpilog(&any_value["scalars.signed"], "signed");
  builder.StructMemberSeparator();
  builder.MemberProlog(&any_value["scalars.bool"], "bool");
  builder.ScalarProlog(&any_value["scalars.bool"]);
  builder.ScalarEpilog(&any_value["scalars.bool"]);
  builder.MemberEpilog(&any_value["scalars.bool"], "bool");
  builder.StructEpilog(&any_value["scalars"]);

  builder.MemberEpilog(&any_value["scalars"], "scalars");
  builder.StructEpilog(&any_value);

  auto pvxs_value = builder.GetPVXSValue();

  // validating pvxs_value
  EXPECT_EQ(pvxs_value.type(), ::pvxs::TypeCode::Struct);
  EXPECT_EQ(pvxs_value.nmembers(), 1);

  auto names = GetMemberNames(pvxs_value);
  EXPECT_EQ(names, std::vector<std::string>({"scalars"}));

  auto nested_value = pvxs_value["scalars"];
  EXPECT_EQ(nested_value.type(), ::pvxs::TypeCode::Struct);
  EXPECT_EQ(nested_value.nmembers(), 2);

  auto nested_names = GetMemberNames(nested_value);
  EXPECT_EQ(nested_names, std::vector<std::string>({"signed", "bool"}));

  EXPECT_EQ(pvxs_value["scalars.signed"].type(), ::pvxs::TypeCode::Int32);
  EXPECT_EQ(pvxs_value["scalars.bool"].type(), ::pvxs::TypeCode::Bool);

  EXPECT_EQ(pvxs_value["scalars.signed"].as<int32_t>(), 42);
  EXPECT_EQ(pvxs_value["scalars.bool"].as<bool>(), true);

}
