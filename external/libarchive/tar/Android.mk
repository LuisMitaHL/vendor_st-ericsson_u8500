#/************************************************************************
# *                                                                      *
# *  Copyright (C) 2010 ST-Ericsson                                      *
# *                                                                      *
# *  Author: Patrice CHOTARD <patrice.chotard AT stericsson.com>         *
# *                                                                      *
# ************************************************************************/

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_CFLAGS += -DHAVE_CONFIG_H

LOCAL_C_INCLUDES += $(LOCAL_PATH)/..
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libarchive
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libarchive_fe
LOCAL_C_INCLUDES += external/zlib


LOCAL_MODULE    := bsdtar

LOCAL_SRC_FILES := \
	bsdtar.c \
	bsdtar_windows.c \
	cmdline.c \
	getdate.c \
	read.c \
	subst.c \
	tree.c \
	util.c \
	write.c \
	mbtowc.c \

LOCAL_SHARED_LIBRARIES := \
	libarchive \
	libarchive_fe

LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)



