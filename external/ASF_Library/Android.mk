LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_C_FLAGS += -DINLINE=__inline__
LOCAL_CFLAGS += -Wno-multichar

LOCAL_C_INCLUDES := \
        $(LOCAL_PATH)/include

LOCAL_SRC_FILES +=  \
        asf.c  \
        byteio.c  \
        data.c  \
        guid.c  \
        header.c  \
        parse.c

LOCAL_MODULE:= libasf

LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
