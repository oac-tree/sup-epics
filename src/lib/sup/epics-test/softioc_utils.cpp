/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Walter Van Herck (IO)
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

#include "softioc_utils.h"

#include <algorithm>
#include <fstream>
#include <random>
#include <sstream>
#include <stdexcept>

namespace
{

//! Generates random file name for temp directory.
std::string GetTempFileName()
{
  std::random_device rndm_device;
  std::mt19937 gen(rndm_device());
  std::string alphabet = "abc123";
  std::shuffle(std::begin(alphabet), std::end(alphabet), gen);
  return "/tmp/sup_epics_tests_" + alphabet + ".out";
}

}  // unnamed namespace

namespace sup
{
namespace epics
{
namespace test
{

std::string GetEPICSExecutablePath(const std::string &exec_name)
{
  return std::string(std::getenv("EPICS_BASE")) + "/bin/"
         + std::string(std::getenv("EPICS_HOST_ARCH")) + "/" + exec_name;
}

void ValidateShellExecutable(const std::string &exec_name)
{
  std::string command("which " + exec_name + " > /dev/null 2>&1");
  if (std::system(command.c_str()))
  {
    throw std::runtime_error("Can't find executable '" + exec_name + "'");
  }
}

void ValidateEPICSExecutable(const std::string &exec_name)
{
  ValidateShellExecutable(sup::epics::test::GetEPICSExecutablePath(exec_name));
}

void RemoveFile(const std::string &file_name)
{
  std::remove(file_name.c_str());
}

std::string GetPvGetOutput(const std::vector<std::string> &variable_names,
                           const std::string &file_name)
{
  std::string out_name = file_name.empty() ? GetTempFileName() : file_name;

  std::string variable_names_str;
  for (const auto &str : variable_names)
  {
    variable_names_str += str + " ";
  }

  std::string command(GetEPICSExecutablePath("pvget") + " -v " + variable_names_str + " > "
                      + out_name);
  if (std::system(command.c_str()) != 0)
  {
    return {};
  }

  std::stringstream sstr;
  sstr << std::ifstream(out_name).rdbuf();

  RemoveFile(out_name);

  return sstr.str();
}

std::string GetPvGetOutput(const std::string &variable_name, const std::string &file_name)
{
  return GetPvGetOutput(std::vector<std::string>({variable_name}), file_name);
}

std::string PvPut(const std::string &variable_name, const std::string &value)
{
  auto out_file_name = GetTempFileName();

  std::string command(GetEPICSExecutablePath("pvput") + " " + variable_name + " " + value + " > "
                      + out_file_name);
  if (std::system(command.c_str()) != 0)
  {
    return {};
  }

  std::stringstream sstr;
  sstr << std::ifstream(out_file_name).rdbuf();

  RemoveFile(out_file_name);

  return sstr.str();
}

std::string CAPut(const std::string &variable_name, const std::string &value)
{
  auto out_file_name = GetTempFileName();

  std::string command(GetEPICSExecutablePath("caput") + " " + variable_name + " " + value + " > "
                      + out_file_name);
  if (std::system(command.c_str()) != 0)
  {
    return {};
  }

  std::stringstream sstr;
  sstr << std::ifstream(out_file_name).rdbuf();

  RemoveFile(out_file_name);

  return sstr.str();
}

}  // namespace test
}  // namespace epics
}  // namespace sup
