# Copyright (C) ST-Ericsson SA 2011. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.

CN_ENABLE_SECURITY_TEST_PROGRAM := false
ifeq ($(CN_ENABLE_SECURITY_TEST_PROGRAM),true)

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

CNS := $(ACCESS_SERVICES_PATH)/call_network/call_network

LOCAL_C_INCLUDES += \
	$(CNS)/libcn/include

LOCAL_SRC_FILES := cn_security_test.c

### Compiler flags and linking directives ###
LOCAL_CFLAGS := -fno-short-enums -Wall -DCFG_USE_ANDROID_LOG
LOCAL_SHARED_LIBRARIES := liblog libcn

### Server executable ###
LOCAL_MODULE:=cn_security_test
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)

endif # CN_ENABLE_SECURITY_TEST_PROGRAM
