LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := tests

LOCAL_JAVA_LIBRARIES := android.test.runner com.stericsson.ril.oem

LOCAL_SRC_FILES := $(call all-java-files-under, src)

LOCAL_PACKAGE_NAME := STErilOemHookTests

LOCAL_REQUIRED_MODULES := com.stericsson.ril.oem

LOCAL_CERTIFICATE := platform

LOCAL_INSTRUMENTATION_FOR :=

include $(BUILD_PACKAGE)

