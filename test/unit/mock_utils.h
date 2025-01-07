/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Gennady Pospelov (IO)
 *
 * Copyright (c) : 2010-2025 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 *****************************************************************************/

#ifndef SUP_EPICS_MOC_UTILS_H_
#define SUP_EPICS_MOC_UTILS_H_

//! @file mock_utils.h
//! Collection of mock classes to use with googlemock framework.

#include <gmock/gmock.h>
#include <sup/epics/pv_access_client_pv.h>
#include <sup/epics/pv_access_client.h>
#include <sup/epics/pv_access_server.h>
#include <sup/epics/utils/dto_types_fwd.h>

#include <functional>
#include <string>

//! Mock class to listen for callbacks.

class MockListener
{
public:
  sup::epics::PvAccessClientPV::VariableChangedCallback GetClientPVCallBack()
  {
    return [this](const sup::epics::PvAccessClientPV::ExtendedValue& value) { OnClientPVValueChanged(value); };
  }

  std::function<void(const sup::dto::AnyValue&)> GetServerPvCallBack()
  {
    return [this](const sup::dto::AnyValue& value) { OnServerPVValueChanged(value); };
  }

  sup::epics::PvAccessClient::VariableChangedCallback GetClientCallBack()
  {
    return [this](const std::string& name, const sup::epics::PvAccessClientPV::ExtendedValue& value)
    { OnClientValueChanged(name, value); };
  }

  sup::epics::PvAccessServer::VariableChangedCallback GetServerCallBack()
  {
    return [this](const std::string& name, const sup::dto::AnyValue& value)
    { OnServerValueChanged(name, value); };
  }

  MOCK_METHOD1(OnClientPVValueChanged, void(const sup::epics::PvAccessClientPV::ExtendedValue& value));
  MOCK_METHOD1(OnServerPVValueChanged, void(const sup::dto::AnyValue& value));
  MOCK_METHOD2(OnClientValueChanged,
               void(const std::string& name, const sup::epics::PvAccessClientPV::ExtendedValue& value));
  MOCK_METHOD2(OnServerValueChanged,
               void(const std::string& name, const sup::dto::AnyValue& value));
};

#endif  // SUP_EPICS_MOC_UTILS_H_
