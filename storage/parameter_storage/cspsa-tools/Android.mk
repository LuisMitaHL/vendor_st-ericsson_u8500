#/************************************************************************
# *                                                                      *
# *  Copyright (C) 2012 ST-Ericsson SA                                   *
# *                                                                      *
# *  This software is released either under the terms of the BSD-style   *
# *  license accompanying CSPSA or a proprietary license obtained from   *
# *  ST-Ericsson SA.                                                     *
# *                                                                      *
# *  Author: Sebastian RASMUSSEN <sebastian.rasmussen AT stericsson.com> *
# *                                                                      *
# ************************************************************************/

LOCAL_PATH := $(call my-dir)

# Build nwm2gdf binary executable for the host
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	nwm2gdf.c \
	nwmcommon.c

LOCAL_CFLAGS := -Wall -Wextra -Werror

LOCAL_MODULE := nwm2gdf
LOCAL_MODULE_TAGS := optional

include $(BUILD_HOST_EXECUTABLE)

# Build gdflist binary executable for the host
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	gdflist.c

LOCAL_CFLAGS := -Wall -Wextra -Werror

LOCAL_MODULE := gdflist
LOCAL_MODULE_TAGS := optional

include $(BUILD_HOST_EXECUTABLE)

# Build gdf2pff binary executable for the host
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	gdf2pff.c

LOCAL_CFLAGS := -Wall -Wextra -Werror

LOCAL_MODULE := gdf2pff
LOCAL_MODULE_TAGS := optional

include $(BUILD_HOST_EXECUTABLE)

# Build pff2cspsa binary executable for the host
include $(CLEAR_VARS)

LOCAL_STATIC_LIBRARIES := libcspsa-ll-file libcspsa-core

LOCAL_C_INCLUDES := $(STORAGE_PATH)/parameter_storage/cspsa

LOCAL_SRC_FILES := pff2cspsa.c

LOCAL_CFLAGS := -Wall -Wextra -Werror

LOCAL_MODULE := pff2cspsa
LOCAL_MODULE_TAGS := optional

include $(BUILD_HOST_EXECUTABLE)

# Build cspsalist binary executable for the host
include $(CLEAR_VARS)

LOCAL_STATIC_LIBRARIES := libcspsa-ll-file libcspsa-core

LOCAL_C_INCLUDES := $(STORAGE_PATH)/parameter_storage/cspsa

LOCAL_SRC_FILES := cspsalist.c

LOCAL_CFLAGS := -Wall -Wextra -Werror

LOCAL_MODULE := cspsalist
LOCAL_MODULE_TAGS := optional

include $(BUILD_HOST_EXECUTABLE)

# Build cspsa2nwm binary exectuable for the target
include $(CLEAR_VARS)

LOCAL_CFLAGS += -Wall -Wextra -Werror -DCFG_CSPSA_ANDROID=1

LOCAL_SRC_FILES := cspsa2nwm.c

LOCAL_C_INCLUDES := $(STORAGE_PATH)/parameter_storage/cspsa

LOCAL_SHARED_LIBRARIES := libcspsa

LOCAL_MODULE := cspsa2nwm
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

# Build cspsa2map binary exectuable for the target
include $(CLEAR_VARS)

LOCAL_CFLAGS += -Wall -Wextra -Werror -DCFG_CSPSA_ANDROID=1

LOCAL_SRC_FILES := cspsa2map.c

LOCAL_C_INCLUDES :=$(STORAGE_PATH)/parameter_storage/cspsa

LOCAL_SHARED_LIBRARIES := libcspsa

LOCAL_MODULE := cspsa2map
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

# Build cspsa2q binary exectuable for the target
include $(CLEAR_VARS)

LOCAL_CFLAGS += -Wall -Wextra -Werror -DCFG_CSPSA_ANDROID=1

LOCAL_SRC_FILES := cspsa2q.c

LOCAL_C_INCLUDES :=$(STORAGE_PATH)/parameter_storage/cspsa

LOCAL_SHARED_LIBRARIES := libcspsa

LOCAL_MODULE := cspsa2q
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

# Build map2cspsa binary exectuable for the target
include $(CLEAR_VARS)

LOCAL_CFLAGS += -Wall -Wextra -Werror -DCFG_CSPSA_ANDROID=1

LOCAL_SRC_FILES := map2cspsa.c\
		   qtools_src/getarg.c

LOCAL_C_INCLUDES :=$(STORAGE_PATH)/parameter_storage/cspsa

LOCAL_SHARED_LIBRARIES := libcspsa

LOCAL_MODULE := map2cspsa
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

# Build map2q binary executable for the host
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
		qtools_src/map2q.c\
		qtools_src/getarg.c

LOCAL_C_INCLUDES := $(LOCAL_PATH)/qtools_src

LOCAL_CFLAGS := -Wall -Wextra -Werror -D LINUX

LOCAL_MODULE := map2q
LOCAL_MODULE_TAGS := optional

include $(BUILD_HOST_EXECUTABLE)

# Build map2q binary executable for the target
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
		qtools_src/map2q.c\
		qtools_src/getarg.c

LOCAL_C_INCLUDES := $(LOCAL_PATH)/qtools_src

LOCAL_CFLAGS := -Wall -Wextra -Werror -D LINUX

LOCAL_MODULE := map2q
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

# Build q2map binary executable for the host
include $(CLEAR_VARS)

LOCAL_SRC_FILES := qtools_src/q2map.c

LOCAL_C_INCLUDES := $(LOCAL_PATH)/qtools_src

LOCAL_CFLAGS := -Wall -Wextra -Werror -D LINUX

LOCAL_MODULE := q2map
LOCAL_MODULE_TAGS := optional

include $(BUILD_HOST_EXECUTABLE)

# Build q2map binary executable for the target
include $(CLEAR_VARS)

LOCAL_SRC_FILES := qtools_src/q2map.c

LOCAL_C_INCLUDES := $(LOCAL_PATH)/qtools_src

LOCAL_CFLAGS := -Wall -Wextra -Werror -D LINUX

LOCAL_MODULE := q2map
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

# Build q2cspsa binary executable for the target
include $(CLEAR_VARS)

LOCAL_SRC_FILES := q2cspsa.c

LOCAL_CFLAGS := -Wall -Wextra -Werror -D LINUX

LOCAL_C_INCLUDES :=$(STORAGE_PATH)/parameter_storage/cspsa

LOCAL_SHARED_LIBRARIES := libcspsa

LOCAL_MODULE := q2cspsa
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

# Build nwm2cspsa binary exectuable for the target
include $(CLEAR_VARS)

LOCAL_CFLAGS += -Wall -Wextra -Werror

LOCAL_SRC_FILES := nwm2cspsa.c \
		   nwmcommon.c

LOCAL_C_INCLUDES :=$(STORAGE_PATH)/parameter_storage/cspsa

LOCAL_SHARED_LIBRARIES := libcspsa

LOCAL_MODULE := nwm2cspsa
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

