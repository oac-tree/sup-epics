# Refactor ChannelAccess PV translation between different types (COA-666)

## Unit tests

Start with unit tests to verify what works and what not:

* Set values of yet another `AnyType` to verify automatic conversion during update.
* Get values of the channel and verify these are correctly converted back.
