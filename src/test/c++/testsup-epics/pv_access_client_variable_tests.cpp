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

#include <gtest/gtest.h>
#include <pvxs/client.h>
#include <pvxs/data.h>
#include <pvxs/nt.h>
#include <pvxs/server.h>
#include <pvxs/sharedpv.h>
#include <sup/epics/channel_access_pv.h>

namespace
{
const int kInitialValue = 42;
const std::string kChannelName = "PVXS-TESTS:NTSCALAR";
}

class PVAccessClientVariableTest : public ::testing::Test
{
public:
  PVAccessClientVariableTest()
      : m_pvxs_value(pvxs::nt::NTScalar{pvxs::TypeCode::Int32}.create())
      , m_shared_pv(pvxs::server::SharedPV::buildMailbox())
      , m_server(pvxs::server::Config::isolated().build().addPV(kChannelName, m_shared_pv))
  {
    m_pvxs_value["value"] = kInitialValue;
  }

  pvxs::Value m_pvxs_value;
  pvxs::server::SharedPV m_shared_pv;
  pvxs::server::Server m_server;
};

TEST_F(PVAccessClientVariableTest, InitialState)
{
  m_server.start();
  m_shared_pv.open(m_pvxs_value);
  EXPECT_EQ(1, 1);
}

TEST_F(PVAccessClientVariableTest, InitialStateV2)
{
  m_server.start();
  m_shared_pv.open(m_pvxs_value);
  EXPECT_EQ(1, 1);
}
