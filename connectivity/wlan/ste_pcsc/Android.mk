ifeq ($(WLAN_SET_PLATFORM),u8500)
STE_PCSC_MODEM := RIL
STE_PCSC_LIB := true
endif

ifeq ($(WLAN_SET_PLATFORM),u9540)
STE_PCSC_MODEM := CAIF
STE_PCSC_LIB := true
endif

STE_PCSC_TEST := false
STE_PCSC_TEST_AT := false
STE_PCSC_DEBUG := false

ifeq ($(STE_PCSC_LIB),true)
#################################################
#   STE_PCSC_LIB
#################################################
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	ste_pcsc.c \
	ste_pcsc_utils.c

ifeq ($(STE_PCSC_MODEM),RIL)
LOCAL_SRC_FILES += \
	modem_utils_ril.c
LOCAL_CFLAGS += -DRIL
endif

ifeq ($(STE_PCSC_MODEM),CAIF)
LOCAL_SRC_FILES += \
	modem_utils_caif.c
LOCAL_CFLAGS += -DCAIF
endif

ifeq ($(STE_PCSC_TEST_AT),true)
  LOCAL_CFLAGS += -DTEST_AT
endif

ifeq ($(STE_PCSC_DEBUG),true)
  LOCAL_CFLAGS += -DDEBUG
endif

LOCAL_LDLIBS += -llog

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_MODULE    := libste_pcsc
LOCAL_MODULE_TAGS := optional
LOCAL_COPY_HEADERS := ste_pcsc.h
include $(BUILD_STATIC_LIBRARY)

#################################################
# 	STE_PCSC_TEST
#################################################

ifeq ($(STE_PCSC_TEST),true)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	test/ste_pcsc_cli.c

ifeq ($(STE_PCSC_TEST_AT),true)
	LOCAL_CFLAGS += -DTEST_AT
endif

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include/PCSC
LOCAL_MODULE    := ste_pcsc_cli
LOCAL_MODULE_TAGS := debug
LOCAL_STATIC_LIBRARIES := libste_pcsc
LOCAL_SHARED_LIBRARIES := libc libcutils
include $(BUILD_EXECUTABLE)
endif #STE_PCSC_TEST

endif #STE_PCSC_LIB
