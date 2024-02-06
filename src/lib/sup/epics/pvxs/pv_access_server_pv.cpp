/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
 *
 * Author        : Gennady Pospelov (IO)
 *
 * Copyright (c) : 2010-2024 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 *****************************************************************************/

#include "sup/epics/pvxs/pv_access_server_pv.h"

#include <stdexcept>

namespace sup
{
namespace epics
{

PvAccessServerPV::PvAccessServerPV(const std::string& variable_name,
                                   const sup::dto::AnyValue& any_value,
                                   VariableChangedCallback callback)
  : m_variable_name(variable_name)
  , m_any_value(any_value)
  , m_pvxs_cache(BuildPVXSValue(m_any_value))
  , m_callback(std::move(callback))
  , m_shared_pv(pvxs::server::SharedPV::buildMailbox())
{
  if (sup::dto::IsScalarValue(any_value))
  {
    throw std::runtime_error("Error in PvAccessServerPV: cannot publish a scalar value");
  }
  using namespace std::placeholders;
  m_shared_pv.onPut(std::bind(&PvAccessServerPV::OnSharedValueChanged, this, _1, _2, _3));
}

PvAccessServerPV::~PvAccessServerPV() = default;

std::string PvAccessServerPV::GetVariableName() const
{
  return m_variable_name;
}

dto::AnyValue PvAccessServerPV::GetValue() const
{
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_any_value;
}

bool PvAccessServerPV::SetValue(const dto::AnyValue& value)
{
  if (sup::dto::IsScalarValue(value))
  {
    throw std::runtime_error("Error in PvAccessServerPV: cannot set a scalar value");
  }
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (sup::dto::IsEmptyValue(m_any_value))
    {
      m_any_value = value;
    }
    else
    {
      m_any_value.ConvertFrom(value);
    }
  }
  // assigning value to shared variable
  if (m_shared_pv.isOpen())
  {
    // Simple copy doesn't work. We have to keep m_pvxs_cache internal storage's pointer
    // alive since server::SharedPV relies on that.
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      auto pvxs_value = BuildPVXSValue(m_any_value);
      m_pvxs_cache.assign(pvxs_value);
    }
    m_shared_pv.post(m_pvxs_cache);
  }
  if (m_callback)  // for some reason `post` above doesn't trigger OnSharedValueChanged
  {
    m_callback(m_any_value);
  }
  return true;
}

void PvAccessServerPV::AddToServer(pvxs::server::Server& server)
{
  if (m_shared_pv.isOpen())
  {
    throw std::runtime_error("Variable was already added to a server");
  }
  server.addPV(m_variable_name, m_shared_pv);
  m_shared_pv.open(m_pvxs_cache);
}

void PvAccessServerPV::OnSharedValueChanged(pvxs::server::SharedPV& /*pv*/,
                                            std::unique_ptr<pvxs::server::ExecOp>&& op,
                                            pvxs::Value&& value)
{
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_any_value = BuildAnyValue(value);

    // Simple copy doesn't work. We have to keep m_pvxs_cache internal storage's pointer alive
    // since server::SharedPV relies on that.
    m_pvxs_cache.assign(value);
    m_shared_pv.post(m_pvxs_cache);
  }
  if (m_callback)
  {
    m_callback(m_any_value);
  }
  op->reply();
}

}  // namespace epics

}  // namespace sup
