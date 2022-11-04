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

#include "mock_utils.h"
#include "unit_test_helper.h"

#include <sup/epics/pv_access_rpc_client.h>
#include <sup/epics/pv_access_rpc_server.h>

#include <gtest/gtest.h>
#include <sup/dto/anyvalue.h>

#include <thread>

using ::testing::_;
using sup::epics::unit_test_helper::BusyWaitFor;

using namespace sup::epics;

//! Testing PvAccessRPCServer and PvAccessRPCClient together.

class PvAccessRPCTests : public ::testing::Test
{
public:
};

//! Standard scenario. Single server and single client.

TEST_F(PvAccessRPCTests, SingleServerSingleClient)
{
}

