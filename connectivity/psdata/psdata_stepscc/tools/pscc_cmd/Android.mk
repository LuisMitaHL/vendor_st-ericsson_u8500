######################
# Build PSCC command #
######################

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := pscc_cmd.c
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../libpscc/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../stepscc/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../psdata/libmpl/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../common/libstecom/
LOCAL_C_INCLUDES += $(KERNEL_HEADERS)

LOCAL_SHARED_LIBRARIES := libstecom libmpl libpscc
LOCAL_MODULE := pscc_cmd
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)