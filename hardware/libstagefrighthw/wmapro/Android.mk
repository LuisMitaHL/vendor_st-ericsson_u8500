##
## Copyright (C) ST-Ericsson SA 2012
##

ifneq ($(ENABLE_ST_ERICSSON_BUILD), true)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    STE_SoftWMAPro_v10dec.cpp

LOCAL_C_INCLUDES := \
    $(TOP)/frameworks/av/media/libstagefright/include \
    $(TOP)/frameworks/native/include/media/openmax \
    $(MULTIMEDIA_PATH)/audio/wmapro_v10/common/include \
    $(MULTIMEDIA_PATH)/audio/wmapro_v10/codec/include_asf \
    $(MULTIMEDIA_PATH)/audio/wmapro_v10/codec/include \
    $(MULTIMEDIA_PATH)/audio/audiolibs/common/include \
    $(MULTIMEDIA_PATH)/audio/audiolibs/fake_dsptools


LOCAL_CFLAGS := \
    -DARM -DNEON \
    -DBUILD_FIXED

LOCAL_SHARED_LIBRARIES := \
    libstagefright \
    libstagefright_omx \
    libstagefright_foundation \
    libutils \
    libdl

LOCAL_MODULE := libstagefright_soft_ste_wmapro_v10
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)

endif #ENABLE_ST_ERICSSON_BUILD
