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

#ifndef SPU_EPICS_PV_ACCESS_SERVER_H_
#define SPU_EPICS_PV_ACCESS_SERVER_H_

#include <sup/epics/dto_types_fwd.h>

namespace sup
{
namespace epics
{

//! Represents a server to run multiple pvAccess variables.

class PVAccessServer
{
public:
  explicit PVAccessServer();
  ~PVAccessServer();

  PVAccessServer(const PVAccessServer&) = delete;
  PVAccessServer& operator=(const PVAccessServer&) = delete;
  PVAccessServer(PVAccessServer&&) = delete;
  PVAccessServer& operator=(PVAccessServer&&) = delete;

private:
  struct PVAccessServerImpl;
  PVAccessServerImpl* p_impl{nullptr};
};

}  // namespace epics

}  // namespace sup

#endif  // SPU_EPICS_PV_ACCESS_SERVER_H_
