# Refactor ChannelAccess PV translation between different types (COA-666)

## Unit tests

Start with unit tests to verify what works and what not:

* Create basic IOC records that cover the range of supported integers/floats.
* Subscribe to those using different `AnyType` arguments.
* Set values of yet another `AnyType` to verify automatic conversion during update.
* Get values of the channel and verify these are correctly converted back.

## Construction and retrieval of CA channel value

Handle the appropriate conversion in CAMonitorWrapper, which already knows about the underlying AnyType.

## Update of CA channel value

Store the AnyType argument passed in the constructor of ChannelInfo. Use this information in `CAChannelManager::UpdateChannel` to convert the passed `AnyValue` to the appropriate type first. From this value, that now has the correct `AnyType`, create the proper byte vector to pass to the update task.
