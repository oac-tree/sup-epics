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
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 *****************************************************************************/

#include <sup/epics/epics_protocol_factory.h>
#include <sup/epics/pv_access_rpc_client.h>
#include <sup/epics/pv_access_rpc_server.h>

#include <sup/dto/anyvalue.h>
#include <sup/protocol/protocol_factory_utils.h>

#include <gtest/gtest.h>

#include <future>

using namespace sup::epics;

class TestProtocol : public sup::protocol::Protocol
{
public:
  TestProtocol() = default;
  ~TestProtocol() = default;

  void SetReply(const sup::dto::AnyValue& value,
                sup::protocol::ProtocolResult result = sup::protocol::Success)
  {
    m_reply = value;
    m_result = result;
  }

  sup::dto::AnyValue GetRequest() const { return m_request; }

  sup::protocol::ProtocolResult Invoke(const sup::dto::AnyValue& input, sup::dto::AnyValue& output)
  {
    m_request = input;
    output = m_reply;
    return m_result;
  }

  sup::protocol::ProtocolResult Service(const sup::dto::AnyValue& input, sup::dto::AnyValue& output)
  {
    m_request = input;
    output = m_reply;
    return m_result;
  }
private:
  sup::dto::AnyValue m_request;
  sup::dto::AnyValue m_reply;
  sup::protocol::ProtocolResult m_result;
};

class EPICSProtocolFactoryTest : public ::testing::Test
{
protected:
  EPICSProtocolFactoryTest() = default;
  ~EPICSProtocolFactoryTest() = default;

  EPICSProtocolFactory m_factory;
};

TEST_F(EPICSProtocolFactoryTest, ChannelAccessPVWrapper)
{
  sup::dto::AnyValue config = {{
    { kProcessVariableClass, kChannelAccessClientClass },
    { kChannelName, "CA-TESTS:BOOL" },
    { kVariableType, R"RAW({"type":"bool"})RAW" }
  }};
  auto var = m_factory.CreateProcessVariable(config);
  EXPECT_TRUE(var->WaitForAvailable(2.0));
  auto info = var->GetValue(0.0);
  EXPECT_TRUE(info.first);
  EXPECT_EQ(info.second.GetType(), sup::dto::BooleanType);
}

TEST_F(EPICSProtocolFactoryTest, TwoChannelAccessPVWrappers)
{
  sup::dto::AnyValue config = {{
    { kProcessVariableClass, kChannelAccessClientClass },
    { kChannelName, "CA-TESTS:FLOAT" },
    { kVariableType, R"RAW({"type":"float64"})RAW" }
  }};
  auto var_1 = m_factory.CreateProcessVariable(config);
  auto var_2 = m_factory.CreateProcessVariable(config);
  EXPECT_TRUE(var_1->WaitForAvailable(2.0));
  EXPECT_TRUE(var_1->IsAvailable());
  EXPECT_TRUE(var_2->WaitForAvailable(2.0));
  EXPECT_TRUE(var_2->IsAvailable());
  sup::dto::AnyValue update{ sup::dto::Float64Type, 1.1 };
  EXPECT_TRUE(SetVariableValue(*var_1, update));
  EXPECT_TRUE(WaitForVariableValue(*var_1, update, 1.0));
  EXPECT_TRUE(WaitForVariableValue(*var_2, update, 1.0));

  // Add callback to var_2
  double cache = 0.0;
  std::promise<void> promise;
  auto future = promise.get_future();
  auto callback = [&cache, &promise](const sup::dto::AnyValue& val, bool connected){
    if (connected && val.GetType() == sup::dto::Float64Type)
    {
      cache = val.As<sup::dto::float64>();
      promise.set_value();
    }
  };
  EXPECT_TRUE(var_2->SetMonitorCallback(callback));
  sup::dto::float64 float64_update = -2.0;
  EXPECT_TRUE(SetVariableValue(*var_1, float64_update));
  EXPECT_TRUE(WaitForVariableValue(*var_1, float64_update, 1.0));
  future.wait();
  EXPECT_EQ(cache, float64_update);
}

TEST_F(EPICSProtocolFactoryTest, PvAccessPVWrappers)
{
  std::string channel_name = "PVWrapperTest::SimpleStruct";

  // Create server variable
  sup::dto::AnyValue val_init = {{
    { "setpoint", { sup::dto::Float64Type, 4.0 }},
    { "enabled", { sup::dto::BooleanType, false }}
  }};
  sup::dto::AnyValue server_config = {{
    { kProcessVariableClass, kPvAccessServerClass },
    { kChannelName, channel_name },
    { kVariableValue, val_init }
  }};
  auto server_var = m_factory.CreateProcessVariable(server_config);
  EXPECT_TRUE(server_var->WaitForAvailable(2.0));
  EXPECT_TRUE(server_var->IsAvailable());
  auto info = server_var->GetValue(0.0);
  EXPECT_TRUE(info.first);
  EXPECT_EQ(info.second, val_init);

  // Create two client variables
  sup::dto::AnyValue client_config = {{
    { kProcessVariableClass, kPvAccessClientClass },
    { kChannelName, channel_name }
  }};
  auto client_var_1 = m_factory.CreateProcessVariable(client_config);
  EXPECT_TRUE(WaitForVariableValue(*client_var_1, val_init, 2.0));
  EXPECT_TRUE(client_var_1->IsAvailable());
  info = client_var_1->GetValue(0.0);
  EXPECT_TRUE(info.first);
  EXPECT_EQ(info.second, val_init);

  auto client_var_2 = m_factory.CreateProcessVariable(client_config);
  EXPECT_TRUE(WaitForVariableValue(*client_var_2, val_init, 2.0));
  EXPECT_TRUE(client_var_2->IsAvailable());
  info = client_var_2->GetValue(0.0);
  EXPECT_TRUE(info.first);
  EXPECT_EQ(info.second, val_init);

  // Update value through first client and observe other client and server
  val_init["setpoint"] = 6.2;
  val_init["enabled"] = true;
  EXPECT_TRUE(SetVariableValue(*client_var_1, val_init));
  EXPECT_TRUE(WaitForVariableValue(*client_var_2, val_init, 2.0));
  EXPECT_TRUE(WaitForVariableValue(*server_var, val_init, 2.0));

  // Update value through server and observe two clients
  val_init["setpoint"] = 3.14;
  val_init["enabled"] = false;
  EXPECT_TRUE(SetVariableValue(*server_var, val_init));
  EXPECT_TRUE(WaitForVariableValue(*client_var_1, val_init, 2.0));
  EXPECT_TRUE(WaitForVariableValue(*client_var_2, val_init, 2.0));
}

TEST_F(EPICSProtocolFactoryTest, TwoChannelAccessPVWrapperServerCallback)
{
  std::string channel_name = "PVWrapperTest::ServerCallback";

  // Create server variable
  sup::dto::AnyValue val_init = {{
    { "setpoint", { sup::dto::Float64Type, 4.0 }},
    { "enabled", { sup::dto::BooleanType, false }}
  }};
  sup::dto::AnyValue server_config = {{
    { kProcessVariableClass, kPvAccessServerClass },
    { kChannelName, channel_name },
    { kVariableValue, val_init }
  }};
  auto server_var = m_factory.CreateProcessVariable(server_config);
  EXPECT_TRUE(server_var->WaitForAvailable(2.0));
  EXPECT_TRUE(server_var->IsAvailable());
  auto info = server_var->GetValue(0.0);
  EXPECT_TRUE(info.first);
  EXPECT_EQ(info.second, val_init);

  // Create client variable
  sup::dto::AnyValue client_config = {{
    { kProcessVariableClass, kPvAccessClientClass },
    { kChannelName, channel_name }
  }};
  auto client_var = m_factory.CreateProcessVariable(client_config);
  EXPECT_TRUE(WaitForVariableValue(*client_var, val_init, 2.0));
  EXPECT_TRUE(client_var->IsAvailable());
  info = client_var->GetValue(0.0);
  EXPECT_TRUE(info.first);
  EXPECT_EQ(info.second, val_init);

  // Add callback to server variable
  sup::dto::AnyValue server_cache = val_init;
  std::promise<void> server_promise;
  auto server_future = server_promise.get_future();
  auto server_callback =
    [&server_cache, &server_promise](const sup::dto::AnyValue& val, bool connected){
      if (connected)
      {
        server_cache = val;
        server_promise.set_value();
      }
    };
  EXPECT_TRUE(server_var->SetMonitorCallback(server_callback));

  // Update value through client and observe server callback
  val_init["setpoint"] = 6.2;
  val_init["enabled"] = true;
  EXPECT_TRUE(SetVariableValue(*client_var, val_init));
  server_future.wait();
  EXPECT_EQ(server_cache, val_init);
}

TEST_F(EPICSProtocolFactoryTest, TwoChannelAccessPVWrapperClientCallback)
{
  std::string channel_name = "PVWrapperTest::ClientCallback";

  // Create server variable
  sup::dto::AnyValue val_init = {{
    { "setpoint", { sup::dto::Float64Type, 4.0 }},
    { "enabled", { sup::dto::BooleanType, false }}
  }};
  sup::dto::AnyValue server_config = {{
    { kProcessVariableClass, kPvAccessServerClass },
    { kChannelName, channel_name },
    { kVariableValue, val_init }
  }};
  auto server_var = m_factory.CreateProcessVariable(server_config);
  EXPECT_TRUE(server_var->WaitForAvailable(2.0));
  EXPECT_TRUE(server_var->IsAvailable());
  auto info = server_var->GetValue(0.0);
  EXPECT_TRUE(info.first);
  EXPECT_EQ(info.second, val_init);

  // Create client variable
  sup::dto::AnyValue client_config = {{
    { kProcessVariableClass, kPvAccessClientClass },
    { kChannelName, channel_name }
  }};
  auto client_var = m_factory.CreateProcessVariable(client_config);
  EXPECT_TRUE(WaitForVariableValue(*client_var, val_init, 2.0));
  EXPECT_TRUE(client_var->IsAvailable());
  info = client_var->GetValue(0.0);
  EXPECT_TRUE(info.first);
  EXPECT_EQ(info.second, val_init);

  // Add callback to client variable
  sup::dto::AnyValue client_cache = val_init;
  std::promise<void> client_promise;
  auto client_future = client_promise.get_future();
  auto client_callback =
    [&client_cache, &client_promise](const sup::dto::AnyValue& val, bool connected){
      if (connected)
      {
        client_cache = val;
        client_promise.set_value();
      }
    };
  EXPECT_TRUE(client_var->SetMonitorCallback(client_callback));

  // Update value through client and observe server callback
  val_init["setpoint"] = 6.2;
  val_init["enabled"] = true;
  EXPECT_TRUE(SetVariableValue(*server_var, val_init));
  client_future.wait();
  EXPECT_EQ(client_cache, val_init);
}

TEST_F(EPICSProtocolFactoryTest, RPCFactory)
{
  EPICSProtocolFactory factory;
  TestProtocol test_protocol;
  const std::string service_name = "EPICSRPCFactory::TestServer";

  // Create RPC server stack
  sup::dto::AnyValue server_def = {{
    { kServiceName, service_name }
  }};
  auto server = factory.CreateRPCServer(test_protocol, server_def);

  // Create corresponding RPC client without encoding
  sup::dto::AnyValue client_def_1 = {{
    { kServiceName, service_name },
    { sup::protocol::kEncoding, sup::protocol::kEncoding_None }
  }};
  auto client_1 = factory.CreateRPCClient(client_def_1);

  // Create corresponding RPC client with base64 encoding
  sup::dto::AnyValue client_def_2 = {{
    { kServiceName, service_name },
    { sup::protocol::kEncoding, sup::protocol::kEncoding_Base64 }
  }};
  auto client_2 = factory.CreateRPCClient(client_def_2);

  // Send request through client 1 and validate
  sup::dto::AnyValue request = {{
    { "setpoint", { sup::dto::Float64Type, 3.5 }},
    { "enabled", false }
  }};
  sup::dto::AnyValue reply = {{
    { "counter", { sup::dto::UnsignedInteger16Type, 2u }},
    { "message", "ok" }
  }};
  test_protocol.SetReply(reply);
  sup::dto::AnyValue output;
  EXPECT_EQ(client_1->Invoke(request, output), sup::protocol::Success);
  EXPECT_EQ(test_protocol.GetRequest(), request);
  EXPECT_EQ(output, reply);

  // Send request through client 2 and validate
  request["setpoint"] = 1.0;
  request["enabled"] = true;
  reply["counter"].ConvertFrom(42u);
  reply["message"] = "hello";
  test_protocol.SetReply(reply);
  EXPECT_EQ(client_2->Invoke(request, output), sup::protocol::Success);
  EXPECT_EQ(test_protocol.GetRequest(), request);
  EXPECT_EQ(output, reply);

  // Send request through client 2 with non-success reply
  test_protocol.SetReply(reply, sup::protocol::ServerProtocolDecodingError);
  output = sup::dto::AnyValue{};
  EXPECT_EQ(client_2->Invoke(request, output), sup::protocol::ServerProtocolDecodingError);
  EXPECT_EQ(test_protocol.GetRequest(), request);
  EXPECT_TRUE(sup::dto::IsEmptyValue(output));
}

TEST_F(EPICSProtocolFactoryTest, RPCFactoryFunctions)
{
  EPICSProtocolFactory factory;
  TestProtocol test_protocol;
  const std::string service_name = "EPICSRPCFactoryFunctions::TestServer";

  // Create RPC server stack
  auto server = CreateEPICSRPCServerStack(test_protocol, GetDefaultRPCServerConfig(service_name));

  // Create corresponding RPC client without encoding
  auto client_1 = CreateEPICSRPCClientStack(GetDefaultRPCClientConfig(service_name),
                                            sup::protocol::PayloadEncoding::kNone);

  // Create corresponding RPC client with base64 encoding
  auto client_2 = CreateEPICSRPCClientStack(GetDefaultRPCClientConfig(service_name),
                                            sup::protocol::PayloadEncoding::kBase64);

  // Send request through client 1 and validate
  sup::dto::AnyValue request = {{
    { "setpoint", { sup::dto::Float64Type, 3.5 }},
    { "enabled", false }
  }};
  sup::dto::AnyValue reply = {{
    { "counter", { sup::dto::UnsignedInteger16Type, 2u }},
    { "message", "ok" }
  }};
  test_protocol.SetReply(reply);
  sup::dto::AnyValue output;
  EXPECT_EQ(client_1->Invoke(request, output), sup::protocol::Success);
  EXPECT_EQ(test_protocol.GetRequest(), request);
  EXPECT_EQ(output, reply);

  // Send request through client 2 and validate
  request["setpoint"] = 1.0;
  request["enabled"] = true;
  reply["counter"].ConvertFrom(42u);
  reply["message"] = "hello";
  test_protocol.SetReply(reply);
  EXPECT_EQ(client_2->Invoke(request, output), sup::protocol::Success);
  EXPECT_EQ(test_protocol.GetRequest(), request);
  EXPECT_EQ(output, reply);

  // Send request through client 2 with non-success reply
  test_protocol.SetReply(reply, sup::protocol::ServerProtocolDecodingError);
  output = sup::dto::AnyValue{};
  EXPECT_EQ(client_2->Invoke(request, output), sup::protocol::ServerProtocolDecodingError);
  EXPECT_EQ(test_protocol.GetRequest(), request);
  EXPECT_TRUE(sup::dto::IsEmptyValue(output));
}