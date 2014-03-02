# -*- Mode: Makefile -*-
# vim:syntax=make:
include $(MM_MAKEFILES_DIR)/SharedConfig.mk
include $(MM_MAKEFILES_DIR)/AudioFlags.mk

CPPFLAGS+=  -I$(COMPONENT_TOP_DIR)/include -I$(COMPONENT_TOP_DIR)/common/include 

CPPFLAGS+=  -I$(MM_HEADER_DIR)/audiolibs/include -I$(MM_HEADER_DIR)/audiolibs/common/include -I$(MM_HEADER_DIR)/audiolibs
