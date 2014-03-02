# Copyright (C) 2008 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Library

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

# check for u5500
ifeq ($(STE_HDMISERVICE_SET_PLATFORM),u5500)
LOCAL_CFLAGS := -DSTE_PLATFORM_U5500
endif #module configuration u5500

LOCAL_PRELINK_MODULE := false
LOCAL_SRC_FILES := src/hdmi_service_api.c src/hdmi_service.c src/cec.c \
	src/edid.c src/hdcp.c src/setres.c src/kevent.c src/socket.c
LOCAL_CFLAGS += -DANDROID
LOCAL_CFLAGS += -DLOG_TAG=\"HDMIservice\"
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include
LOCAL_SHARED_LIBRARIES := liblog
LOCAL_MODULE := libhdmi_service
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

# Executable, to be used to start service when there is no daemon
include $(CLEAR_VARS)
LOCAL_PRELINK_MODULE := false
LOCAL_SRC_FILES := src/hdmi_service_start.c
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include
LOCAL_SHARED_LIBRARIES := libhdmi_service
LOCAL_MODULE := hdmi_service_st
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)
