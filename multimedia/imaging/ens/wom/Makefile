# vim:syntax=make:
# -*- Mode: Makefile -*-

#WARNING COMPONENT_PATH is MANDATORY for Make.config and Make.rules
#-------------------------------
# Default Config
#-------------------------------
MMSYSTEM=x
MMBOARD =x

include $(MM_MAKEFILES_DIR)/SharedConfig.mk

CPPFLAGS += -I../..
CPPFLAGS += -DCONFIG_DEBUG -DDBGT_VAR=mDebugTraceWom -DCONFIG_DEBUG_AUTOVAR

ifdef DEBUG
	DBG_SUF   =_debug
	CPPFLAGS += -D_DEBUG
	CPPFLAGS += -g -O3
	OBJ_EXT   =odebug
	FORCE_EXPORT_OF_SOURCES=1
else
	DBG_SUF=
	CPPFLAGS += -D_RELEASE
	CPPFLAGS += -g
endif
SOLIBNAME = ste_wom$(DBG_SUF)

LIBOBJ   = wom_async_processor wom_async_port wom_queue wom_fsm wom_port wom_component wom_tools wom_proxy wom_syncproxy

INSTALL_HEADER = wom_async_processor.h wom_async_port.h wom_fsm.h wom_queue.h  wom_shared.h wom_tools.h wom_port.h wom_component.h wom_lib.h wom_proxy.h wom_syncproxy.h 
#INSTALL_LIB    = $(addprefix lib, $(addsuffix .a, $(LIBNAME)))

todo: solib  install_source 
.PHONY: print

ifdef FORCE_EXPORT_OF_SOURCES
install_source:
	@echo " info FORCE_EXPORT_OF_SOURCES defined -> install source code from '$(PWD)/src' to '$(INSTALL_ROOTFS_DIR)$(PWD)/src'!!!"
	mkdir -p $(INSTALL_ROOTFS_DIR)$(PWD)
	if  test ! -d $(INSTALL_ROOTFS_DIR)$(PWD)/src; then ln -fs $(PWD)/src $(INSTALL_ROOTFS_DIR)$(PWD)/src; fi
else
install_source:
	@echo  "Don't export sources"
endif

SOLIBNAME1    =${SOLIBNAME}
SOLIBNAMES    =${SOLIBNAME1}
INSTALL_SOLIB =${SOLIBNAMES}

LIBOBJ_$(SOLIBNAME1) =$(LIBOBJ)
LIBS_$(SOLIBNAME1)  += st_img_processing_thread st_img_osal ste_omx_toolbox$(DBG_SUF) ste_osi_toolbox$(DBG_SUF) 
FLAGS_$(SOLIBNAME1) += -lpthread -lrt  -lst_img_processing_thread -lste_img_enswrapper -ltrace -lst_mmhwbuffer -lste_img_ens$(DBG_SUF)
ifneq ($(MMPROCESSOR),x86)
  FLAGS_$(SOLIBNAME1) +=-lstelpcutils 
else
#  FLAGS_$(SOLIBNAME1) +=-llos
endif

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

print:
	@echo Display variables content for VARS=$(VARS)
	@echo -e "$(foreach VAR, $(VARS),$(VAR)=$($(VAR))\n)"

