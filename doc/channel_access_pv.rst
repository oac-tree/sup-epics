EPICS Channel Access PVs
========================


The ``ChannelAccessPV`` class provides a convenient API to represent a client connection to an EPICS Channel Access process variable (PV). Upon construction, it will try to establish a channel to this PV and allows the user to define callbacks for connection and update events.

.. contents::
   :local:

A note on types
---------------

An EPICS Channel Access PV has a definite record type, e.g. binary input (bi), analog output (ao), etc. When creating a client connection to such a PV, an ``AnyType`` needs to be provided to indicate the desired format of the ``AnyValue`` that will be made available at the client side. This process allows for certain types of conversions, but also imposes some restrictions.

A distinction is made between scalar numerical types and strings, array types and enumeration types. It is advised to adhere to the following rules in order not to face surprises:

* For numerical records, use the ``AnyType`` that corresponds directly to the underlying record type's value format. Larger integer representations can be used safely as long as they have the same signedness, i.e. do not mix signed and unsigned types.
* For string records, use the string ``AnyType``.
* For enumeration records (bi/bo/mbbi/mbbo), use an unsigned ``AnyType`` to get the numerical index (any size will do, but 16bit is preferred as that is the native size of the record's index value) and use ``sup::dto::StringType`` to retrieve the string enumerator.
* For waveform and array records (aai/aao/waveform), use an array ``AnyType`` with an element type that corresponds to the native element's format. Again, larger representations will work as expected if the signedness of integers is respected.

Scalar numerical record types and strings
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

For record types that are numerical scalars, i.e. their value encodes just a number, there is always a corresponding ``AnyType`` that has the exact same precision and range:

* Analog input/output (ai/ao) and other records with ``DOUBLE`` value: ``sup::dto::Float64Type``,
* Long input/output (longin/longout): ``sup::dto::SignedInteger32Type``,
* 64bit integer input/output (int64in/int64out): ``sup::dto::SignedInteger64Type``,
* String input/output (stringin/stringout) and other records that encode string values: ``sup::dto::StringType``.

When using this exact ``AnyType``, no conversion will take place and the resulting client PV will contain an exact representation of the PV's value. However, if the user wants to receive the values in a different format, the following rules apply:

* All values of numerical types can be retrieved as a string. This will result in a string representation of the value.
* String records can in general not be converted to a numerical ``AnyType``, unless they encode exactly a number that can be represented in the given ``AnyType``.
* Conversions from floating point records to integer ``AnyType`` will truncate the fractional part of the number. However, if the resulting integer does not fit inside the range of the ``AnyType``, the result can surprise the user so this should be avoided.
* Conversions between integer types typically truncate the most significant bits. Again, users should take care not to request the wrong type from an EPICS CA record.

Note that long string records (lsi/lso) are currently not supported.

Array types
^^^^^^^^^^^

Record types that encode arrays (aai/aao/waveform) should be requested with an array ``AnyType``, whose element type corresponds to the record's element type:

* aai/aao: element type should be ``sup::dto::Float64Type``,
* waveform: element type should be the one used in the record's definition (can be any integer or floating point type).

The same conversions as for scalars can occur here and the user is advised to adhere to the same rules regarding requested types.

Enumeration types
^^^^^^^^^^^^^^^^^

These represent a more complex case, since requesting a string will result in the string enumerator, as opposed to a string representation of a number. Due to limitations in the EPICS CA client library, some fundamental types cannot be encoded directly over a connected channel:

* Unsigned integers, other than ``sup::dto::UnsignedInteger16Type`` cannot be transferred over the channel as numerical values of the correct size. When requesting any unsigned integer ``AnyType``, this will result in a channel that will transfer the values as 16bit unsigned integers that will be converted to the appropriate target ``AnyType``. When used on enumeration types, this is perfectly fine, since the underlying index is also 16bit, so no truncation will take place.
* 64bit signed integers have a similar issue and will be encoded as strings over the channel. This can cause issues with enumeration records, since the retrieved string enumerator in general cannot be parsed as an integer. Follow the general rule and never use signed integer representations for retrieving the index of an enumeration record.

Note that requesting the type ``sup::dto::StringType`` will behave as expected and retrieve the string enumerator of the enumeration record.

Construction
------------

A ``ChannelAccessPV`` object is created by providing the EPICS channel name, a requested ``AnyType`` and an optional callback function that will be called on value or connection state updates.

.. function:: ChannelAccessPV(const std::string& channel, const sup::dto::AnyType& type, VariableChangedCallback cb = {});

   :param channel: Name of the EPICS channel.
   :param type: Requested type.
   :param cb: Optional callback function.

   Construct a client that connects to an EPICS Channel Access process variable.
