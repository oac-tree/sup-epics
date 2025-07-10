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

#include <sup/dto/anyvalue.h>
#include <sup/dto/any_functor.h>

class RPCSniffServer : public sup::dto::AnyFunctor
{
public:
  RPCSniffServer(const sup::dto::AnyValue& reply);
  ~RPCSniffServer() override;

  sup::dto::AnyValue operator()(const sup::dto::AnyValue& input) override;

private:
  sup::dto::AnyValue m_reply;
};
