#
#  Created on: 06.1.2012
#      Author: mbodan01
#

LIBRARY_PREFIX=usb_jni

#Directory for windows specifics - libraries, defs etc.
WIN_LIB_DIR=../../windows-usb-driver/installer/data/x86
WIN_INCLUDE_DIR=headers/windows

WIN_DIST_DIR=../../com.stericsson.sdk.equipment.io.usb.win32
MACOS_DIST_DIR=../../com.stericsson.sdk.equipment.io.usb.macos64
LINUX64_DIST_DIR=../../com.stericsson.sdk.equipment.io.usb.linux64
LINUX_DIST_DIR=../../com.stericsson.sdk.equipment.io.usb.linux32


#Object files
OBJ=$(BUILD_DIR)/libusb_jni.o $(BUILD_DIR)/USBHandler.o $(BUILD_DIR)/USBDevice.o $(BUILD_DIR)/util.o $(BUILD_DIR)/Logger.o 

#Sources
SRC=libusb_jni.cpp USBHandler.cpp USBDevice.cpp util.cpp Logger.cpp


#Headers
HEAD=com_stericsson_sdk_equipment_io_usb_internal_USBNativeDevice.h \
     com_stericsson_sdk_equipment_io_usb_internal_USBNativeHandler.h \
     javadefs.h USBDevice.h USBHandler.h util.h headers/windows/usb.h

include ../../jni.mk
#Rule for compiling Logger.cpp
INCLUDES+= -I../../com.stericsson.sdk.equipment.io.uart/jni/src/
$(BUILD_DIR)/Logger.o : ../../com.stericsson.sdk.equipment.io.uart/jni/src/Logger.cpp
	@echo
	@echo $(SEPARATOR)$(SEPARATOR)
	@echo "$(SEPARATOR) Compiling '$<'"
	@echo $(SEPARATOR)$(SEPARATOR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o "$@"

ifneq ( , $(findstring Darwin, $(SYS_INFO)))
  LLIBS+= -lusb
else ifneq ( , $(findstring Linux, $(SYS_INFO)))
  LLIBS+= -lusb
else
  VPATH+=:$(WIN_INCLUDE_DIR)
  INCLUDES+= -I"$(WIN_INCLUDE_DIR)"
  LLIBS+= $(WIN_LIB_DIR)/libusb0_x86.dll
endif
 
