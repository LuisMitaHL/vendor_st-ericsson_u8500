#*****************************************************************************/
#
# Copyright (C) ST-Ericsson SA 2012. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#
#
#*****************************************************************************/

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES :=  \
        alsactrl_alsa.c \
        alsactrl_fm.c \
        alsactrl_audiocodec.c \
        alsactrl_debug.c \
        alsactrl_hwh.c \
        alsactrl_hwh_db.c \
        alsactrl_hwh_u85xx.c \
        alsactrl_statefile_parser.c \
        alsactrl_tinyalsa_extn.c \
        alsactrl_hwh_u8500.c \
        alsactrl_hwh_u8500_dev.c \
        alsactrl_hwh_u8500_d2d.c \
        alsactrl_hwh_u8500_vc.c \
        alsactrl_hwh_u8540.c \
        alsactrl_hwh_u8540_dev.c \
        alsactrl_hwh_u8540_d2d.c \
        alsactrl_hwh_u8540_vc.c \
        alsactrl_cscall.c

LOCAL_CFLAGS += -D_POSIX_C_SOURCE
LOCAL_CFLAGS += -O3 -Wall -funwind-tables
LOCAL_CFLAGS += -DALSACTRL_LOG_ERRORS -DADM_LOG_WARNINGS

LOCAL_MODULE           := libalsactrl
LOCAL_MODULE_TAGS      := optional
LOCAL_PRELINK_MODULE   := false

LOCAL_SHARED_LIBRARIES := \
        libcutils \
        libutils \
        libdl \
        libc \
        libsqlite \
        libtinyalsa

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/include \
    $(LOCAL_PATH)/ \
    $(TOP)/external/sqlite/dist/  \
    $(TOP)/external/tinyalsa/include/

include $(BUILD_SHARED_LIBRARY)
