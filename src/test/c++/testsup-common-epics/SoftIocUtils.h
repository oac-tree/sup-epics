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

#ifndef SUP_EPICS_SoftIocUtils_H
#define SUP_EPICS_SoftIocUtils_H

#include <string>

//! Returns string representing EPICS database file with several testing variables.
std::string GetEpicsDBContentString();

//! Removes file with given name from disk.
void RemoveFile(const std::string& file_name);

#endif  // SUP_EPICS_SoftIocUtils_H
