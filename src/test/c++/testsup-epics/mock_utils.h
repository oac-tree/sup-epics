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

#ifndef SUP_EPICS_MOC_UTILS_H_
#define SUP_EPICS_MOC_UTILS_H_

//! @file mock_utils.h
//! Collection of mock classes to use with googlemock framework.

#include <gmock/gmock.h>
#include <sup/epics/pv_client_pv.h>
#include <sup/epics/pv_access_client.h>
#include <sup/epics/dto_types_fwd.h>

#include <functional>
#include <string>

//! Mock class to listen for callbacks.

class MockListener
{
public:
  sup::epics::PvClientPV::VariableChangedCallback GetCallBack()
  {
    return [this](const sup::epics::PvClientPV::ExtendedValue& value) { OnValueChanged(value); };
  }

  std::function<void(const sup::dto::AnyValue&)> GetCallBack_old()
  {
    return [this](const sup::dto::AnyValue& value) { OnValueChanged_old(value); };
  }

  sup::epics::PVAccessClient::VariableChangedCallback GetNamedCallBack()
  {
    return [this](const std::string& name, const sup::epics::PvClientPV::ExtendedValue& value)
    { OnNamedValueChanged(name, value); };
  }

  std::function<void(const std::string&, const sup::dto::AnyValue&)> GetNamedCallBack_old()
  {
    return [this](const std::string& name, const sup::dto::AnyValue& value)
    { OnNamedValueChanged_old(name, value); };
  }

  MOCK_METHOD1(OnValueChanged, void(const sup::epics::PvClientPV::ExtendedValue& value));
  MOCK_METHOD1(OnValueChanged_old, void(const sup::dto::AnyValue& value));
  MOCK_METHOD2(OnNamedValueChanged,
               void(const std::string& name, const sup::epics::PvClientPV::ExtendedValue& value));
  MOCK_METHOD2(OnNamedValueChanged_old,
               void(const std::string& name, const sup::dto::AnyValue& value));
};

#endif  // SUP_EPICS_MOC_UTILS_H_
