/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : Pulse Counter Service
 *
 * Description   : Pulse Counter Service of Supervision and Automation System
 *
 * Author        : Gennady Pospelov
 *
 * Copyright (c) : 2010-2025 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 * SPDX-License-Identifier: MIT
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file LICENSE located in the top level directory
 * of the distribution package.
 *****************************************************************************/

#include "softioc_runner.h"

#include "softioc_utils.h"

#include <fstream>
#include <stdexcept>

namespace
{
//! Generates Epic DB file on disk with given name in current working directory.
void GenerateEpicDatabaseFile(const std::string& name, const std::string& db_file_content)
{
  std::ofstream result(name);
  if (!(result << db_file_content))
  {
    const std::string error =
      "Error in SoftIocRunner::GenerateEpicDatabaseFile(): db file not created";
    throw std::runtime_error(error);
  }
  result.close();
}

}  // unnamed namespace

namespace sup
{
namespace epics
{
namespace test
{

SoftIocRunner::SoftIocRunner(const std::string& session_name) : m_is_active(false)
{
  m_session_name = session_name.empty() ? "temp-softioc" : session_name;
}

SoftIocRunner::~SoftIocRunner()
{
  if (m_is_active)
  {
    Stop();
  }
}

void SoftIocRunner::Start(const std::string& db_file_content)
{
  ValidateShellExecutable("/usr/bin/screen");
  ValidateEPICSExecutable("softIoc");

  if (m_is_active)
  {
    throw std::runtime_error("Error in SoftIocRunner::Start(): thread is already running");
  }

  GenerateEpicDatabaseFile(GetDataBaseFileName(), db_file_content);

  const std::string command{"/usr/bin/screen -d -m -S " + m_session_name + " "
                            + GetEPICSExecutablePath("softIoc") + " -d " + GetDataBaseFileName()
                            + " &> /dev/null"};
  std::system(command.c_str());
  m_is_active = true;
}

void SoftIocRunner::Stop()
{
  if (!m_is_active)
  {
    throw std::runtime_error("Error in SoftIocRunner::Stop(): was not started");
  }

  const std::string command{"/usr/bin/screen -S " + m_session_name + " -X quit &> /dev/null"};
  std::system(command.c_str());
  m_is_active = false;

  RemoveFile(GetDataBaseFileName());
}

bool SoftIocRunner::IsActive() const
{
  return m_is_active;
}

std::string SoftIocRunner::GetDataBaseFileName() const
{
  return m_session_name + ".db";
}

}  // namespace test
}  // namespace epics
}  // namespace sup
