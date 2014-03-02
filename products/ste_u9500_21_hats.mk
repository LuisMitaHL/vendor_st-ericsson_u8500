# Superclass
$(call inherit-product, vendor/st-ericsson/products/ste_u9500_21.mk)

# Overrides
PRODUCT_NAME := ste_u9500_21_hats


#Add specific HATS tuning variable
ifeq ($(TARGET_PRODUCT), ste_u9500_21_hats)
ENABLE_FEATURE_BUILD_HATS := true
HATS_MAIN_PATH := $(TOP)/vendor/st-ericsson/validation/hardware
endif

#Add specific PATCHs VAR
ifeq ($(TARGET_PRODUCT), ste_u9500_21_hats)
PATCH_VARS := HATS_PATH=vendor/st-ericsson/validation/hardware/hats
endif

