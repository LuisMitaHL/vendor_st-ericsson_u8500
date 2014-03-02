LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
        socket.cpp \
        server.cpp \
        tuningServer.cpp \
        admRequest.cpp \
        tuningServer_utils.c

LOCAL_C_INCLUDES += $(MULTIMEDIA_PATH)/audio/adm/include

LOCAL_MODULE           := tuning_server
LOCAL_SHARED_LIBRARIES := libste_adm liblog
LOCAL_MODULE_TAGS      := optional

include $(BUILD_EXECUTABLE)
