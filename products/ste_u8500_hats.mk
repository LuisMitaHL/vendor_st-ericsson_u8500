# Superclass
$(call inherit-product, vendor/st-ericsson/products/ste_u8500.mk)

# Overrides
PRODUCT_NAME := ste_u8500_hats

#Add specific HATS tuning variable
ifeq ($(TARGET_PRODUCT), ste_u8500_hats)
ENABLE_FEATURE_BUILD_HATS := true
HATS_MAIN_PATH := $(TOP)/vendor/st-ericsson/validation/hardware
PRODUCT_COPY_FILES += $(HATS_MAIN_PATH)/hats/ramdisk/init.hats.rc:root/init.hats.rc
endif

#Add specific PATCHs VAR
ifeq ($(TARGET_PRODUCT), ste_u8500_hats)
PATCH_VARS := HATS_PATH=vendor/st-ericsson/validation/hardware/hats
endif
