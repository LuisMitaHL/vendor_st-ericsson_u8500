######################### -*- Mode: Makefile-Gmake -*- ########################
## Copyright (C) 2010, ST-Ericsson
##
## File name       : Android.mk
## Description     : mhmcel test client
##
## Author          : Stefan Svenberg <stefan.svenberg@stericsson.com>
##
###############################################################################

include $(CLEAR_VARS)

LOCAL_MODULE := mhmcel
LOCAL_PRELINK_MODULE := false

LOCAL_C_INCLUDES := \
        $(MODEM_DIR)/libmaluicc/include \
        $(MODEM_DIR)/libmalmce/include \
        $(MODEM_DIR)/mal_util/include \
        $(SIM_DIR)/include \
        $(SIM_DIR)/libsimcom \
        $(SIM_DIR)/libapdu \
        $(SIM_DIR)/cat \
        $(SIM_DIR)/simd \
        $(SIM_DIR)/libsim \
        $(SIM_DIR)/uiccd/include

LOCAL_SRC_FILES := \
  mh_mce.c \
  ../sim_malmce_rsp_trace.c \
  ../sim_maluicc_req_trace.c \
  ../sim_maluicc_rsp_trace.c

LOCAL_CFLAGS :=-DSIM_INIT_FULL

LOCAL_SHARED_LIBRARIES := libmalmce

LOCAL_STATIC_LIBRARIES := libsim libsimcom

include $(BUILD_EXECUTABLE)
