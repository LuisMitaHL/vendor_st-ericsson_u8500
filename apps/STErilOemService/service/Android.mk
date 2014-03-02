LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_PACKAGE_NAME := $(LOCAL_STEROS_SERVICE_NAME)

LOCAL_MODULE_TAGS := optional

LOCAL_JAVA_LIBRARIES := com.stericsson.ril.oem

LOCAL_REQUIRED_MODULES := com.stericsson.ril.oem

LOCAL_SRC_FILES := $(call all-java-files-under, src)

LOCAL_CERTIFICATE := platform

LOCAL_PROGUARD_FLAG_FILES := proguard.flags

include $(BUILD_PACKAGE)
