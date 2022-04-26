# Supervision and automation system EPICS interface

Library of common components to send/receive SUP data-transfer-objects via EPICS Channel Access and PV access network protocols.
This is bare CMake project intended for the development and prototyping on Linux of the user's choice.
CODAC packaging will follow at a later stage.

## Installation on CODAC development machines

```
# install Epics
yum install codac-core-7.1-epics-devel.x86_64

# install sup-dto
git clone https://git.iter.org/scm/coa/sup-dto.git
mvn clean install

# install this packages
git clone https://git.iter.org/scm/coa/sup-common-epics.git
mkdir <build>; cd <build>
cmake <source> && make -j4 && ctest
```

## Installation on Linux of the user's choice

The `sup-comon-epics`  requires EPICS, PVXS and `coa-compact` libraries.

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

### Install coa-compact

```
git clone https://git.iter.org/scm/coa/coa-compact.git
mkdir <build>; cd <build>
cmake <source> && make -j4 && ctest
```

Please note, that thanks to CMake magic (the magic is located in $HOME/.cmake directory), installation is optional. 
Without installation, libraries will be discoverable right from the build directory.

### Finally, install `cpp-common-epics`

```
git clone https://git.iter.org/scm/coa/sup-common-epics.git
mkdir <build>; cd <build>
cmake <source> && make -j4 && ctest
```
