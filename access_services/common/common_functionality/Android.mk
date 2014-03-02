###############################################################################
#                                                                             #
# Copyright (C) ST-Ericsson SA 2011. All rights reserved.                     #
# This code is ST-Ericsson proprietary and confidential.                      #
# Any use of the code for whatever purpose is subject to                      #
# specific written permission of ST-Ericsson SA.                              #
#                                                                             #
###############################################################################

LOCAL_PATH := $(call my-dir)

### libaccsutil_log (library that is included in all other static libraries) ###
include $(CLEAR_VARS)

LOCAL_MODULE := libaccsutil_log
LOCAL_MODULE_TAGS := optional

LOCAL_C_INCLUDES :=	$(LOCAL_PATH)/include \
			$(LOCAL_PATH)/log/include
LOCAL_SRC_FILES :=	log/src/util_log.c
LOCAL_CFLAGS := -fno-short-enums -Wall -DCFG_USE_ANDROID_LOG

include $(BUILD_STATIC_LIBRARY)


### libaccsutil_security (static library) ###
include $(CLEAR_VARS)

LOCAL_MODULE := libaccsutil_security
LOCAL_MODULE_TAGS := optional

LOCAL_C_INCLUDES :=	$(LOCAL_PATH)/include \
			$(LOCAL_PATH)/security/include \
			system/core/include/private
LOCAL_SRC_FILES :=	security/src/util_user.c \
			security/src/util_file.c
LOCAL_CFLAGS := -fno-short-enums -Wall -DANDROID_BUILD

LOCAL_WHOLE_STATIC_LIBRARIES := libaccsutil_log

include $(BUILD_STATIC_LIBRARY)



### libaccsutil_mainloop (static) ###
include $(CLEAR_VARS)

LOCAL_MODULE := libaccsutil_mainloop
LOCAL_MODULE_TAGS := optional

LOCAL_C_INCLUDES :=	$(LOCAL_PATH)/include
LOCAL_SRC_FILES :=	main_loop/src/main_loop.c

LOCAL_CFLAGS := -fno-short-enums -Wall -DANDROID_BUILD

LOCAL_WHOLE_STATIC_LIBRARIES := libaccsutil_log

include $(BUILD_STATIC_LIBRARY)

### libaccsutil_char_conv (dynamic library) ###
ifeq ($(COMMON_ENABLE_FEATURE_LIBACCSUTIL_CHAR_CONV),true)

include $(CLEAR_VARS)

LOCAL_MODULE:=libaccsutil_char_conv
LOCAL_MODULE_TAGS := optional

LOCAL_C_INCLUDES :=	$(LOCAL_PATH)/include
LOCAL_SRC_FILES  :=	char_conv/src/util_char_conv.c
LOCAL_CFLAGS:= -fno-short-enums -Wall

LOCAL_WHOLE_STATIC_LIBRARIES := libaccsutil_log
LOCAL_SHARED_LIBRARIES := libutils
LOCAL_PRELINK_MODULE:= false

include $(BUILD_SHARED_LIBRARY)

endif # COMMON_ENABLE_FEATURE_LIBACCSUTIL_CHAR_CONV


### libaccsutil_predial_checks (dynamic library) ###
ifeq ($(COMMON_ENABLE_FEATURE_LIBACCSUTIL_PREDIAL_CHECKS),true)

include $(CLEAR_VARS)

LOCAL_MODULE := libaccsutil_predial_checks
LOCAL_MODULE_TAGS := optional

LOCAL_C_INCLUDES :=     $(LOCAL_PATH)/include
LOCAL_SRC_FILES  :=     predial_checks/src/predial_checks.c
LOCAL_CFLAGS:= -fno-short-enums -Wall

LOCAL_WHOLE_STATIC_LIBRARIES := libaccsutil_log
LOCAL_SHARED_LIBRARIES := libutils
LOCAL_PRELINK_MODULE:= false

include $(BUILD_SHARED_LIBRARY)

endif # COMMON_ENABLE_FEATURE_LIBACCSUTIL_PREDIAL_CHECKS


### static files ###

CF_FILE_PATH := $(LOCAL_PATH)

# telephony_log_levels

include $(CLEAR_VARS)

LOCAL_PATH := $(CF_FILE_PATH)
LOCAL_SRC_FILES := telephony_log_levels
LOCAL_MODULE := $(LOCAL_SRC_FILES)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := DATA
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/data/misc

include $(BUILD_PREBUILT)
