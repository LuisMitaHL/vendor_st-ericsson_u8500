#
#  Created on: 06.1.2012
#      Author: mbodan01
#

LIBRARY_PREFIX=uart_jni

WIN_DIST_DIR=../../com.stericsson.sdk.equipment.io.uart.win32
MACOS_DIST_DIR=../../com.stericsson.sdk.equipment.io.uart.macos64
LINUX64_DIST_DIR=../../com.stericsson.sdk.equipment.io.uart.linux64
LINUX_DIST_DIR=../../com.stericsson.sdk.equipment.io.uart.linux32

#Object files
CORE_OBJ=$(BUILD_DIR)/util.o $(BUILD_DIR)/COMPort.o
SL_OBJ=$(CORE_OBJ) $(BUILD_DIR)/libuart_jni.o $(BUILD_DIR)/Logger.o
TA_OBJ=$(CORE_OBJ) $(BUILD_DIR)/test.o 
OBJ=$(SL_OBJ)

#Sources
SRC=COMPort.cpp util.cpp test.cpp libuart_jni.cpp Logger.cpp

#Headers
HEAD=util.h COMPort.h\
	com_stericsson_sdk_equipment_io_uart_internal_SerialPortHandler.h \
	com_stericsson_sdk_equipment_io_uart_internal_SerialPort.h

TEST_APP=uarttest

include ../../jni.mk

build: $(BUILD_DIR)/$(TEST_APP)

clean_build: $(BUILD_DIR)/$(TEST_APP)

$(BUILD_DIR)/$(TEST_APP): $(TA_OBJ)
	@echo
	@echo $(SEPARATOR)$(SEPARATOR)
	@echo "$(SEPARATOR) Linking '$(BUILD_DIR)/$(TEST_APP)'"
	@echo $(SEPARATOR)$(SEPARATOR)
	$(CC) $(LFLAGS) $(TA_OBJ) -o $(BUILD_DIR)/$(TEST_APP)

$(TA_OBJ): $(filter-out $(wildcard $(BUILD_DIR)), $(BUILD_DIR)) $(HEAD)

