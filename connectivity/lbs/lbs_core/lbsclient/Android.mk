##########################################################
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
##########################################################
LOCAL_PATH:= $(call my-dir)

ifeq ($(LBS_SET_NDK_BUILD),true)
LOCAL_SET_NDK_BUILD := true
endif
ifeq ($(LBS_SET_MODEMCLIENT),CAIF)
LOCAL_SET_4500_BUILD := true
endif

ifeq ($(LBS_SET_MODEM_CELLINFO_SUPPORT),true)
$(warning "Compiling Modem supporting cell info" )
LOCAL_SET_MODEM_CELLINFO_SUPPORT := true
endif

LOCAL_SET_MODEM_WIRELESSINFO_SUPPORT := false

include $(CLEAR_VARS)
LOCAL_ENABLE_TLS_STUB := false
ANDROID_VER_GINGERBREAD := true
LOCAL_ENABLE_SUPL := true
LOCAL_ENABLE_SBEE := false

# --------------------------- END  :  Platform Specific Config ---------------------------


LOCAL_CFLAGS += -DAGPS_LINUX_FTR -UAGPS_AUTO_ACCECPT_NOTIFICATION
LOCAL_CFLAGS += -DDEBUG_LOG_LEV2 -DAGPS_ANDR_ECLAIR_FTR -DAGPS_PID0001_FTR -UGPSCLIENT_FORCE_IMSI -DMETRICO_TEST_TOOL
LOCAL_CFLAGS += -UMULTIPLE_PDP_FOR_ATT_TEMP_FTR -UAGPS_DISABLE_TLS_CA_CERT_VERIFY

ifeq ($(ANDROID_VER_GINGERBREAD),true)
LOCAL_CFLAGS += -DAGPS_ANDR_GINGER_FTR
endif

ifeq ($(LOCAL_ENABLE_SBEE),true)
LOCAL_CFLAGS += -DAGPS_SBEE_FTR
endif

ifeq ($(LOCAL_ENABLE_SUPL),true)
$(warning "Compiling with SUPL Enabled" )
LOCAL_CFLAGS += -DAGPS_UP_FTR
endif

ifeq ($(LOCAL_SET_MODEM_CELLINFO_SUPPORT),true)
LOCAL_CFLAGS += -DMODEM_SUPPORT_CELLINFO_FTR
endif

ifeq ($(LOCAL_SET_MODEM_WIRELESSINFO_SUPPORT),true)
LOCAL_CFLAGS += -DMODEM_SUPPORT_WIRELESSINFO_FTR
endif

LOCAL_SRC_FILES += srce/clientutils.c \
		srce/gpsclient.c \
		srce/sbeeclient.c \
		srce/HttpHandler.c \
		srce/BearerHandler.c \
		srce/eeclient.c
ifeq ($(LOCAL_ENABLE_TLS_STUB),true)
LOCAL_SRC_FILES += srce/TlsHandler_stub.c
LOCAL_CFLAGS += -DAGPS_UNIT_TEST_FTR
else
LOCAL_SRC_FILES += srce/TlsHandler.c
endif

LOCAL_SRC_FILES += ../lbsd/agpscodec/srce/agpscodec.c

LOCAL_AGPS_PATH := hardware

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../lbsd/include \
	$(LOCAL_PATH)/../lbsd/agpscodec/incl \
	$(LOCAL_PATH)/../lbsd/libagpsosa/incl \
	$(LOCAL_PATH)/incl \
	$(LOCAL_PATH)/../include \
	external/openssl/include
ifeq ($(LOCAL_SET_NDK_BUILD),true)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../lbs_external/openssl/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../lbs_external/utils_lib
LOCAL_LDLIBS += -llog
endif

LOCAL_STATIC_LIBRARIES := liblog
LOCAL_SHARED_LIBRARIES := libutils libssl libcrypto libz

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE:= libclientgps
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)


