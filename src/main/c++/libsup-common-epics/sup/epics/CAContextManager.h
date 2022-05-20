/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP common components for EPICS network protocol
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

#ifndef SUP_EPICS_CACONTEXTMANAGER_H
#define SUP_EPICS_CACONTEXTMANAGER_H

#include <functional>

namespace sup::epics
{

//! Interface of a generic client variable.

class CAContextManager
{
public:
  CAContextManager();
  ~CAContextManager();

};

}  // namespace sup::epics

#endif  // SUP_EPICS_CACONTEXTMANAGER_H
