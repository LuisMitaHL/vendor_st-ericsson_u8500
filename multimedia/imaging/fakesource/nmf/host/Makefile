# vim:syntax=make:
# -*- Mode: Makefile -*-

#WARNING COMPONENT_PATH is MANDATORY for Make.config and Make.rules
#-------------------------------
# Default Config
#-------------------------------

CORE=arm

include $(MM_MAKEFILES_DIR)/SharedNmfConfig.mk

CPPFLAGS += -DMMU_OFFSET

CPPFLAGS += -I$(MM_HEADER_DIR)
CPPFLAGS += -I$(MM_NMF_REPO_DIR)
CPPFLAGS += -I$(MM_NMF_REPO_BOARD_DIR)
CPPFLAGS += -Ifake_source/inc

export CPPFLAGS
CXXFLAGS += -g -O0 
export CXXFLAGS

ITFS_HEADER=	


COMPONENTS=fake_source_wrp 
#EXPORTED_DIR=fake_source

MMINSTALL_ARGS += -n=/armnmf_buffer.idt.h \
		-n=/armnmf_emptythisbuffer.hpp \
		-n=/armnmf_fillthisbuffer.hpp \
		-n=/ee/api/panic.idt.h \
		-n=/ee/api/trace.hpp \
		-n=/eventhandler.hpp \
		-n=/fsmInit.idt.h \
		-n=/fsminit.hpp \
		-n=/omxcommand.idt.h \
		-n=/omxerror.idt.h \
		-n=/omxevent.idt.h \
		-n=/omxeventext.idt.h \
		-n=/omxstate.idt.h \
		-n=/sendcommand.hpp \
		-n=/trace.idt.h


#-------------------------------------
# Generic rules
#-------------------------------------
include $(MM_MAKEFILES_DIR)/SharedNmfRules.mk

ifeq ($(findstring symbian,$(PLATFORM)),symbian)
NO_VARIANT = true
include ${MMROOT}/imaging/symbian_makerules/mmh.mk
endif

NMFCONFC_FLAGS= "--cpp --archive"

#install: $(HEADER_LNK)
