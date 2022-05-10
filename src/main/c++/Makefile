.PHONY: all

TARGET=../../../target

# BUILD_DIR := ./build
BUILD_DIR := $(TARGET)/build

BUILD_TYPE = RelWithDebInfo

CMAKE_FLAGS := -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
CMAKE_FLAGS += -DCMAKE_INSTALL_PREFIX=$(TARGET)

ifeq ($(COVERAGE),true)
CMAKE_FLAGS += -DCOVERAGE=true
else
CMAKE_FLAGS += -DCOVERAGE=false
endif

all: cmake_targets

cmake_targets:
	@mkdir -p $(BUILD_DIR)
	cp -r ../../../source/libsup-common-epics/sup .
	cmake -B$(BUILD_DIR)  $(CMAKE_FLAGS) ../../..
	$(MAKE) -C $(BUILD_DIR) install
