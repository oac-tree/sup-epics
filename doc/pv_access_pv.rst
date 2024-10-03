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

Restrictions on types
---------------------

Not all types supported by ``AnyValue`` are currently supported for PvAccess PVs. The following restrictions apply:

* Scalar nodes (inside a structure or array) of the type ``char8`` will be converted to ``uint8``;
* Scalar values are not supported at the top level;
* Arrays of structures are not supported;
* Arrays, both when top level or as members of a struct, cannot be named.
