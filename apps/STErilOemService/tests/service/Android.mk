LOCAL_PATH:= $(call my-dir)

# Relative path to the STErilOEmService
LOCAL_SERVICE_PATH := ../../service

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := tests

LOCAL_JAVA_LIBRARIES :=

LOCAL_SRC_FILES := $(call all-java-files-under, src)
# build in STErilOemService to avoid linkage problems during runtime
LOCAL_SRC_FILES += $(LOCAL_SERVICE_PATH)/src/com/stericsson/ril/oem/service/STErilOemService.java

LOCAL_PACKAGE_NAME := IsolatedSTErilOemService

LOCAL_JAVA_LIBRARIES := com.stericsson.ril.oem

LOCAL_REQUIRED_MODULES := com.stericsson.ril.oem

LOCAL_CERTIFICATE := platform

include $(BUILD_PACKAGE)

