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


LOCAL_PATH := $(call my-dir)
COPYBIT_PATH:= $(LOCAL_PATH)
include $(CLEAR_VARS)

$(TARGET_OUT_INTERMEDIATES)/SHARED_LIBRARIES/copybit.$(TARGET_BOARD_PLATFORM)_intermediates/lib/copybit.$(TARGET_BOARD_PLATFORM).so:
		@mkdir -p $(TARGET_OUT_INTERMEDIATES)/SHARED_LIBRARIES/copybit.$(TARGET_BOARD_PLATFORM)_intermediates/lib
		$(hide) cp -f $(COPYBIT_PATH)/copybit.st-ericsson.so $(TARGET_OUT_INTERMEDIATES)/SHARED_LIBRARIES/copybit.$(TARGET_BOARD_PLATFORM)_intermediates/lib/copybit.$(TARGET_BOARD_PLATFORM).so


LOCAL_PATH:= $(TARGET_OUT_INTERMEDIATES)/SHARED_LIBRARIES/copybit.$(TARGET_BOARD_PLATFORM)_intermediates/lib
LOCAL_MODULE := copybit.$(TARGET_BOARD_PLATFORM).so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw

LOCAL_SRC_FILES := $(LOCAL_MODULE)
OVERRIDE_BUILT_MODULE_PATH := $(TARGET_OUT_INTERMEDIATE_LIBRARIES)

include $(BUILD_PREBUILT)


