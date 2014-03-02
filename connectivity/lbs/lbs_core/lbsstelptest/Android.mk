#
# stelp_test_app
#
LOCAL_PATH := $(call my-dir)

ifeq ($(LBS_SET_NDK_BUILD),true)
LOCAL_SET_NDK_BUILD := true
endif

include $(CLEAR_VARS)
LOCAL_ENABLE_TLS_STUB := false

LOCAL_SRC_FILES := \
	srce/test_sbee.c \
	srce/main.c \
	srce/test_naf.c \
	srce/test_stelp_socket.c \
	srce/test_amilat.c \
	srce/test_production.c
ifeq ($(LOCAL_ENABLE_TLS_STUB),true)
LOCAL_SRC_FILES += srce/test_supl.c
LOCAL_CFLAGS += -DAGPS_UNIT_TEST_FTR
endif

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/incl \
	$(LOCAL_PATH)/../lbsd/include \
	$(LOCAL_PATH)/../liblbstestengine/incl \
	$(LOCAL_PATH)/../lbsclient/incl \
	$(LOCAL_PATH)/../lbsd/libagpsosa/incl  \
	$(LOCAL_PATH)/../include \
	$(LOCAL_PATH)/incl       \
	external/openssl/include

ifeq ($(LOCAL_SET_NDK_BUILD),true)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../lbs_external/openssl/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../lbs_external/utils_lib
LOCAL_LDLIBS += -llog
endif

LOCAL_CFLAGS += \
	-DAGPS_LINUX_FTR \
	-DAGPS_ANDR_ECLAIR_FTR \
	-DDEBUG_LOG_LEV2

LOCAL_SHARED_LIBRARIES := \
	libc \
        libutils \
	liblbstestengine \
	libclientgps \
	libssl \
	libcrypto \
	libz


LOCAL_MODULE := lbsstelp
LOCAL_MODULE_TAGS := tests

include $(BUILD_EXECUTABLE)

