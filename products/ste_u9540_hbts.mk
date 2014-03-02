# Superclass
$(call inherit-product, vendor/st-ericsson/products/ste_u9540.mk)

# Overrides
PRODUCT_NAME := ste_u9540_hbts
#Add specific HATS tuning variable
ifeq ($(TARGET_PRODUCT), ste_u9540_hbts)
ENABLE_FEATURE_BUILD_HBTS := true
endif


