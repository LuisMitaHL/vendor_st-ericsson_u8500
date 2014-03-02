#/*
# * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# * This code is ST-Ericsson proprietary and confidential.
# * Any use of the code for whatever purpose is subject to
# * specific written permission of ST-Ericsson SA.
# */
ifeq ($(SMS_ENABLE_FEATURE_SMS),true)
LOCAL_PATH:= $(call my-dir)
SMS := $(LOCAL_PATH)

############################################################################
# Configure build options for SMS Server
############################################################################

ifeq ($(SMS_SET_FEATURE_BACKEND),MAL)
$(info SMS: Building for MAL backend)
else
ifeq ($(SMS_SET_FEATURE_BACKEND),MFL)
$(info SMS: Building for MFL backend)
else
$(error SMS: No backend selected!)
endif
endif

# Enable support for SMS Storage Layer. This provides support for storage of SMS
# messages in the SIM and in the ME.
CFG_ENABLE_SMS_STORAGE_LAYER:=true

# Enable flag CFG_ENABLE_EXTRA_SMS_LOGGING if medium debugging is wanted in SMS server.
# (Adds SMS_B_ prints)
CFG_ENABLE_EXTRA_SMS_LOGGING:=false

# Enable flag CFG_ENABLE_VERBOSE_SMS_LOGGING if maximum debugging is wanted in SMS server.
# (Adds SMS_B_ and SMS_C_ prints)
CFG_ENABLE_VERBOSE_SMS_LOGGING:=false


# Edit the CFG_ENABLE_SMS_TEST_HARNESS flag to be "true" if you want the test harness
CFG_ENABLE_SMS_TEST_HARNESS:=false

# Edit the CFG_ENABLE_SMS_TEST_HARNESS_STUB flag to be "true" if you want the test harness stub
# module impl to replace MAL
CFG_ENABLE_SMS_TEST_HARNESS_STUB:=false

# It should not be possible to enable the test harness stubbed MAL without the test harness configured
ifeq ($(CFG_ENABLE_SMS_TEST_HARNESS), false)
ifeq ($(CFG_ENABLE_SMS_TEST_HARNESS_STUB), true)
CFG_ENABLE_SMS_TEST_HARNESS_STUB:=false
endif
endif

# Enable the CFG_ENABLE_MEMORY_CHECK flag to be true if you want the sms_server to track its
# memory allocation and print its status to the log at the beginning and end of the test harness
# run.
CFG_ENABLE_MEMORY_CHECK:=false



############################################################################
# Build SMS Server API Library (libsms_server)
############################################################################
include $(CLEAR_VARS)

LOCAL_MODULE := libsms_server
LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS := -DPLATFORM_ANDROID -DSMS_PRINT_A_ -DCFG_USE_ANDROID_LOG

ifeq ($(CFG_ENABLE_EXTRA_SMS_LOGGING),true)
LOCAL_CFLAGS += -DSMS_PRINT_B_
endif

ifeq ($(CFG_ENABLE_VERBOSE_SMS_LOGGING),true)
LOCAL_CFLAGS += -DSMS_PRINT_B_ -DSMS_PRINT_C_
endif


ifeq ($(CFG_ENABLE_SMS_TEST_HARNESS),true)
# Using the test harness. Enable a simulation of a CAT Refresh event.
LOCAL_CFLAGS += -DSMS_TEST_HARNESS_CAT_REFRESH_SIMULATION_ENABLED
ifeq ($(CFG_ENABLE_MEMORY_CHECK),true)
# Use the SMS_TEST_HARNESS_MEMORY_CHECK_ENABLED compile switch to enable an extra internal signal
# that the test harness can send to the SMS Server to get the current memory allocation status.
LOCAL_CFLAGS += -DSMS_TEST_HARNESS_MEMORY_CHECK_ENABLED
endif
endif


LOCAL_SRC_FILES := \
    internal/main/src/r_sms.c       \
    internal/main/src/r_cbs.c       \
    internal/porting/src/smsipc.c     \
    internal/porting/src/smslinuxporting.c \
    internal/main/src/sms_log_client.c

LOCAL_PRELINK_MODULE:= false

LOCAL_SHARED_LIBRARIES := libutils

LOCAL_STATIC_LIBRARIES := libaccsutil_security

LOCAL_C_INCLUDES := $(SMS)/internal/main/include    \
  $(SMS)/include                  \
  $(SMS)/internal/porting/include \
  $(ACCESS_SERVICES_PATH)/common/common_functionality/include

LOCAL_SHARED_LIBRARIES := libutils

include $(BUILD_SHARED_LIBRARY)



############################################################################
# Build SMS Server (sms_server)
############################################################################
include $(CLEAR_VARS)

LOCAL_MODULE := sms_server
LOCAL_MODULE_TAGS := optional

ifneq ($(CFG_ENABLE_SMS_TEST_HARNESS),true)
# Not using test harness so setup modem include file path based on which modem is being used.

ifeq ($(SMS_SET_FEATURE_BACKEND), MAL)
SMS_MODEM_INCLUDE_PATH := $(ACCESS_SERVICES_PATH)/modem_adaptations/mal/modem_lib/mal/libmalsms/include
SMS_SHMNETLNK_INCLUDE_PATH := $(ACCESS_SERVICES_PATH)/modem_adaptations/mal/modem_lib/mal/libshmnetlnk/include
endif
ifeq ($(SMS_SET_FEATURE_BACKEND), MFL)
SMS_MODEM_INCLUDE_PATH := $(ACCESS_SERVICES_PATH)/mfl_framework/include \
  $(ACCESS_SERVICES_PATH)/mfl_proxy_cas/include
endif

else # TEST_HARNESS

# Test harness being used to setup modem include file path to use stubs.
SMS_MODEM_INCLUDE_PATH := $(SMS)/test_harness/common/include
ifeq ($(CFG_ENABLE_SMS_TEST_HARNESS_STUB),true)
# Using stubs for SMS Modem
ifeq ($(SMS_SET_FEATURE_BACKEND), MAL)
SMS_MODEM_INCLUDE_PATH += $(SMS)/test_harness/stubs/mal/include \
  $(ACCESS_SERVICES_PATH)/modem_adaptations/mal/modem_lib/mal/libmalsms/include \
  $(ACCESS_SERVICES_PATH)/modem_adaptations/mal/modem_lib/mal/libshmnetlnk/include
endif
ifeq ($(SMS_SET_FEATURE_BACKEND), MFL)
SMS_MODEM_INCLUDE_PATH += $(SMS)/test_harness/stubs/mfl/include
endif
else # else NOT STUB
# NOT using stubs for SMS Modem
ifeq ($(SMS_SET_FEATURE_BACKEND), MAL)
SMS_MODEM_INCLUDE_PATH := $(ACCESS_SERVICES_PATH)/modem_adaptations/mal/modem_lib/mal/libmalsms/include \
  $(ACCESS_SERVICES_PATH)/modem_adaptations/mal/modem_lib/mal/libshmnetlnk/include
endif
ifeq ($(SMS_SET_FEATURE_BACKEND), MFL)
SMS_MODEM_INCLUDE_PATH := $(ACCESS_SERVICES_PATH)/mfl_framework/include \
  $(ACCESS_SERVICES_PATH)/mfl_proxy_cas/include
endif
endif # STUB
endif # TEST_HARNESS

LOCAL_CFLAGS := -DPLATFORM_ANDROID -DSMS_PRINT_A_ -DCFG_USE_ANDROID_LOG

ifeq ($(CFG_ENABLE_EXTRA_SMS_LOGGING),true)
LOCAL_CFLAGS += -DSMS_PRINT_B_
endif

ifeq ($(CFG_ENABLE_VERBOSE_SMS_LOGGING),true)
LOCAL_CFLAGS += -DSMS_PRINT_B_ -DSMS_PRINT_C_
endif

# Configure default storage mount point. Used for persistent parameter data storage.
LOCAL_CFLAGS += -DSMS_FS_DEFAULT_TOP_MOUNT_POINT=\"/data/app-private\"

ifneq ($(CFG_ENABLE_SMS_STORAGE_LAYER),true)
# Storage layer not enabled so add flag to remove support from compiled files.
LOCAL_CFLAGS += -DREMOVE_SMSTRG_LAYER
endif

ifeq ($(SMS_SET_FEATURE_BACKEND), MFL)
# Using MFL mode so enable the associated build flag. (Code defaults to
# MAL modem.)
LOCAL_CFLAGS += -DSMS_USE_MFL_MODEM

# Current version of code does not have necessary MFL modem NS Event IDs to subscribe to NS events.
# Therefore disable the code, in smbearer.c, until it is fixed.
# *********** NOTE THAT IT IS IMPOSSIBLE TO RECEIVE MT SMS MESSAGES UNTIL THIS IS FIXED ***********
LOCAL_CFLAGS += -DSMS_TEMPORARILY_REMOVE_NS_EVENT_SUBSCRIPTION
endif

ifeq ($(CFG_ENABLE_MEMORY_CHECK),true)
# Use the heap check versions of the malloc macros. This can only be done for the sms_server
# so the SMS_HEAP_CHECK compiler switch must not be enabled for any other part of the SMS
# build; if it is, linker errors will occur.
LOCAL_CFLAGS += -DSMS_HEAP_CHECK
ifeq ($(CFG_ENABLE_SMS_TEST_HARNESS),true)
# Use the SMS_TEST_HARNESS_MEMORY_CHECK_ENABLED compile switch to enable an extra internal signal
# that the test harness can send to the SMS Server to get a current memory allocation status.
LOCAL_CFLAGS += -DSMS_TEST_HARNESS_MEMORY_CHECK_ENABLED
endif
endif

ifeq ($(CFG_ENABLE_SMS_TEST_HARNESS),true)
# Using the test harness. Enable a simulation of a CAT Refresh event.
LOCAL_CFLAGS += -DSMS_TEST_HARNESS_CAT_REFRESH_SIMULATION_ENABLED
endif

ifeq ($(SMS_ENABLE_FEATURE_MO_ROUTE),CS_ONLY)
#Enable SMS over CS only.
LOCAL_CFLAGS += -DSMS_ENABLE_FEATURE_ROUTE_CS_ONLY
else
ifeq ($(SMS_ENABLE_FEATURE_MO_ROUTE),PS_ONLY)
#Enable SMS over PS only.
LOCAL_CFLAGS += -DSMS_ENABLE_FEATURE_ROUTE_PS_ONLY
endif
endif

LOCAL_SRC_FILES := \
    internal/main/src/smsmain.c     \
    internal/main/src/smsevnthndr.c \
    internal/main/src/smslist.c     \
    internal/sm/src/sm.c            \
    internal/sm/src/smbearer.c      \
    internal/sm/src/smmngr.c        \
    internal/sm/src/smotadp.c       \
    internal/sm/src/smrouter.c      \
    internal/sm/src/smsimtkt.c      \
    internal/sm/src/smstrg.c        \
    internal/sm/src/smtpdu.c        \
    internal/sm/src/smutil.c        \
    internal/cb/src/cbmngr.c        \
    internal/cb/src/cbmsgs.c        \
    internal/cb/src/cbsubscribe.c   \
    internal/porting/src/smstimerserver.c \
    internal/main/src/sms_log_client.c

ifneq ($(CFG_ENABLE_SMS_TEST_HARNESS),true)
LOCAL_SRC_FILES += config/src/c_sms_config.c
else
# The test harness is enabled use a special version of c_sms_config.c with test harness specific settings.
LOCAL_SRC_FILES += test_harness/config/c_sms_config_test.c
ifeq ($(CFG_ENABLE_SMS_TEST_HARNESS_STUB),true)
ifeq ($(SMS_SET_FEATURE_BACKEND), MAL)
# Stub files MAL mode.
LOCAL_SRC_FILES += test_harness/stubs/mal/src/client_side_mal_sms.c
else
# Stub files MFL mode.
LOCAL_SRC_FILES += test_harness/stubs/mfl/src/client_side_mfl_framework_stub.c \
                   test_harness/stubs/mfl/src/mfl_cas_stub.c
endif
endif # STUB
endif # TEST_HARNESS


LOCAL_PRELINK_MODULE:= false

LOCAL_SHARED_LIBRARIES := libcutils libsms_server libcops libcn
LOCAL_STATIC_LIBRARIES := libaccsutil_security

ifeq ($(CFG_ENABLE_SMS_TEST_HARNESS),false)
# Not using test harness so include modem libraries. (Test harness uses stubs.)
ifeq ($(SMS_SET_FEATURE_BACKEND), MAL)
LOCAL_SHARED_LIBRARIES += libmalsms libshmnetlnk
endif
ifeq ($(SMS_SET_FEATURE_BACKEND), MFL)
LOCAL_SHARED_LIBRARIES += libmfl libmfl_cas
endif
else
ifeq ($(CFG_ENABLE_SMS_TEST_HARNESS_STUB),false)
# Not using test harness so include modem libraries. (Test harness uses stubs.)
ifeq ($(SMS_SET_FEATURE_BACKEND), MAL)
LOCAL_SHARED_LIBRARIES += libmalsms libshmnetlnk
endif
ifeq ($(SMS_SET_FEATURE_BACKEND), MFL)
LOCAL_SHARED_LIBRARIES += libmfl libmfl_cas
endif
endif # STUB
endif # TEST_HARNESS


LOCAL_C_INCLUDES := $(SMS)/internal/main/include    \
  $(SMS)/include                  \
  $(SMS)/config/include           \
  $(SMS)/internal/porting/include \
  $(SMS)/internal/sm/include      \
  $(SMS)/internal/cb/include      \
  $(SMS_MODEM_INCLUDE_PATH)       \
  $(ACCESS_SERVICES_PATH)/sim/sim/include \
  $(ACCESS_SERVICES_PATH)/call_network/call_network/libcn/include \
  $(PROCESSING_PATH)/security_framework/cops/cops-api/include \
  $(SMS_SHMNETLNK_INCLUDE_PATH) \
  $(ACCESS_SERVICES_PATH)/common/common_functionality/include

LOCAL_STATIC_LIBRARIES += libsim libsimcom

include $(BUILD_EXECUTABLE)




ifeq ($(CFG_ENABLE_SMS_TEST_HARNESS),true)
############################################################################
# Build SMS Server Test Harness (sms_test_harness)
############################################################################
include $(CLEAR_VARS)

LOCAL_CFLAGS := -DPLATFORM_ANDROID -DSMS_PRINT_A_ -DCFG_USE_ANDROID_LOG

ifneq ($(CFG_ENABLE_SMS_STORAGE_LAYER),true)
# Storage layer not enabled so add flag to remove support from compiled files.
LOCAL_CFLAGS += -DREMOVE_SMSTRG_LAYER
endif

ifeq ($(CFG_ENABLE_EXTRA_SMS_LOGGING),true)
LOCAL_CFLAGS += -DSMS_PRINT_B_
endif

ifeq ($(CFG_ENABLE_VERBOSE_SMS_LOGGING),true)
LOCAL_CFLAGS += -DSMS_PRINT_B_ -DSMS_PRINT_C_
endif

ifeq ($(CFG_ENABLE_MEMORY_CHECK),true)
# Use the SMS_TEST_HARNESS_MEMORY_CHECK_ENABLED compile switch to enable an extra internal signal
# that the test harness can send to the SMS Server to get a current memory allocation status.
LOCAL_CFLAGS += -DSMS_TEST_HARNESS_MEMORY_CHECK_ENABLED
endif

# Using the test harness. Enable a simulation of a CAT Refresh event.
LOCAL_CFLAGS += -DSMS_TEST_HARNESS_CAT_REFRESH_SIMULATION_ENABLED


LOCAL_MODULE := sms_test_harness
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := test_harness/stand_alone/src/smstest.c \
                   test_harness/common/src/test_cases.c \
                   test_harness/config/c_sms_config_test.c

LOCAL_SHARED_LIBRARIES := libsms_server

LOCAL_C_INCLUDES := $(SMS)/internal/main/include    \
  $(SMS)/include                  \
  $(SMS)/internal/porting/include \
  $(SMS)/config/include           \
  $(SMS)/test_harness/common/include \
  $(ACCESS_SERVICES_PATH)/sim/sim/include \
  $(ACCESS_SERVICES_PATH)/common/common_functionality/include

ifeq ($(CFG_ENABLE_SMS_TEST_HARNESS_STUB),true)
ifeq ($(SMS_SET_FEATURE_BACKEND), MAL)
LOCAL_C_INCLUDES += $(SMS)/test_harness/stubs/mal/include
endif
ifeq ($(SMS_SET_FEATURE_BACKEND), MFL)
LOCAL_C_INCLUDES += $(SMS)/test_harness/stubs/mfl/include
endif
endif # STUB

LOCAL_PRELINK_MODULE:= false

LOCAL_STATIC_LIBRARIES += libsim libsimcom

include $(BUILD_EXECUTABLE)

ifeq ($(CFG_ENABLE_SMS_TEST_HARNESS_STUB),true)
############################################################################
# Build SMS Server Test Harness Modem Stubs (sms_stub)
############################################################################
include $(CLEAR_VARS)

LOCAL_CFLAGS := -DPLATFORM_ANDROID -DSMS_PRINT_A_ -DCFG_USE_ANDROID_LOG

ifneq ($(CFG_ENABLE_SMS_STORAGE_LAYER),true)
# Storage layer not enabled so add flag to remove support from compiled files.
LOCAL_CFLAGS += -DREMOVE_SMSTRG_LAYER
endif

ifeq ($(CFG_ENABLE_EXTRA_SMS_LOGGING),true)
LOCAL_CFLAGS += -DSMS_PRINT_B_
endif

ifeq ($(CFG_ENABLE_VERBOSE_SMS_LOGGING),true)
LOCAL_CFLAGS += -DSMS_PRINT_B_ -DSMS_PRINT_C_
endif

LOCAL_MODULE := sms_stub
LOCAL_MODULE_TAGS := optional

ifeq ($(SMS_SET_FEATURE_BACKEND), MAL)
LOCAL_SRC_FILES := test_harness/stubs/mal/src/stub_side_mal_sms.c
endif
ifeq ($(SMS_SET_FEATURE_BACKEND), MFL)
LOCAL_SRC_FILES := test_harness/stubs/mfl/src/stub_side_mfl_framework_stub.c
endif

LOCAL_SHARED_LIBRARIES := libsms_server

LOCAL_C_INCLUDES := $(SMS)/internal/main/include    \
  $(SMS)/include                  \
  $(SMS)/internal/porting/include \
  $(SMS)/test_harness/common/include

ifeq ($(SMS_SET_FEATURE_BACKEND), MAL)
LOCAL_C_INCLUDES += $(SMS)/test_harness/stubs/mal/include \
  $(ACCESS_SERVICES_PATH)/modem_adaptations/mal/modem_lib/mal/libmalsms/include
endif
ifeq ($(SMS_SET_FEATURE_BACKEND), MFL)
LOCAL_C_INCLUDES += $(SMS)/test_harness/stubs/mfl/include
endif

LOCAL_PRELINK_MODULE:= false

include $(BUILD_EXECUTABLE)


endif # STUB
endif


############################################################################
# Use the following to include anything else.
############################################################################
include $(call all-makefiles-under,$(LOCAL_PATH))
endif
