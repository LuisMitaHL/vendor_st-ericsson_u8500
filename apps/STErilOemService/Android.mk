LOCAL_TOP_PATH := $(call my-dir)


#build the library
LOCAL_STEROS_LIB_NAME := com.stericsson.ril.oem
include $(LOCAL_TOP_PATH)/lib/Android.mk

#build the service
LOCAL_STEROS_SERVICE_NAME := STErilOemService
include $(LOCAL_TOP_PATH)/service/Android.mk

#build the Testsuite
# Warning: The test package contains a variant of STErilOemService
# which runs in isolation from the Android telephony framework.
# This service will interfere with STErilOemService if they are
# installed at the same time.
# See tests/README for details
# include $(LOCAL_TOP_PATH)/tests/Android.mk
