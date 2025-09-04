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

#include <sup/epics/epics_protocol_factory.h>
#include <sup/epics/factory/epics_protocol_factory_utils.h>

#include <sup/dto/anyvalue.h>
#include <sup/protocol/exceptions.h>

#include <gtest/gtest.h>

using namespace sup::epics;

class EPICSProtocolFactoryUtilsTest : public ::testing::Test
{
protected:
  EPICSProtocolFactoryUtilsTest() = default;
  ~EPICSProtocolFactoryUtilsTest() override = default;
};

TEST_F(EPICSProtocolFactoryUtilsTest, ParsePvAccessRPCClientConfig)
{
  {
    // No service field throws
    const sup::dto::AnyValue config = {{
      { kTimeout, 2.0 }
    }};
    EXPECT_THROW(utils::ParsePvAccessRPCClientConfig(config),
                 sup::protocol::InvalidOperationException);
  }
  {
    // Wrong type of service field throws
    const sup::dto::AnyValue config = {{
      { kServiceName, 42 },
      { kTimeout, 2.0 }
    }};
    EXPECT_THROW(utils::ParsePvAccessRPCClientConfig(config),
                 sup::protocol::InvalidOperationException);
  }
  {
    // Only service field
    const sup::dto::AnyValue config = {{
      { kServiceName, { sup::dto::StringType, "MyServiceName"} }
    }};
    auto client_config = utils::ParsePvAccessRPCClientConfig(config);
    EXPECT_EQ(client_config.service_name, "MyServiceName");
    EXPECT_EQ(client_config.timeout, 5.0); // default timeout
  }
  {
    // Wrong type of timeout field throws
    const sup::dto::AnyValue config = {{
      { kServiceName, { sup::dto::StringType, "MyServiceName"} },
      { kTimeout, 42 }
    }};
    EXPECT_THROW(utils::ParsePvAccessRPCClientConfig(config),
                 sup::protocol::InvalidOperationException);
  }
  {
    // Negative timeout field throws
    const sup::dto::AnyValue config = {{
      { kServiceName, { sup::dto::StringType, "MyServiceName"} },
      { kTimeout, { sup::dto::Float64Type, -3.14 } }
    }};
    EXPECT_THROW(utils::ParsePvAccessRPCClientConfig(config),
                 sup::protocol::InvalidOperationException);
  }
  {
    // Correct timeout field is taken into account
    const sup::dto::AnyValue config = {{
      { kServiceName, { sup::dto::StringType, "MyServiceName"} },
      { kTimeout, { sup::dto::Float64Type, 2.0 } }
    }};
    auto client_config = utils::ParsePvAccessRPCClientConfig(config);
    EXPECT_EQ(client_config.service_name, "MyServiceName");
    EXPECT_EQ(client_config.timeout, 2.0);
  }
}

TEST_F(EPICSProtocolFactoryUtilsTest, CreateChannelAccessClientVar)
{
  {
    // No channel field throws
    const sup::dto::AnyValue config = {{
      { kVariableType, "DoesntMatter" }
    }};
    EXPECT_THROW(utils::CreateChannelAccessClientVar(config),
                 sup::protocol::InvalidOperationException);
  }
  {
    // Wrong channel field throws
    const sup::dto::AnyValue config = {{
      { kChannelName, 42 },
      { kVariableType, "DoesntMatter" }
    }};
    EXPECT_THROW(utils::CreateChannelAccessClientVar(config),
                 sup::protocol::InvalidOperationException);
  }
  {
    // No variable type field throws
    const sup::dto::AnyValue config = {{
      { kChannelName, "MyChannel" }
    }};
    EXPECT_THROW(utils::CreateChannelAccessClientVar(config),
                 sup::protocol::InvalidOperationException);
  }
  {
    // Wrong variable type field throws
    const sup::dto::AnyValue config = {{
      { kChannelName, "MyChannel" },
      { kVariableType, 42 }
    }};
    EXPECT_THROW(utils::CreateChannelAccessClientVar(config),
                 sup::protocol::InvalidOperationException);
  }
  {
    // Variable type field that cannot be parsed throws
    const sup::dto::AnyValue config = {{
      { kChannelName, "MyChannel" },
      { kVariableType, "NotParseable" }
    }};
    EXPECT_THROW(utils::CreateChannelAccessClientVar(config),
                 sup::protocol::InvalidOperationException);
  }
  {
    // Correct configuration
    const sup::dto::AnyValue config = {{
      { kChannelName, "MyChannel" },
      { kVariableType, R"RAW({"type":"float64"})RAW" }
    }};
    EXPECT_NO_THROW(utils::CreateChannelAccessClientVar(config));
  }
}

TEST_F(EPICSProtocolFactoryUtilsTest, CreatePvAccessClientVar)
{
  {
    // No channel field throws
    const sup::dto::AnyValue config = sup::dto::EmptyStruct();
    EXPECT_THROW(utils::CreatePvAccessClientVar(config),
                 sup::protocol::InvalidOperationException);
  }
  {
    // Wrong channel field throws
    const sup::dto::AnyValue config = {{
      { kChannelName, 42 }
    }};
    EXPECT_THROW(utils::CreatePvAccessClientVar(config),
                 sup::protocol::InvalidOperationException);
  }
  {
    // Correct configuration
    const sup::dto::AnyValue config = {{
      { kChannelName, "MyChannel" }
    }};
    EXPECT_NO_THROW(utils::CreatePvAccessClientVar(config));
  }
}

TEST_F(EPICSProtocolFactoryUtilsTest, CreatePvAccessServerVar)
{
  {
    // No channel field throws
    const sup::dto::AnyValue config = {{
      { kVariableValue, {{
        { "ID", 42 }
      }}}
    }};
    EXPECT_THROW(utils::CreatePvAccessServerVar(config),
                 sup::protocol::InvalidOperationException);
  }
  {
    // Wrong channel field throws
    const sup::dto::AnyValue config = {{
      { kChannelName, 42 },
      { kVariableValue, {{
        { "ID", 42 }
      }}}
    }};
    EXPECT_THROW(utils::CreatePvAccessServerVar(config),
                 sup::protocol::InvalidOperationException);
  }
  {
    // No variable value field throws
    const sup::dto::AnyValue config = {{
      { kChannelName, "MyChannel" }
    }};
    EXPECT_THROW(utils::CreatePvAccessServerVar(config),
                 sup::protocol::InvalidOperationException);
  }
  {
    // Correct configuration
    const sup::dto::AnyValue config = {{
      { kChannelName, "MyChannel" },
      { kVariableValue, {{
        { "ID", 42 }
      }}}
    }};
    EXPECT_NO_THROW(utils::CreatePvAccessServerVar(config));
  }
}
