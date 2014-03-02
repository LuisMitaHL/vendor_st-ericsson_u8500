LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    STE_SoftAAC_Dec.cpp

LOCAL_C_INCLUDES := \
    $(TOP)/frameworks/av/media/libstagefright/include \
    $(TOP)/frameworks/native/include/media/openmax \
    $(MULTIMEDIA_PATH)/audio/aac_arm/codec/include

LOCAL_CFLAGS := \
    -DARM -DNEON -DMC -DSBR_MC \
    -DBUILD_FIXED

LOCAL_SHARED_LIBRARIES := \
    libstagefright \
    libstagefright_omx \
    libstagefright_foundation \
    libutils \
    libcutils \
    libdl

LOCAL_MODULE := libstagefright_soft_ste_aacdec
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
