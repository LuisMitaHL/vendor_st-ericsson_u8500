LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := $(LOCAL_STEROS_LIB_NAME)

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := $(call all-java-files-under, src)

LOCAL_CERTIFICATE := platform

include $(BUILD_JAVA_LIBRARY)


################## Install the permission file #####################

include $(CLEAR_VARS)

LOCAL_MODULE := com.stericsson.ril.oem.xml

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_CLASS := ETC

# This will install the file in /system/etc/permissions
#
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions

LOCAL_SRC_FILES := etc/$(LOCAL_MODULE)

include $(BUILD_PREBUILT)
