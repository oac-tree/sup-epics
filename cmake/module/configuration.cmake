# -----------------------------------------------------------------------------
# Common project configuration
# -----------------------------------------------------------------------------

include(COACommonConfig)
include(COASetupCodacEnvironment)

find_package(EPICS REQUIRED MODULE)
find_package(sup-dto REQUIRED)
find_package(sup-protocol REQUIRED)
find_package(sup-di REQUIRED)
find_package(sup-utils REQUIRED)