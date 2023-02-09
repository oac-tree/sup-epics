/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : CODAC Supervision and Automation (SUP) Sequencer component
 *
 * Description   : UserInterface implementation
 *
 * Author        : B.Bauvir (IO)
 *
 * Copyright (c) : 2010-2020 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#include <sup/epics-test/softioc_runner.h>
#include <sup/epics-test/softioc_utils.h>

#include <gtest/gtest.h>

namespace
{

const std::string db_content = R"RAW(
record (bo,"CA-TESTS:BOOL")
{
    field(DESC,"Some EPICSv3 record")
    field(ONAM,"TRUE")
    field(OSV,"NO_ALARM")
    field(ZNAM,"FALSE")
    field(ZSV,"NO_ALARM")
    field(VAL,"0")
}

record (ao,"CA-TESTS:FLOAT")
{
    field(DESC,"Some EPICSv3 record")
    field(DRVH,"5.0")
    field(DRVL,"-5.0")
    field(VAL,"0")
}

record (stringout,"CA-TESTS:STRING")
{
    field(DESC,"Some EPICSv3 record")
    field(VAL,"undefined")
}

record (waveform,"CA-TESTS:CHARRAY")
{
    field(DESC,"Some EPICSv3 record")
    field(FTVL, "CHAR")
    field(NELM, "1024")
}

record (longout,"CA_TESTS:LONG")
{
    field(DESC,"Some EPICSv3 record")
    field(VAL,"0")
    field(PINI,"YES")
}
)RAW";

//! Returns string representing EPICS database file with several testing variables.
std::string GetEpicsDBContentString()
{
  return db_content;
}

}  // namespace

class IOCEnvironment : public ::testing::Environment
{
public:
  IOCEnvironment();
  ~IOCEnvironment() override;

  void SetUp() override;
  void TearDown() override;

  sup::epics::test::SoftIocRunner m_softioc_service;
};

::testing::Environment* const ioc_environment =
    ::testing::AddGlobalTestEnvironment(new IOCEnvironment);

IOCEnvironment::IOCEnvironment() : m_softioc_service{"ChannelAccessTests"} {}

IOCEnvironment::~IOCEnvironment() = default;

void IOCEnvironment::SetUp()
{
  m_softioc_service.Start(GetEpicsDBContentString());
}

void IOCEnvironment::TearDown()
{
  m_softioc_service.Stop();
}
