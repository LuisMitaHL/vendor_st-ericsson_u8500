LOCAL_DIR := $(GET_LOCAL_DIR)

OBJS += \
	$(LOCAL_DIR)/display-sony_acx424akp_dsi.o \
	$(LOCAL_DIR)/display-himax_hx8392.o \
	$(LOCAL_DIR)/db8500-prcmu.o \
	$(LOCAL_DIR)/mcde_bus.o \
	$(LOCAL_DIR)/mcde_display.o \
	$(LOCAL_DIR)/mcde_display_dsi.o \
	$(LOCAL_DIR)/mcde_display_image.o \
	$(LOCAL_DIR)/mcde_display_config.o \
	$(LOCAL_DIR)/dsilink.o \
	$(LOCAL_DIR)/dsilink_v2.o \
	$(LOCAL_DIR)/dsilink_v3.o \
	$(LOCAL_DIR)/mcde_hw.o
ifeq ($(ENABLE_FEATURE_SELFTESTS_IN_LOADER),true)
	OBJS += $(LOCAL_DIR)/cmd_display.o
endif