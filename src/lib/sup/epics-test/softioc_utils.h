/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Walter Van Herck (IO)
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

#ifndef SUP_EPICS_TEST_SOFTIOC_UTILS_H_
#define SUP_EPICS_TEST_SOFTIOC_UTILS_H_

#include <string>
#include <vector>

namespace sup
{
namespace epics
{
namespace test
{

//! Returns path where EPICS binaries are located.
std::string GetEPICSBinaryPath();

//! Validates if given EPICS executable is present. Throws if it is not the case.
void ValidateEPICSExecutable(const std::string& exec_name);

//! Removes file with given name from disk.
void RemoveFile(const std::string& file_name);

//! Returns string representing PVGet output for given variable names.
std::string GetPvGetOutput(const std::vector<std::string>& variable_names,
                           const std::string& file_name = {});

//! Returns string representing `pvget` output for given variable name.
std::string GetPvGetOutput(const std::string& variable_name, const std::string& file_name = {});

//! Put the value into PV with `pvput` command, returns string representing command output.
std::string PvPut(const std::string& variable_name, const std::string& value);

//! Put the value into PV with `caput` command, returns string representing command output.
std::string CAPut(const std::string& variable_name, const std::string& value);

}  // namespace test
}  // namespace epics
}  // namespace sup

#endif  // SUP_EPICS_TEST_SOFTIOC_UTILS_H_
