# Supervision and automation system EPICS interface

Library of components to send/receive SUP data-transfer-objects via EPICS Channel Access and PvAccess network protocols.
This is a CMake project intended development and deployment on most Linux systems.

## Installation on CODAC development machines

```bash
# install Epics
yum install codac-core-7.1-epics-devel.x86_64

# install sup-dto
git clone https://git.iter.org/scm/coa/sup-dto.git
mvn clean install

# install this packages
git clone https://git.iter.org/scm/coa/sup-epics.git
mvn clean install
```

## Installation on custom Linux system

The `sup-epics` module requires EPICS, PVXS and `sup-dto` libraries.

### Install EPICS and PVXS

Installation recipe (one of many possible) can be found [here](https://mdavidsaver.github.io/pvxs/building.html).
Short excerpt is shown below.

```bash
# install epics
git clone --branch 7.0 https://github.com/epics-base/epics-base.git
make -C <epics-base>

# install pvxs
git clone --recursive https://github.com/mdavidsaver/pvxs.git

# create file <pvxs>/configure/RELEASE.local with the content (use real path)
EPICS_BASE=<epics-base>

make -C <pvxs>

make -C <pvxs> runtests

# define environment variables in .bashrc  (use real path)
export EPICS_BASE=<epics-base>
export EPICS_HOST_ARCH=$(${EPICS_BASE}/startup/EpicsHostArch)
export PVXS_DIR=<pvxs>
```

### Install `sup-dto`

```bash
git clone https://git.iter.org/scm/coa/sup-dto.git
mkdir <build>; cd <build>
cmake <source> && make -j4 && ctest
```

Please note, that thanks to CMake magic (the magic is located in $HOME/.cmake directory), installation is optional.
Without installation, libraries will be discoverable right from the build directory.

### Finally, install `sup-epics`

```bash
git clone https://git.iter.org/scm/coa/sup-epics.git
mkdir <build>; cd <build>
cmake <source> && make -j4 && ctest
```

## Limitations of ChannelAccessPV

Since the ChannelAccess C library does not support most unsigned types directly (nor 64bit signed integeres), those are supported here by requesting values in string format. This happens for `UInt8`, `UInt32`, `Int64`, `UInt64` and arrays thereof. This implies that if the supporting EPICS record consists of boolean input/output records, this will fail (since their string representation is `TRUE`/`FALSE` and cannot be parsed as a number). The same applies for using those types to write to boolean records.
