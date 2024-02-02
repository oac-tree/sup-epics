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

#ifndef SUP_EPICS_EPICS_PROTOCOL_FACTORY_H_
#define SUP_EPICS_EPICS_PROTOCOL_FACTORY_H_

#include <sup/protocol/protocol_factory.h>

namespace sup
{
namespace epics
{

class EPICSProtocolFactory : public sup::protocol::ProtocolFactory
{
public:
  ~EPICSProtocolFactory();
};

}  // namespace epics

}  // namespace sup

#endif  // SUP_EPICS_EPICS_PROTOCOL_FACTORY_H_
