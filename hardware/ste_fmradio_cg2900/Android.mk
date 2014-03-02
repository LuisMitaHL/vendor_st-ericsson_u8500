##############################################################################
#  (c) ST-Ericsson, 2010 - All rights reserved
#  Reproduction and Communication of this document is strictly prohibited
#  unless specifically authorized in writing by ST-Ericsson
#
#  \brief   fmradio support for cg2900 chip
#           make libfmradio.cg2900(_tx/_rx) for a quick build
#
#  \author  ST-Ericsson
##############################################################################

ifeq ($(FMRADIO_CG2900_ENABLE_FEATURE_VENDOR_DRIVE), true)

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	ste_fmradio_cg2900.c

ifneq ($(FMRADIO_CG2900_SET_TX_ONLY), true)
  LOCAL_SRC_FILES += ste_fmradio_cg2900_rds_parser.c
endif


LOCAL_SHARED_LIBRARIES := \
	libcutils libutils libril libnetutils

LOCAL_C_INCLUDES := $(KERNEL_HEADERS) $(TOP)/hardware/libhardware/include/hardware $(TOP)/vendor/st-ericsson/hardware/ste_fmradio_cg2900/

LOCAL_CFLAGS += -iquote $(TOP)/kernel/include

ifeq ($(FMRADIO_CG2900_SET_RX_ONLY), true)
  LOCAL_CFLAGS += -DFMRADIO_CG2900_SET_RX_ONLY
  LOCAL_MODULE:= libfmradio.cg2900_rx
else
ifeq ($(FMRADIO_CG2900_SET_TX_ONLY), true)
  LOCAL_CFLAGS += -DFMRADIO_CG2900_SET_TX_ONLY
  LOCAL_MODULE:= libfmradio.cg2900_tx
else
  LOCAL_MODULE:= libfmradio.cg2900
endif
endif

LOCAL_MODULE_TAGS := optional

# Disable prelink, or add to build/core/prelink-linux-arm.map
LOCAL_PRELINK_MODULE := false

# Build shared library
LOCAL_SHARED_LIBRARIES += \
	libcutils libutils

include $(BUILD_SHARED_LIBRARY)

endif

