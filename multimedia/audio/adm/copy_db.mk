#
# Copyright (C) ST-Ericsson SA 2012. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

ifeq ($(ADM_BUILD_PROD_SPEC),1)
LOCAL_MODULE           := adm.sqlite
else
LOCAL_MODULE           := adm.sqlite-$(ADM_DB_VARIANT)
endif
LOCAL_MODULE_TAGS      := optional
LOCAL_MODULE_CLASS     := ETC
LOCAL_SRC_FILES        := db/adm.sqlite-$(ADM_DB_VARIANT)
LOCAL_MODULE_PATH      := $(TARGET_OUT_ETC)

include $(BUILD_PREBUILT)
