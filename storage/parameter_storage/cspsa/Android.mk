#/************************************************************************
# *                                                                      *
# *  Copyright (C) 2010 ST-Ericsson SA                                   *
# *                                                                      *
# *  This software is released either under the terms of the BSD-style   *
# *  license accompanying CSPSA or a proprietary license obtained from   *
# *  ST-Ericsson SA.                                                     *
# *                                                                      *
# *  Author: Martin LUNDHOLM <martin.xa.lundholm AT stericsson.com>      *
# *                                                                      *
# ************************************************************************/

LOCAL_PATH:= $(call my-dir)

CSPSA_COMMON_CFLAGS := -Wall -Wextra -Werror -DCFG_CSPSA_ANDROID=1 \
	-DCFG_CSPSA_API_LINUX_SOCKET_COMMON_SOCKET_PATH='"/dev/socket/cspsa_socket_"'

# ------------------------------------------------------------------------------
# - Build cspsa-server.
# ------------------------------------------------------------------------------

include $(CLEAR_VARS)

LOCAL_CFLAGS += $(CSPSA_COMMON_CFLAGS)

LOCAL_SRC_FILES := \
	cspsa_core.c \
	crc32.c \
	cspsa_ll.c \
	cspsa_server.c \
	cspsa_api_linux_socket_server.c \
	cspsa_api_linux_socket_common.c \
	config_file_reader.c \
	cspsa_ll_dev.c \
	cspsa_ll_ram.c \
	cspsa_ll_mmc.c \
	cspsa_ll_file.c

#TODO: Should be named 'cspsa-server' (or possible only 'cspsa'),
# but init.rc script in rootdir must also then be updated.
LOCAL_MODULE := cspsa-server
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

# ------------------------------------------------------------------------------
# - Build dynamic client lib.
# ------------------------------------------------------------------------------

include $(CLEAR_VARS)

LOCAL_MODULE := libcspsa
LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_CLASS := SHARED_LIBRARIES

LOCAL_CFLAGS += $(CSPSA_COMMON_CFLAGS)

LOCAL_PRELINK_MODULE := no

LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)

LOCAL_SRC_FILES := \
	cspsa_api_linux_socket_common.c \
	cspsa_api_linux_socket_client.c

include $(BUILD_SHARED_LIBRARY)

# ------------------------------------------------------------------------------
# - Install cspsa.conf.
# ------------------------------------------------------------------------------

include $(CLEAR_VARS)

LOCAL_MODULE_CLASS := ETC

LOCAL_MODULE := cspsa.conf
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := config/cspsa.conf

include $(BUILD_PREBUILT)

# ------------------------------------------------------------------------------
# - Build cspsa_linux_us_test_client_cmd (for test and debug).
# ------------------------------------------------------------------------------

include $(CLEAR_VARS)

LOCAL_CFLAGS += $(CSPSA_COMMON_CFLAGS)

LOCAL_SRC_FILES := cspsa_api_linux_socket_client.c cspsa_api_linux_socket_common.c cspsa_test_client_cmd.c

LOCAL_MODULE := cspsa-cmd
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

# ------------------------------------------------------------------------------
# - Build static cspsa lib for host tools.
# ------------------------------------------------------------------------------

include $(CLEAR_VARS)

LOCAL_MODULE := libcspsa
LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS += $(CSPSA_COMMON_CFLAGS)

LOCAL_SRC_FILES := \
	cspsa_api_linux_socket_common.c \
	cspsa_api_linux_socket_client.c

include $(BUILD_HOST_STATIC_LIBRARY)

# ------------------------------------------------------------------------------
# - Build static core lib for host tools.
# ------------------------------------------------------------------------------

include $(CLEAR_VARS)

LOCAL_MODULE := libcspsa-core
LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS += $(CSPSA_COMMON_CFLAGS)

LOCAL_SRC_FILES := \
	cspsa_core.c \
	crc32.c \
	cspsa_ll.c

include $(BUILD_HOST_STATIC_LIBRARY)

# ------------------------------------------------------------------------------
# - Build static ll file lib for host tools.
# ------------------------------------------------------------------------------

include $(CLEAR_VARS)

LOCAL_MODULE := libcspsa-ll-file
LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS += $(CSPSA_COMMON_CFLAGS)

LOCAL_SRC_FILES := cspsa_ll_file.c

include $(BUILD_HOST_STATIC_LIBRARY)

# ------------------------------------------------------------------------------
