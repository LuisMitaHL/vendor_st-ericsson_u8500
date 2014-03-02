BASE_PATH := $(call my-dir)

DIRS := \
	$(BASE_PATH)/libmpl/Android.mk \
	$(BASE_PATH)/libnlcom/Android.mk
include $(DIRS)
