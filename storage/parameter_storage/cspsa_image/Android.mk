#/************************************************************************
# *                                                                      *
# *  Copyright (C) 2010 ST-Ericsson. All rights reserved.                *
# *  This code is ST-Ericsson proprietary and confidential.              *
# *  Any use of the code for whatever purpose is subject to              *
# *  specific written permission of ST-Ericsson SA.                      *
# *                                                                      *
# *  Author: Mattias Bramsvik <mattias.xx.bramsvik@stericsson.com>       *
# *  Author: Axel Lilliecrantz <axel.lilliecrantz@stericsson.com>        *
# *                                                                      *
# ************************************************************************/


LOCAL_PATH := $(call my-dir)
CSPSA_IMAGE_DIR := $(LOCAL_PATH)
CSPSA_IMAGE_PFF_PATH ?= $(CSPSA_IMAGE_DIR)/pff
CSPSA_IMAGE_PFF_ITP_PATH ?= $(CSPSA_IMAGE_PFF_PATH)_itp
THIS_MODULE_PATH := $(PRODUCT_OUT)

include $(CLEAR_VARS)

LOCAL_MODULE := cspsa_image
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(THIS_MODULE_PATH)

CSPSA_IMAGE_INTERMEDIATES := $(call local-intermediates-dir)
$(shell mkdir -p $(CSPSA_IMAGE_INTERMEDIATES))
$(shell touch $(CSPSA_IMAGE_INTERMEDIATES)/$(LOCAL_MODULE))

CSPSA_IMAGE_MODEMFS_INTERMEDIATES := $(CSPSA_IMAGE_INTERMEDIATES)/nwm/modemfs

include $(BUILD_PREBUILT)

# CSPSA_SET_DEFAULT_CSPSA_IMAGES is set in BoardConfig.mk
# Create the ITP image names, add _itp in the name.
CSPSA_SET_DEFAULT_ITP_CSPSA_IMAGES := $(subst .,_itp., $(CSPSA_SET_DEFAULT_CSPSA_IMAGES))

$(LOCAL_BUILT_MODULE): create-cspsa

.PHONY: create-cspsa
create-cspsa: nwm2gdf gdf2pff pff2cspsa cspsalist
ifeq ($(CSPSA_IMAGE_ENABLE_MODEM_PARAMETERS), true)
	$(call create-cspsa-modem-pff,$(filter %.gdf,$(CSPSA_SET_DEFAULT_CSPSA_IMAGES)))
endif
	$(call create-cspsa-files,$(filter %.bin,$(CSPSA_SET_DEFAULT_CSPSA_IMAGES)),$(filter %.gdf,$(CSPSA_SET_DEFAULT_CSPSA_IMAGES)),$(filter %.bin,$(CSPSA_SET_DEFAULT_ITP_CSPSA_IMAGES)),$(filter %.gdf,$(CSPSA_SET_DEFAULT_ITP_CSPSA_IMAGES)))

# Function to create modem pff files.
# Input parameters
# $1 gdf output file name
# From $1 we get the nwm.gdf file, we string replacement below.
define create-cspsa-modem-pff
	rm -rf $(CSPSA_IMAGE_MODEMFS_INTERMEDIATES)
	mkdir -p $(CSPSA_IMAGE_MODEMFS_INTERMEDIATES)
	mkdir -p $(CSPSA_IMAGE_MODEMFS_INTERMEDIATES)/CONF
	cp -rf $(NWM_MODEMFS_PATH)/CONF/MCE $(CSPSA_IMAGE_MODEMFS_INTERMEDIATES)/CONF/MCE
	mkdir -p $(CSPSA_IMAGE_MODEMFS_INTERMEDIATES)/RFHAL
	nwm2gdf -o $(CSPSA_IMAGE_INTERMEDIATES)/$(1:%.gdf=%_nwm.gdf) $(CSPSA_IMAGE_INTERMEDIATES)/nwm
	mkdir -p $(CSPSA_IMAGE_PFF_PATH)/0x00000000-0x0000FFFF.modem
	rm -f $(CSPSA_IMAGE_PFF_PATH)/0x00000000-0x0000FFFF.modem/*
	gdf2pff -p $(CSPSA_IMAGE_PFF_PATH)/0x00000000-0x0000FFFF.modem -n modem $(CSPSA_IMAGE_INTERMEDIATES)/$(1:%.gdf=%_nwm.gdf)
	cp -rf $(CSPSA_IMAGE_PFF_PATH)/0x00000000-0x0000FFFF.modem $(CSPSA_IMAGE_PFF_ITP_PATH)
endef

# Function to create cspsa images.
# Input parameters
# $1 bin output file name
# $2 gdf output file name
# $3 ITP bin output file name
# $4 ITP gdf output file name
define create-cspsa-files
	pff2cspsa -d -t 0xFFFFE000 -f $(PRODUCT_OUT)/$1 -z 0x100000 $(CSPSA_IMAGE_PFF_PATH)
	cspsalist -o $(PRODUCT_OUT)/$2 $(PRODUCT_OUT)/$1
	pff2cspsa -d -t 0xFFFFE000 -f $(PRODUCT_OUT)/$3 -z 0x100000 $(CSPSA_IMAGE_PFF_ITP_PATH)
	cspsalist -o $(PRODUCT_OUT)/$4 $(PRODUCT_OUT)/$3
	rm -rf $(CSPSA_IMAGE_PFF_PATH)/0x00000000-0x0000FFFF.modem
	rm -rf $(CSPSA_IMAGE_PFF_ITP_PATH)/0x00000000-0x0000FFFF.modem
endef
