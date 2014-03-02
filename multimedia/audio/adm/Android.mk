#
# Copyright (C) ST-Ericsson SA 2012. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

LOCAL_PATH := $(call my-dir)

################################################################################
#
#  Select db to build
#
ADM_PATH := $(abspath $(LOCAL_PATH))


#### Use product specific DB if one exists
PRODUCT_DB_PATH := $(wildcard $(ADM_PATH)/db/$(TARGET_PRODUCT))

ifneq ($(strip $(PRODUCT_DB_PATH)),)
    ADM_BUILD_PROD_SPEC := 1
else

#### else use platform default DB.
ifeq ($(ADM_SET_AUDIO_CHIP),u8500_ab8500)
    ADM_BUILD_U8500_AB8500 := 1
endif
ifeq ($(ADM_SET_AUDIO_CHIP),u8520_ab8505_v1)
    ADM_BUILD_U8500_AB8505_V1 := 1
endif
ifeq ($(ADM_SET_AUDIO_CHIP),u8520_ab8505_v2)
    ADM_BUILD_U8520_AB8505_V2 := 1
endif
ifeq ($(ADM_SET_AUDIO_CHIP),u9540_ab9540_v1)
    ADM_BUILD_U9540_AB9540_V1 := 1
endif
ifeq ($(ADM_SET_AUDIO_CHIP),u9540_ab9540_v2)
    ADM_BUILD_U9540_AB9540_V2 := 1
endif
ifeq ($(ADM_SET_AUDIO_CHIP),u9540_ab8540_v1)
    ADM_BUILD_U9540_AB8540_V1 := 1
endif

# To be uncommented when AB9540_V3 is available
#ifeq ($(ADM_SET_AUDIO_CHIP),u9540_ab9540_v3)
#    ADM_BUILD_U9540_AB9540_V3 := 1
#endif
ifeq ($(ADM_SET_AUDIO_CHIP),)
    ADM_BUILD_U8500_AB8500 := 1
    ADM_BUILD_U8500_AB8505_V1 := 1
    ADM_BUILD_U8520_AB8505_V2 := 1
    ADM_BUILD_U9540_AB9540_V1 := 1
    ADM_BUILD_U9540_AB9540_V2 := 1
    ADM_BUILD_U9540_AB8540_V1 := 1
#    ADM_BUILD_U9540_AB9540_V3 := 1 # To be uncommented when AB9540_V3 is available
endif

endif

################################################################################
#
# libste_adm.so
#

include $(CLEAR_VARS)

LOCAL_SRC_FILES :=  \
        src/api/cscall/ste_adm_api_cscall.c \
        src/api/cscall/ste_adm_cscall_omx.c \
        src/api/cscall/ste_adm_api_modem.c \
        src/api/ste_adm_api_d2d.c \
        src/api/ste_adm_api_devset.c \
        src/api/ste_adm_api_util.c \
        src/db/ste_adm_db.c \
        src/db/ste_adm_db_remap_check.c \
        src/db/ste_adm_db_cfg.c \
        src/db/ste_adm_db_test.c \
        src/dev/ste_adm_dev.c \
        src/dev/ste_adm_dev_omx.c \
        src/dev/ste_adm_dev_omx_graph.c \
        src/dev/ste_adm_dev_omx_util.c \
        src/il_tool/ste_adm_omx_io.c \
        src/il_tool/ste_adm_omx_log.c \
        src/il_tool/ste_adm_omx_tool.c \
        src/il_tool/ste_adm_omx_core.c \
        src/il_tool/ste_adm_platform_adaptions.c \
        src/ste_adm_client.c \
        src/ste_adm_dbg.c \
        src/ste_adm_srv.c \
        src/ste_adm_main.c \
        src/ste_adm_util.c \
        src/api/ste_adm_api_tg.cpp

LOCAL_CFLAGS += -fvisibility=hidden
LOCAL_CFLAGS += -D_POSIX_C_SOURCE
LOCAL_CFLAGS += -O3 -Wall -funwind-tables
LOCAL_CFLAGS += -DADM_SOCKET_UID=1005
LOCAL_CFLAGS += -DADM_SOCKET_GID=1005
LOCAL_CFLAGS += -DADM_SOCKET_MODE=0660
LOCAL_CFLAGS += -DADM_LOG_WARNINGS -DADM_LOG_ERRORS -DADM_LOG_STATUS
LOCAL_CFLAGS += -DADM_SRC_MODE -DSRC_MODE_INTERNAL

ifeq ($(ADM_DISABLE_FEATURE_CSCALL),true)
LOCAL_CFLAGS += -DADM_EXCLUDE_CSCALL
endif

ifeq ($(MMPROBE_ENABLE_FEATURE_MMPROBE),true)
LOCAL_CFLAGS += -DADM_MMPROBE
endif

ifeq ($(ADM_NO_EFFECT_CONFIG_SUPPORT), 1)
  LOCAL_CFLAGS += -DADM_NO_EFFECT_CONFIG_SUPPORT
endif

ifeq ($(ADM_DISABLE_TONEGEN), 1)
  LOCAL_CFLAGS += -DADM_DISABLE_TONEGEN
endif

ifeq ($(ADM_BUILD_PROD_SPEC),1)
  LOCAL_CFLAGS += -DADM_DB_PROD_SPEC
endif

ifeq ($(ADM_DEBUG), 1)
  LOCAL_CFLAGS += -DADM_DEBUG
endif

LOCAL_MODULE           := libste_adm
LOCAL_MODULE_TAGS      := optional
LOCAL_PRELINK_MODULE   := false

# U9540-specific stuff
ifeq ($(MULTIMEDIA_SET_PLATFORM),u9540)
  LOCAL_CFLAGS    += -DADM_ENABLE_FEATURE_FAT_MODEM
  LOCAL_CFLAGS    += -DADM_SUPPORT_RC_FILTER
  LOCAL_CFLAGS    += -DADM_DISABLE_MIXER_EFFECTS
endif

ifeq ($(MULTIMEDIA_SET_PLATFORM),l8540)
  LOCAL_CFLAGS    += -DSTE_PLATFORM_U8540
  LOCAL_CFLAGS    += -DADM_ENABLE_FEATURE_FAT_MODEM
  LOCAL_CFLAGS    += -DADM_SUPPORT_RC_FILTER
endif
ifeq ($(ONE_SHOT_MAKEFILE),)
  ## Source files depend upon STE MM header files installed during STE MM build
  $(addprefix $(LOCAL_PATH)/,$(LOCAL_SRC_FILES)): | st-ericsson-multimedia-package
endif

LOCAL_SHARED_LIBRARIES := \
        libcutils \
        libstelpcutils \
        libutils \
        libdl \
        libc \
        libste_ens_audio_samplerateconv \
        libsqlite \
        libmedia \
        libste_audio_mixer \
        libhardware_legacy \
        libomxil-bellagio \

ifeq ($(MULTIMEDIA_SET_PLATFORM),u9540)
LOCAL_SHARED_LIBRARIES += \
        libcaifsocketbroker
endif

ifeq ($(MULTIMEDIA_SET_PLATFORM),l8540)
LOCAL_SHARED_LIBRARIES += \
        libcaifsocketbroker
endif

ifeq ($(MMPROBE_ENABLE_FEATURE_MMPROBE),true)
LOCAL_SHARED_LIBRARIES += \
        libmmprobe
endif

LOCAL_C_INCLUDES += \
        $(LOCAL_PATH)/include \
        $(LOCAL_PATH)/src/il_tool \
        $(LOCAL_PATH)/src/ \
        $(LOCAL_PATH)/src/db \
        $(LOCAL_PATH)/src/dev \
        $(LOCAL_PATH)/src/api \
        $(LOCAL_PATH)/src/api/cscall \
        $(TOP)/external/sqlite/dist/  \
        $(TOP)/external/tinyalsa/include/ \
        $(TOP)/external/dbus/ \
        $(TOP)/external/libat/ \
        $(STE_MM_C_INCLUDES) \
        $(MULTIMEDIA_PATH)/audio/mmprobe/api \
        $(APPS_PATH)/caif_socket_broker/lib \
        $(APPS_PATH)/caif_socket_broker/integration

LOCAL_CFLAGS += -DSTE_PLATFORM_U8500

LOCAL_SHARED_LIBRARIES += \
        libalsactrl

LOCAL_C_INCLUDES += \
        $(MULTIMEDIA_PATH)/audio/alsactrl/include

# U9540-specific stuff
ifeq ($(MULTIMEDIA_SET_PLATFORM),u9540)

  LOCAL_SHARED_LIBRARIES += \
	libdbus \
	libat

  LOCAL_C_INCLUDES += \
        $(LOCAL_PATH)/src/hw_handler
endif

# l8540-specific stuff
ifeq ($(MULTIMEDIA_SET_PLATFORM),l8540)

  LOCAL_SHARED_LIBRARIES += \
        libdbus \
        libat

  LOCAL_C_INCLUDES += \
        $(LOCAL_PATH)/src/hw_handler
endif
include $(BUILD_SHARED_LIBRARY)

################################################################################
#
# admsrv
#

include $(CLEAR_VARS)

LOCAL_SRC_FILES        := src/ste_adm_server_process.c
LOCAL_MODULE           := admsrv
LOCAL_C_INCLUDES       := $(LOCAL_PATH)/include
LOCAL_SHARED_LIBRARIES := libste_adm libdl liblog
LOCAL_MODULE_TAGS      := optional

include $(BUILD_EXECUTABLE)

################################################################################
#
#  ste-adm-test
#

include $(CLEAR_VARS)

LOCAL_SRC_FILES        := src/test/test_client.c src/test/cmdline_parser.c
LOCAL_MODULE           := ste-adm-test
LOCAL_SHARED_LIBRARIES := libste_adm
LOCAL_MODULE_TAGS      := optional

LOCAL_C_INCLUDES       += \
        $(LOCAL_PATH)/include \
        $(STE_MM_C_INCLUDES)

ifeq ($(ADM_TEST_RADIO), 1)
  LOCAL_CFLAGS := -DTEST_CLIENT_ENABLE_RADIO
endif
ifeq ($(MULTIMEDIA_SET_PLATFORM),u9540)
  LOCAL_CFLAGS    += -DADM_ENABLE_FEATURE_FAT_MODEM
endif
ifeq ($(MULTIMEDIA_SET_PLATFORM),l8540)
  LOCAL_CFLAGS    += -DADM_ENABLE_FEATURE_FAT_MODEM
endif

ifeq ($(ONE_SHOT_MAKEFILE),)
  ## Source files depend upon STE MM header files installed during STE MM build
  $(addprefix $(LOCAL_PATH)/,$(LOCAL_SRC_FILES)): | st-ericsson-multimedia-package
endif

include $(BUILD_EXECUTABLE)

################################################################################
#
#  /system/etc/asound.conf
#

include $(CLEAR_VARS)

LOCAL_MODULE           := adm-asound.conf
LOCAL_MODULE_STEM      := asound.conf
LOCAL_SRC_FILES        := asound.conf
LOCAL_MODULE_PATH      := $(TARGET_OUT_ETC)
LOCAL_MODULE_TAGS      := optional
LOCAL_MODULE_CLASS     := ETC

include $(BUILD_PREBUILT)

################################################################################
#
#  /system/usr/share/nmf/repository/mmdsp_.../preload_adm.txt
#

STE_MMDSP_PLATFORM=

ifeq ($(MULTIMEDIA_SET_PLATFORM),u8500)
  STE_MMDSP_PLATFORM=mmdsp_8500_v2
endif
ifeq ($(MULTIMEDIA_SET_PLATFORM),u9500)
  STE_MMDSP_PLATFORM=mmdsp_8500_v2
endif
ifeq ($(MULTIMEDIA_SET_PLATFORM),u9540)
  STE_MMDSP_PLATFORM=mmdsp_9540_v1
endif
ifeq ($(MULTIMEDIA_SET_PLATFORM),l8540)
  STE_MMDSP_PLATFORM=mmdsp_8540_v1
endif
ifneq ($(STE_MMDSP_PLATFORM),)

  include $(CLEAR_VARS)

  LOCAL_MODULE           := preload_adm.txt
  LOCAL_SRC_FILES        := preload_adm.txt
  LOCAL_MODULE_PATH      := $(TARGET_OUT)/usr/share/nmf/repository/$(STE_MMDSP_PLATFORM)
  LOCAL_MODULE_TAGS      := optional
  LOCAL_MODULE_CLASS     := ETC

  include $(BUILD_PREBUILT)

endif

################################################################################
#
#  adm.sqlite
#
include $(CLEAR_VARS)

## Build db for u8500_ab8500
ifeq ($(ADM_BUILD_U8500_AB8500),1)
	ADM_DB_VARIANT := u8500_ab8500
	ifeq ($(ADM_PATH)/db/adm.sqlite-$(ADM_DB_VARIANT), $(wildcard $(ADM_PATH)/db/adm.sqlite-$(ADM_DB_VARIANT)))
		include $(ADM_PATH)/copy_db.mk
	else
		include $(ADM_PATH)/make_db_gen.mk
		include $(ADM_PATH)/make_db_sqlite.mk
	endif
endif

## Build db for u8520_ab8505_v1
ifeq ($(ADM_BUILD_U8500_AB8505_V1),1)
    ADM_DB_VARIANT := u8520_ab8505_v1
    include $(ADM_PATH)/make_db_gen.mk
    include $(ADM_PATH)/make_db_sqlite.mk
endif

## Build db for u8520_ab8505_2
ifeq ($(ADM_BUILD_U8520_AB8505_V2),1)
    ADM_DB_VARIANT := u8520_ab8505_v2
    include $(ADM_PATH)/make_db_gen.mk
    include $(ADM_PATH)/make_db_sqlite.mk
endif

## Build db for u9540_ab9540_v1
ifeq ($(ADM_BUILD_U9540_AB9540_V1),1)
    ADM_DB_VARIANT := u9540_ab9540_v1
    include $(ADM_PATH)/make_db_gen.mk
    include $(ADM_PATH)/make_db_sqlite.mk
endif

## Build db for u9540_ab8540_v1
ifeq ($(ADM_BUILD_U9540_AB8540_V1),1)
    ADM_DB_VARIANT := l8540_ab8540_v1
    include $(ADM_PATH)/make_db_gen.mk
    include $(ADM_PATH)/make_db_sqlite.mk
endif

## Build db for u9540_ab9540_v2
ifeq ($(ADM_BUILD_U9540_AB9540_V2),1)
    ADM_DB_VARIANT := u9540_ab9540_v2
    include $(ADM_PATH)/make_db_gen.mk
    include $(ADM_PATH)/make_db_sqlite.mk
endif

## Build db for u9540_ab9540_v3
ifeq ($(ADM_BUILD_U9540_AB9540_V3),1)
    ADM_DB_VARIANT := u9540_ab9540_v3
    include $(ADM_PATH)/make_db_gen.mk
    include $(ADM_PATH)/make_db_sqlite.mk
endif

## Build product specific db
ifeq ($(ADM_BUILD_PROD_SPEC),1)
    ADM_DB_VARIANT := $(TARGET_PRODUCT)
    include $(ADM_PATH)/make_db_gen.mk
    include $(ADM_PATH)/make_db_sqlite.mk
endif
