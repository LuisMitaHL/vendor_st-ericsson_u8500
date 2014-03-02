LOCAL_PATH := $(call my-dir)

# Build JAVA API
include $(CLEAR_VARS)
LOCAL_SRC_FILES := \
         $(call all-java-files-under, java)

LOCAL_JAVA_LIBRARIES := \
         core

LOCAL_JAVACFLAGS := \
        -Xlint:unchecked

LOCAL_MODULE := \
        com.stericsson.thermal

LOCAL_MODULE_TAGS := \
	optional eng debug

include $(BUILD_JAVA_LIBRARY)

# Inform Android
include $(CLEAR_VARS)

LOCAL_MODULE := \
        com.stericsson.thermal.xml

LOCAL_MODULE_CLASS := \
        ETC
LOCAL_MODULE_PATH := \
        $(TARGET_OUT_ETC)/permissions

LOCAL_SRC_FILES := \
        $(LOCAL_MODULE)

LOCAL_MODULE_TAGS := \
	optional eng debug

include $(BUILD_PREBUILT)

# Build JNI Bindings
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
        $(call find-subdir-subdir-files, jni, *cpp)

LOCAL_C_INCLUDES += \
        $(JNI_H_INCLUDE) \
	$(LOCAL_PATH)/../../../apps/thermal_service/libthermal

LOCAL_REQUIRED_MODULES += \
	libthermal

LOCAL_SHARED_LIBRARIES := \
        libandroid_runtime \
	libnativehelper \
	libthermal \
        libcutils

LOCAL_PRELINK_MODULE := \
       false

LOCAL_MODULE := \
        libthermal_jni

LOCAL_MODULE_TAGS := \
	optional eng debug

include $(BUILD_SHARED_LIBRARY)
