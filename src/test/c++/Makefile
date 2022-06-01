.PHONY: all

TARGET=../../../target

BUILD_DIR := $(TARGET)/build
BIN_DIR := $(TARGET)/bin

BUILD_TYPE = RelWithDebInfo

CMAKE_FLAGS := -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
CMAKE_FLAGS += -DCMAKE_INSTALL_PREFIX=$(TARGET)

ifeq ($(COVERAGE),true)
CMAKE_FLAGS += -DCOVERAGE=true
else
CMAKE_FLAGS += -DCOVERAGE=false
endif

all: unit-tests

unit-tests:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)
	cmake -B$(BUILD_DIR)  $(CMAKE_FLAGS) $(TARGET)
	$(MAKE) -C $(BUILD_DIR)
	cp $(BUILD_DIR)/bin/testsup-epics $(BIN_DIR)/.

