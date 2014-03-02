LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	main.c \
	test.c \
	thermal.c

LOCAL_MODULE := \
	thermaltest

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../libthermal

LOCAL_MODULE_TAGS := \
	debug

LOCAL_SHARED_LIBRARIES := \
	libthermal

include $(BUILD_EXECUTABLE)
