#/************************************************************************
# *                                                                      *
# *  Copyright (C) 2011 ST-Ericsson SA                                   *
# *                                                                      *
# *  This software is released under the terms of the BSD-style          *
# *  license accompanying TOC.                                           *
# *                                                                      *
# *  Author: Mikael LARSSON <mikael.xt.larsson@stericsson.com>           *
# *                                                                      *
# ************************************************************************/

# ------------------------------------------------------------------------------
# - Build static lib.
# ------------------------------------------------------------------------------

LOCAL_PATH:= $(call my-dir)

#Build static library
include $(CLEAR_VARS)

LOCAL_CFLAGS += -Wall -Werror -Wextra

LOCAL_SRC_FILES := \
	toc_parser.c

LOCAL_MODULE := libtocparser
LOCAL_MODULE_TAGS := optional

include $(BUILD_STATIC_LIBRARY)

# Build the cmd-line tool
include $(CLEAR_VARS)

LOCAL_CFLAGS := -Wall -Wextra -Werror

LOCAL_SRC_FILES := \
	toc_parser_cmdline.c

LOCAL_STATIC_LIBRARIES := libtocparser

LOCAL_MODULE := tocparser
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
