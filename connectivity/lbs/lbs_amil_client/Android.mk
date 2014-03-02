##########################################################
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
##########################################################

#lbsamilclient make file


LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

# +++++++++++++++++++++++++++ BEGIN : Platform Specific Config +++++++++++++++++++++++++++

# Enable / Disable below mentioned switches even in the lbs_mal_client\liblcsclient\Android.mk & \lbs_core\lbsd\Android.mk file.
LOCAL_ENABLE_SUPL := true
LOCAL_ENABLE_CP := true





LOCAL_ENABLE_FREQ_AID := true

LOCAL_ENABLE_FTA_AID := true


ifeq ($(LBS_SET_MODEMCLIENT),MAL)
ifeq ($(STE_USE_MOD_APE), 1)
LOCAL_AMIL_LCS_SOLUTION := false
else
LOCAL_AMIL_LCS_SOLUTION := true
endif
else
LOCAL_AMIL_LCS_SOLUTION := false
endif
ifeq ($(LBS_SET_MODEM_CELLINFO_SUPPORT),true)
$(warning "Compiling Modem supporting cell info" )
LOCAL_SET_MODEM_CELLINFO_SUPPORT := true
endif

ifeq ($(LBS_SET_NDK_BUILD),true)
LOCAL_SET_NDK_BUILD := true
endif


# --------------------------- END  :  Platform Specific Config ---------------------------




LOCAL_SHARED_LIBRARIES := libc libphonet libisimessage libshmnetlnk libsms_server libcn

LOCAL_CFLAGS += -DAGPS_LINUX_FTR -DAGPS_GENERIC_SOLUTION_FTR
LOCAL_CFLAGS += -DDEBUG_LOG_LEV2 -DAGPS_ANDR_ECLAIR_FTR -DAGPS_PID0001_FTR

ifeq ($(LOCAL_ENABLE_CP),true)
LOCAL_CFLAGS += -DAGPS_FTR
endif

ifeq ($(LOCAL_ENABLE_SUPL),true)
LOCAL_CFLAGS += -DAGPS_UP_FTR
endif

ifeq ($(LOCAL_ENABLE_FREQ_AID),true)
LOCAL_CFLAGS += -DGPS_FREQ_AID_FTR
endif


$(warning "Compiling LCS_AMIL_client alone" )

ifeq ($(LOCAL_AMIL_LCS_SOLUTION),true)
LOCAL_CFLAGS += -DAMIL_LCS_FTR
ifeq ($(LOCAL_SET_MODEM_CELLINFO_SUPPORT),true)
LOCAL_CFLAGS += -DMODEM_SUPPORT_CELLINFO_FTR
endif

LOCAL_SRC_FILES +=  srce/netClient1SocketHandling.c

ifeq ($(LOCAL_ENABLE_SUPL),true)
LOCAL_SRC_FILES +=  srce/smsClient1SocketHandling.c \
                    srce/smsClient2SmsHandling.c \
                    srce/simClient1SocketHandling.c \
		    		srce/amilMain.c
else
ifeq ($(LOCAL_ENABLE_CP),true)
LOCAL_SRC_FILES +=  srce/amilMain.c
endif
endif

LOCAL_C_INCLUDES += 	$(LOCAL_PATH)/incl \
	$(LOCAL_PATH)/../lbs_core/lbsd/libagpsosa/incl \
	$(LOCAL_PATH)/../lbs_core/lbsd/agpsmain/incl \
	$(LOCAL_PATH)/../lbs_core/lbsd/agpscodec/incl \
	$(LOCAL_PATH)/../lbs_core/lbsd/include	\
	$(LOCAL_PATH)/../lbs_core/lbsd/gns/incl \
	$(LOCAL_PATH)/../lbs_core/lbsd/gnsi/incl \
	$(LOCAL_PATH)/../lbs_core/lbsd/halgps/incl \
	$(LOCAL_PATH)/../lbs_core/lbsd/cgps/incl \
	$(LOCAL_PATH)/../lbs_core/lbsd/lsim/incl \
	$(LOCAL_PATH)/../../../access_services/modem_adaptations/mal/modem_lib/mal/libphonet/include \
	$(LOCAL_PATH)/../../../access_services/modem_adaptations/mal/modem_lib/mal/libshmnetlnk/include \
	$(LOCAL_PATH)/../../../access_services/sms/sms/include \
	$(LOCAL_PATH)/../../../access_services/sim/sim/include \
	$(LOCAL_PATH)/../../../access_services/call_network/call_network/libcn/include

ifeq ($(LOCAL_SET_NDK_BUILD),true)
LOCAL_C_INCLUDES +=      \
        $(LOCAL_PATH)/../lbs_external/mil_lib/libcn/include    \
        $(LOCAL_PATH)/../lbs_external/mil_lib/libphonet/include    \
        $(LOCAL_PATH)/../lbs_external/mil_lib/libshmnetlnk/include    \
        $(LOCAL_PATH)/../lbs_external/mil_lib/sim/include    \
        $(LOCAL_PATH)/../lbs_external/mil_lib/sms/include
endif

endif


LOCAL_STATIC_LIBRARIES := liblog


LOCAL_PRELINK_MODULE := false
LOCAL_MODULE:= libamilclient
LOCAL_MODULE_TAGS := optional
include $(BUILD_STATIC_LIBRARY)


