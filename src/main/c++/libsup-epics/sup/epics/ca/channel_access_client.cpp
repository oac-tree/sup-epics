/******************************************************************************
 *
 * Project       : Supervision and automation system EPICS interface
 *
 * Description   : Library of SUP components for EPICS network protocol
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

#include <sup/epics/channel_access_client.h>

namespace sup
{
namespace epics
{

ChannelAccessClient::ChannelAccessClient() = default;
ChannelAccessClient::~ChannelAccessClient() = default;

bool ChannelAccessClient::AddVariable(const std::string& name, const sup::dto::AnyType& type)
{}

bool ChannelAccessClient::IsConnected(const std::string& name) const
{}

sup::dto::AnyValue ChannelAccessClient::GetValue(const std::string& name) const
{}

ChannelAccessPV::ExtendedValue ChannelAccessClient::GetExtendedValue(const std::string& name) const
{}

bool ChannelAccessClient::SetValue(const std::string& name, const sup::dto::AnyValue& value)
{}

bool ChannelAccessClient::WaitForConnected(const std::string& name, double timeout_sec) const
{}

bool ChannelAccessClient::RemoveVariable(const std::string& name)
{}

}  // namespace epics

}  // namespace sup
