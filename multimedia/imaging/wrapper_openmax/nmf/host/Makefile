# vim:syntax=make:
# -*- Mode: Makefile -*-

#WARNING COMPONENT_PATH is MANDATORY for Make.config and Make.rules
#-------------------------------
# Default Config
#-------------------------------

CPPFLAGS += -DMMU_OFFSET

CORE=arm
MMSYSTEM=x
MMBOARD=x

include $(MM_MAKEFILES_DIR)/SharedNmfConfig.mk

CPPFLAGS += -I$(COMPONENT_TOP_DIR)/proxy/include
CPPFLAGS += -I$(MM_HEADER_DIR)
CPPFLAGS += -I$(MM_NMF_REPO_DIR)
CPPFLAGS += -I$(INSTALL_NMF_REPO_BOARD_DIR)
CPPFLAGS += -I$(SHARED_REPOSITORY)/$(MMPROCESSOR)-x-$(MMCOMPILER)-x

export CPPFLAGS
CXXFLAGS += -g -O0 
export CXXFLAGS

ITFS_HEADER= 

COMPONENTS=openmax_processor_wrp 

INSTALL_COMPONENTS = nmf.host.openmax_processor

INSTALL_NMF_COMP_SRC=true

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

#EXPORTED_DIR= 
# openmax_processor
# imageproc_proc

#-------------------------------------
# Header 
#-------------------------------------
## Install generated files
EXTRA_INSTALL = openmax_processor_wrp_header_install
EXTRA_UNINSTALL = openmax_processor_wrp_header_uninstall

NMF_HEADERS= \
  openmax_processor.nmf \
  lifecycle/starter.hpp \
  lifecycle/stopper.hpp

VPATH += $(TMPDIR)/openmax_processor_wrp/src

INSTALL_HEADER_PREFIX_DIR=openmax_processor_wrp/
HEADER_INSTALL= $(addprefix $(INSTALL_HEADER_DIR)/$(INSTALL_HEADER_PREFIX_DIR), $(NMF_HEADERS))

$(INSTALL_HEADER_DIR)/$(INSTALL_HEADER_PREFIX_DIR)%: %
	@echo "  MMINSTALL $<"
	$(VERCHAR)$(MMINSTALL) $< $@

internal_header_install: $(HEADER_INSTALL) 

openmax_processor_wrp_header_install:
	make internal_header_install

openmax_processor_wrp_header_uninstall: 
	$(VERCHAR)echo "*** Removing dir $(INSTALL_HEADER_DIR)/$(INSTALL_HEADER_PREFIX_DIR) " 
	@rm -rf $(INSTALL_HEADER_DIR)/$(INSTALL_HEADER_PREFIX_DIR)


include $(MM_MAKEFILES_DIR)/SharedNmfRules.mk

#  $(info )
#  $(info VPATH=$(VPATH))
#  $(info INSTALL_HEADER_DIR=$(INSTALL_HEADER_DIR))
#  $(info )


ifeq ($(MMSYSTEM),symbian) 
	NO_VARIANT=true
	include ${MEDIUM_DIR}/symbian_makerules/mmh.mk
endif

NMFCONFC_FLAGS= "--cpp --archive"

