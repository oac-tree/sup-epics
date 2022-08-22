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

#ifndef SPU_EPICS_PVXS_PV_ACCESS_CLIENT_H_
#define SPU_EPICS_PVXS_PV_ACCESS_CLIENT_H_

namespace sup
{
namespace epics
{
//! Represents a client to access/update multiple pvAccess variables.

class PVAccessClient
{
public:
  explicit PVAccessClient();
  ~PVAccessClient();

  PVAccessClient(const PVAccessClient&) = delete;
  PVAccessClient& operator=(const PVAccessClient&) = delete;
  PVAccessClient(PVAccessClient&&) = delete;
  PVAccessClient& operator=(PVAccessClient&&) = delete;

private:
  struct PVAccessClientImpl;
  PVAccessClientImpl* p_impl{nullptr};
};

}  // namespace epics

}  // namespace sup

#endif  // SPU_EPICS_PVXS_PV_ACCESS_CLIENT_H_
