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

using namespace ::sup::epics;

class PvxsTypeBuilderTest : public ::testing::Test
{
public:
 static  pvxs::TypeDef GetPVXSType(const sup::dto::AnyType& any_type)
  {
    PvxsTypeBuilder builder;
    sup::dto::SerializeAnyType(any_type, builder);
    return builder.GetPVXSType();
  }
};

TEST_F(PvxsTypeBuilderTest, PVXSTypeDefBasics)
{
  auto pvxs_type = pvxs::TypeDef(pvxs::TypeCode::Int32);
}
