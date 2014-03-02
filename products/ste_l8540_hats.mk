# Superclass
$(call inherit-product, vendor/st-ericsson/products/ste_l8540.mk)

# Overrides
PRODUCT_NAME := ste_l8540_hats

PRODUCT_PACKAGES += libdbushats \
                    dbus-daemhats \
                    lifecycle

#Add specific HATS tuning variable
ifeq ($(TARGET_PRODUCT), ste_l8540_hats)
ENABLE_FEATURE_BUILD_HATS := true
HATS_MAIN_PATH := $(TOP)/vendor/st-ericsson/validation/hardware
endif

#Add specific PATCHs VAR
ifeq ($(TARGET_PRODUCT), ste_l8540_hats)
PATCH_VARS := HATS_PATH=vendor/st-ericsson/validation/hardware/hats
endif

#Add specific HATS defconfig
ifeq ($(TARGET_PRODUCT), ste_l8540_hats)
# Call script to create/modify u8500_defconfig (add CONFIG_UX500_HATS_DEEP_DBG=y and CONFIG_DEEP_DEBUG=y )
UPDATE_KERNEL_DEFCONFIG_FILE:= $(shell test -f vendor/st-ericsson/l8540/patch_kernel_defconfig.sh && sh vendor/st-ericsson/l8540/patch_kernel_defconfig.sh)
endif

