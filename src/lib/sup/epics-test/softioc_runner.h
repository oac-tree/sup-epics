/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : CODAC Core System
 *
 * Description   : EPICS 7 C++ wrapper classes
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

#ifndef SUP_EPICS_TEST_SOFTIOC_RUNNER_H_
#define SUP_EPICS_TEST_SOFTIOC_RUNNER_H_

#include <string>

namespace sup
{
namespace epics
{
namespace test
{

//! Provides possibility to start/stop 'softIoc' service on request.
//! Intended for use with google-test and its SetUpTestCase/TearDownTestCase machinery.
//! The goal is to isolate the user from startup/shutdown details of the softIoc service
//! and provide a guarantee, that service will be available between Start/Stop calls.
class SoftIocRunner
{
public:
  SoftIocRunner(const std::string& session_name = {});
  ~SoftIocRunner();

  //! Starts the service.
  void Start(const std::string& db_file_content);

  //! Stops the service.
  void Stop();

  //! Returns true if SoftIoc service is active and running.
  bool IsActive() const;

  //! Returns the name of the EPICS database file name
  std::string GetDataBaseFileName() const;

protected:
  bool m_is_active;
  std::string m_session_name;
};

}  // namespace test
}  // namespace epics
}  // namespace sup

#endif  // SUP_EPICS_TEST_SOFTIOC_RUNNER_H_
