#
#  Created on: 06.1.2012
#      Author: mbodan01

LIBRARY_PREFIX=lc_jni

WIN_DIST_DIR=../../com.stericsson.sdk.loader.communication.win32
MACOS_DIST_DIR=../../com.stericsson.sdk.loader.communication.macos64
LINUX64_DIST_DIR=../../com.stericsson.sdk.loader.communication.linux64
LINUX_DIST_DIR=../../com.stericsson.sdk.loader.communication.linux32

#Object files
OBJ=$(BUILD_DIR)/JNInative.o $(BUILD_DIR)/util.o $(BUILD_DIR)/Logger.o 

#Sources
SRC=JNInative.cpp util.cpp ../../com.stericsson.sdk.equipment.io.uart/jni/src/Logger.cpp

#Headers
HEAD=LCDInterface.h JNInative.h javadefs.h lcddefs.h util.h\
	com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface.h

include ../../jni.mk
#Rule for compiling Logger.cpp
INCLUDES+= -I../../com.stericsson.sdk.equipment.io.uart/jni/src/
$(BUILD_DIR)/Logger.o : ../../com.stericsson.sdk.equipment.io.uart/jni/src/Logger.cpp
	@echo
	@echo $(SEPARATOR)$(SEPARATOR)
	@echo "$(SEPARATOR) Compiling '$<'"
	@echo $(SEPARATOR)$(SEPARATOR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o "$@"

