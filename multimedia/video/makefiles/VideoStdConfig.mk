# -*- Mode: Makefile -*-
# vim:syntax=make:

export MEDIUM_DIR MMPLATFORM SVA_SUBSYSTEM # for install idt

#MCPROJECT = $(DSPTOOLS)/target/project/nomadik/STn8815/video
include $(MM_MAKEFILES_DIR)/VideoConfig.mk

ifeq ($(MAKEFILES_BACKWARD_COMPATIBLE),yes)
COMPONENTS_ROOT = $(MMROOT)/video/components/
HW_VERSION = 0x$(shell grep 'define HV_HW_REL' '$(COMPONENTS_ROOT)/registers_def/$(SVA_SUBSYSTEM)/inc/t1xhv_hamac_xbus_regs.h' | sed -e "s/.*0x//" | sed -e "s/U.*//")
else
HW_VERSION = 0x$(shell grep 'define HV_HW_REL' '$(MM_HEADER_DIR)/$(SVA_SUBSYSTEM)/t1xhv_hamac_xbus_regs.h' | sed -e "s/.*0x//" | sed -e "s/U.*//")
endif

FW_VERSION = 0x0500

STACK = 500
SSTACK = 500

CPPFLAGS = -DMMDSP -D__$(SVA_SUBSYSTEM)__ -D__PLATFORM_$(SVA_SUBSYSTEM)__
ifeq ($(MAKEFILES_BACKWARD_COMPATIBLE),yes)
CPPFLAGS +=	-I$(COMPONENTS_ROOT)/registers_def/$(SVA_SUBSYSTEM)/inc	\
			-I$(COMPONENTS_ROOT)/hardware/inc/$(SVA_SUBSYSTEM) 		\
			-I$(COMPONENTS_ROOT)/hardware/inc				\
			-I$(COMPONENTS_ROOT)/common/inc/$(SVA_SUBSYSTEM) 		\
			-I$(COMPONENTS_ROOT)/common/inc
else
CPPFLAGS += -I$(MM_HEADER_DIR)/$(SVA_SUBSYSTEM) -I$(MM_HEADER_DIR)
endif

EXELDFLAGS = pla=-g

#EXELDFLAGS += project=$(DSPTOOLS)/target/project/nomadik/STn8815/video
EXELDFLAGS += +link +nostdio

include $(MM_MAKEFILES_DIR)/SharedMMDSPConfig.mk

# Remove -Iinc from generic makefiles
CPPFLAGS := $(subst -Iinc ,,$(CPPFLAGS))

# Install of idt files
IDT_DIR = $(MODULE)/idt
#already defined in SharedDefines.mk!
#INSTALL_IDT_DIR    = $(MEDIUM_DIR)/idt/$(MMDSPCORE)
#INSTALL_IDT_DIR = $(INSTALL_NMF_COMP_SRC_COMMON_DIR)

# Compatibility with older makefiles
ifndef MMDSPPROCESSOR
MMDSPPROCESSOR = $(PLATFORM)
endif

#already defined in generic makefiles (+ link $(MMROOT)/video/lib -> $MMBUILDOUT/lib)
#INSTALL_LIB_DIR = $(MMROOT)/video/lib/$(MMDSPPROCESSOR)

ifeq ($(LIB_EXT),a)
MEMCFG_EXT = ld
else
MEMCFG_EXT = lcf
endif

VPATH += src/$(SVA_SUBSYSTEM)

POST_BUILD = cd $(dir $@);
ifeq ($(MAKEFILES_BACKWARD_COMPATIBLE),yes)
POST_BUILD += echo Creating the files for mmdsp simulation;
POST_BUILD += splitrom $(EXENAME).hex $(COMPONENTS_ROOT)/common/splitrom/video.spec;
POST_BUILD += splitrom $(EXENAME).pla $(COMPONENTS_ROOT)/common/splitrom/code_video.spec;
else
POST_BUILD += echo Creating the files for mmdsp simulation;
POST_BUILD += splitrom $(EXENAME).hex $(MM_HEADER_DIR)/video.spec;
POST_BUILD += splitrom $(EXENAME).pla $(MM_HEADER_DIR)/code_video.spec;
endif
POST_BUILD += echo Creating deasm;
POST_BUILD += deasm $(EXENAME).elf

MODE16 = 1
#ACFLAGS += project=$(DSPTOOLS)/target/project/nomadik/STn8815/video

