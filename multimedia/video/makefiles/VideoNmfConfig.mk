# -*- Mode: Makefile -*-
# vim:syntax=make:

export CONFFILES ITFLIST IDLLIST IDTLIST SVA_SUBSYSTEM

include $(MM_MAKEFILES_DIR)/SharedNmfConfig.mk
include $(MM_MAKEFILES_DIR)/VideoConfig.mk

CPPFLAGS += -D__$(SVA_SUBSYSTEM)__

ifeq ($(findstring 500_v2,$(PLATFORM)),500_v2)
CPPFLAGS += -D_8500_V2_
endif

# Remove -Iinc from generic makefiles
CPPFLAGS := $(subst -Iinc ,,$(CPPFLAGS))

VPATH += ../$(MODULE)
SRCPATH += ../$(MODULE) 

#temporary fix for symbian build and backward compatibility
ifeq ($(MYMMSYSTEM),symbian)
SRCPATH += $(MMROOT)/video/idt/$(MMDSPCORE) 
SRCPATH += $(MMROOT)/video/idt
SRCPATH += $(MMROOT)/video/components/common/nmf/idt
SRCPATH += $(MMROOT)/video/components/resource_manager/idt
endif

ITF_PREFIX = video_$(shell echo $(shell pwd) | sed -e "s/.*\/components\///" | sed -e "s/\/.*//")

PREBUILD_COMPONENTS = $(CONFFILES)
