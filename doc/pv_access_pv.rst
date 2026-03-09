EPICS PvAccess PVs
==================

For client connections to EPICS PvAccess process variables (PV), the following classes are provided:

* ``PvAccessClient``: a client that allows managing multiple PvAccess PVs;
* ``PvAccessClientPV``: a client for a single PvAccess PV.

On the server side, there is:

* ``PvAccessServer``: a server that manages multiple PvAccess PVs;
* ``PvAccessServerPV``: a server for a single PvAccess PV.

.. contents::
   :local:

Partial updates
---------------

Both client and server PVs allow to set partial values. This means that the update value may contain
less structure member fields, at any depth, than the underlying value. This only applies to structures
and cannot be used to do partial array updates.

Restrictions on types
---------------------

Not all types supported by ``AnyValue`` are currently supported for PvAccess PVs. The following restrictions apply:

* Scalar nodes (inside a structure or array) of the type ``char8`` will be converted to ``uint8``;
* Scalar values are not supported at the top level;
* Arrays, both when top level or as members of a struct, cannot be named.
