#/************************************************************************
# *                                                                      *
# *  Copyright (C) 2010 ST-Ericsson SA                                   *
# *                                                                      *
# *  This software is released either under the terms of the BSD-style   *
# *  license accompanying CSPSA or a proprietary license obtained from   *
# *  ST-Ericsson SA.                                                     *
# *                                                                      *
# *  Author: Sebastian RASMUSSEN <sebastian.rasmussen AT stericsson.com> *
# *                                                                      *
# ************************************************************************/

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	factoryreset.c

LOCAL_CFLAGS += -Wall -Wextra -Werror
LOCAL_CFLAGS += -DCFG_CSPSA_ANDROID=1 -DPROGRESS_FLAG_IN_CSPSA=1

LOCAL_C_INCLUDES += system/extras/ext4_utils $(STORAGE_PATH)/parameter_storage/cspsa
LOCAL_STATIC_LIBRARIES += libext4_utils libz libcutils
LOCAL_SHARED_LIBRARIES := libcspsa

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= factoryreset
include $(BUILD_EXECUTABLE)
