ifeq ($(ENABLE_ST_ERICSSON_BUILD), true)
include $(call first-makefiles-under, $(call my-dir))
endif
