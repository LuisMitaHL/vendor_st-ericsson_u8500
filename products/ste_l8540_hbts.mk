# Superclass
$(call inherit-product, vendor/st-ericsson/products/ste_l8540.mk)

# Overrides
PRODUCT_NAME := ste_l8540_hbts
#Add specific HATS tuning variable
ifeq ($(TARGET_PRODUCT), ste_l8540_hbts)
ENABLE_FEATURE_BUILD_HBTS := true
endif


