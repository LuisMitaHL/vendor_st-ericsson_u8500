LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := tests

LOCAL_JAVA_LIBRARIES := android.test.runner \
	com.stericsson.thermal

LOCAL_SRC_FILES := $(call all-java-files-under, src)

LOCAL_PACKAGE_NAME := ThermalTests

include $(BUILD_PACKAGE)
