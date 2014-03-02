BASE_PATH := $(call my-dir)

PSDATA_STEPSCC_BUILD_PSCC_CMD := false

DIRS := \
	$(BASE_PATH)/libpscc/Android.mk \
	$(BASE_PATH)/stepscc/Android.mk
ifeq ($(PSDATA_STEPSCC_BUILD_PSCC_CMD), true)
  DIRS += $(BASE_PATH)/tools/pscc_cmd/Android.mk
endif

include $(DIRS)

