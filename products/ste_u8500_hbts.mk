# Superclass
$(call inherit-product, vendor/st-ericsson/products/ste_u8500.mk)

# Overrides
PRODUCT_NAME := ste_u8500_hbts
#Add specific HBTS tuning variable
ifeq ($(TARGET_PRODUCT), ste_u8500_hbts)
ENABLE_FEATURE_BUILD_HBTS := true
endif

