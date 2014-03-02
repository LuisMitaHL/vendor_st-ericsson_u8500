LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LOCAL_DIR)/include \
			-Iapp/steboot \
			-Iinclude/platform

ifeq ($(ENABLE_FEATURE_BUILD_HBTS),)
OBJS += \
	$(LOCAL_DIR)/crc32.o \
	$(LOCAL_DIR)/cspsa_fp.o \
	$(LOCAL_DIR)/partition_parser.o \
	$(LOCAL_DIR)/tiny_env.o \
	$(LOCAL_DIR)/toc_common.o \
	$(LOCAL_DIR)/toc_norom.o \
	$(LOCAL_DIR)/usb_if.o \
	$(LOCAL_DIR)/usb_wrapper.o \
	$(LOCAL_DIR)/fs_sd_wrapper.o
endif

ifeq ($(ENABLE_FEATURE_BUILD_HBTS),true)
OBJS += \
	$(LOCAL_DIR)/cmd_mem.o \
	$(LOCAL_DIR)/cmd_mmc.o
endif

OBJS += \
	$(LOCAL_DIR)/boottime.o \
	$(LOCAL_DIR)/db_gpio.o \
	$(LOCAL_DIR)/mmc_if.o \
	$(LOCAL_DIR)/cmd_reboot.o \
	$(LOCAL_DIR)/cmd_i2c.o


MODULES += \
	lib/fs/ff9
