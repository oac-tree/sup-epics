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

#include <sup/epics/channel_access_pv.h>

#include <sup/dto/anytype.h>
#include <sup/dto/anyvalue.h>

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
    field(PINI,"YES")
}

record (ao,"CA-TESTS:FLOAT")
{
    field(DESC,"Some EPICSv3 record")
    field(DRVH,"5.0")
    field(DRVL,"-5.0")
    field(VAL,"0")
    field(PINI,"YES")
}

record (stringout,"CA-TESTS:STRING")
{
    field(DESC,"Some EPICSv3 record")
    field(VAL,"undefined")
    field(PINI,"YES")
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

record (int64out,"CA_TESTS:INT64")
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
  sup::dto::AnyType char_array_t(1024, sup::dto::Character8Type, "char8[]");
  sup::dto::AnyValue char_array_v{char_array_t};
  sup::epics::ChannelAccessPV ca_chararray_var("CA-TESTS:CHARRAY", char_array_t);
  ca_chararray_var.WaitForConnected(2.0);
  ca_chararray_var.SetValue(char_array_v);
  ca_chararray_var.WaitForValidValue(2.0);
}

void IOCEnvironment::TearDown()
{
  m_softioc_service.Stop();
}
