#
# Copyright (C) ST-Ericsson SA 2012. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

ifeq ($(ADM_BUILD_PROD_SPEC),1)
LOCAL_MODULE           := adm.sqlite
else
LOCAL_MODULE           := adm.sqlite-$(ADM_DB_VARIANT)
endif
LOCAL_MODULE_TAGS      := optional
LOCAL_MODULE_CLASS     := ETC
LOCAL_SRC_FILES        := adm.sqlite-tmp-$(ADM_DB_VARIANT)
LOCAL_MODULE_PATH      := $(TARGET_OUT_ETC)
ADM_DB_INTERMEDIATES   := $(call local-intermediates-dir)
ADM_PATH               := $(abspath $(LOCAL_PATH))
LOCAL_PATH             := $(ADM_DB_INTERMEDIATES)
ADM_DB_PATH            := $(ADM_PATH)/db/$(ADM_DB_VARIANT)
STE_ADM_GEN_DB         := ste-adm-gen-db-$(ADM_DB_VARIANT)

ifeq ($(ADM_NO_EFFECT_CONFIG_SUPPORT), 1)
  LOCAL_CFLAGS += -DADM_NO_EFFECT_CONFIG_SUPPORT
endif

ifeq ($(MULTIMEDIA_SET_PLATFORM),u8500)
  LOCAL_CFLAGS += -DSTE_PLATFORM_U8500
endif

ifeq ($(MULTIMEDIA_SET_PLATFORM),u9540)
  LOCAL_CFLAGS += -DSTE_PLATFORM_U8500
endif

ifeq ($(MULTIMEDIA_SET_PLATFORM),u9540)
APF_FILE_LIST=$(wildcard $(ADM_PATH)/db/u9540_ab9540_v1/apf/*.apf)
else
APF_FILE_LIST=
endif

ifeq ($(MULTIMEDIA_SET_PLATFORM),l8540)
APF_FILE_LIST=$(wildcard $(ADM_PATH)/db/l8540_ab8540_v1/apf/*.apf)
else
APF_FILE_LIST=
endif


$(ADM_DB_INTERMEDIATES)/adm.sqlite-tmp-$(ADM_DB_VARIANT) : ADM_DB_VARIANT := $(ADM_DB_VARIANT)
$(ADM_DB_INTERMEDIATES)/adm.sqlite-tmp-$(ADM_DB_VARIANT) : ADM_PATH := $(ADM_PATH)
$(ADM_DB_INTERMEDIATES)/adm.sqlite-tmp-$(ADM_DB_VARIANT) : ADM_DB_PATH := $(ADM_DB_PATH)
$(ADM_DB_INTERMEDIATES)/adm.sqlite-tmp-$(ADM_DB_VARIANT) : STE_ADM_GEN_DB := $(STE_ADM_GEN_DB)
$(ADM_DB_INTERMEDIATES)/adm.sqlite-tmp-$(ADM_DB_VARIANT) : ADM_DB_INTERMEDIATES := $(ADM_DB_INTERMEDIATES)
$(ADM_DB_INTERMEDIATES)/adm.sqlite-tmp-$(ADM_DB_VARIANT) :  $(ADM_PATH)/db/generate_db.py $(ADM_DB_PATH)/*.txt $(ADM_DB_PATH)/*.state $(HOST_OUT_EXECUTABLES)/$(STE_ADM_GEN_DB) $(APF_FILE_LIST)
	@mkdir -p $(ADM_DB_INTERMEDIATES)
	@echo ADM DB: $(ADM_DB_VARIANT) - Generating IL struct descriptions
	$(HOST_OUT_EXECUTABLES)/$(STE_ADM_GEN_DB) > $(ADM_DB_INTERMEDIATES)/il_cfgdef.txt
	@echo ADM DB: $(ADM_DB_VARIANT) - Generating ADM database
	$(hide) $(ADM_PATH)/db/generate_db.py $(ADM_DB_INTERMEDIATES) $(ADM_DB_PATH) $(ADM_DB_VARIANT)
	@echo -------------------------
ifeq ($(MULTIMEDIA_SET_PLATFORM),u9540)
ifeq ($(ADM_DB_VARIANT),u9540_ab9540_v1)
	@echo Install apf files
	@mkdir -p $(ANDROID_PRODUCT_OUT)/system/etc/apf
	@cp -f $(ADM_DB_PATH)/apf/*.apf $(ANDROID_PRODUCT_OUT)/system/etc/apf/
endif
ifeq ($(ADM_DB_VARIANT),u9540_ab9540_v2)
	@echo Install apf files
	@mkdir -p $(ANDROID_PRODUCT_OUT)/system/etc/apf
	@cp -f $(ADM_DB_PATH)/apf/*.apf $(ANDROID_PRODUCT_OUT)/system/etc/apf/
endif
ifeq ($(ADM_DB_VARIANT),u9540_ab9540_v3)
	@echo Install apf files
	@mkdir -p $(ANDROID_PRODUCT_OUT)/system/etc/apf
	@cp -f $(ADM_DB_PATH)/apf/*.apf $(ANDROID_PRODUCT_OUT)/system/etc/apf/
endif
endif
ifeq ($(MULTIMEDIA_SET_PLATFORM),l8540)
ifeq ($(ADM_DB_VARIANT),l8540_ab8540_v1)
	@echo Install apf files
	@mkdir -p $(ANDROID_PRODUCT_OUT)/system/etc/apf
	@cp -f $(ADM_DB_PATH)/apf/*.apf $(ANDROID_PRODUCT_OUT)/system/etc/apf/
endif
endif
include $(BUILD_PREBUILT)
