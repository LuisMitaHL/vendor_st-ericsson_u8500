#
# Copyright (C) ST-Ericsson SA 2012. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

LOCAL_PATH := $(call my-dir)

################################################################################
#
# mmprobed
#

include $(CLEAR_VARS)

LOCAL_SRC_FILES :=  \
        src/main.c \
        src/server.c \
        src/logger.c \
        src/utils.c

LOCAL_C_INCLUDES       += \
	$(LOCAL_PATH)/api \
	$(LOCAL_PATH)/include \
	$(MULTIMEDIA_PATH)/audio/adm/include

LOCAL_MODULE           := mmprobed
LOCAL_MODULE_TAGS      := optional
LOCAL_PRELINK_MODULE   := false

LOCAL_SHARED_LIBRARIES := libste_adm \
	        libcutils \
			libstelpcutils


include $(BUILD_EXECUTABLE)



################################################################################
#
# libmmprobe.so
#

include $(CLEAR_VARS)

LOCAL_SRC_FILES :=  \
        src/client.c

LOCAL_C_INCLUDES       += \
	$(LOCAL_PATH)/api \
	$(LOCAL_PATH)/include

LOCAL_MODULE           := libmmprobe
LOCAL_MODULE_TAGS      := optional
LOCAL_PRELINK_MODULE   := false

LOCAL_SHARED_LIBRARIES := libcutils

include $(BUILD_SHARED_LIBRARY)
