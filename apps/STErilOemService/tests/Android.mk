LOCAL_TEST_TOP_PATH:= $(call my-dir)

include $(CLEAR_VARS)

# NOTE! you have to run mm on this level to build the tests
ifndef $(LOCAL_STEROS_SERVICE_NAME)
	LOCAL_STEROS_SERVICE_NAME := STErilOemService
endif

ifndef $(LOCAL_STEROS_LIB_NAME)
	LOCAL_STEROS_LIB_NAME := com.stericsson.ril.oem.service
endif

# Build the client and service test packages
include $(LOCAL_TEST_TOP_PATH)/client/Android.mk
include $(LOCAL_TEST_TOP_PATH)/service/Android.mk
