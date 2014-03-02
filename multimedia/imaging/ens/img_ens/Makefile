# vim:syntax=make:
# -*- Mode: Makefile -*-

#WARNING COMPONENT_PATH is MANDATORY for Make.config and Make.rules
#-------------------------------
# Default Config
#-------------------------------
MMSYSTEM=x
MMBOARD =x

include $(MM_MAKEFILES_DIR)/SharedConfig.mk

# CR337836 Activate dynamic trace
CPPFLAGS += -DCONFIG_DEBUG -DDBGT_VAR=mDebugTraceVar_ImgEns -DCONFIG_DEBUG_AUTOVAR

VPATH      += src/Component src/Fsm src/Utils 
#Following line is required for finding sub dir of includes
VPATH      += inc/Component inc/omxil

ifdef DEBUG
	CPPFLAGS   += -D_DEBUG
	CFLAGS     += -g -O3
	OBJ_EXT     =odebug
	DBG_SUF     =_debug
else
	CPPFLAGS   += -g -D_RELEASE
	DBG_SUF     =
endif
LIBNAME     = ste_img_ens$(DBG_SUF)
SOLIBNAMES =$(LIBNAME)

INSTALL_HEADER     := ImgEns_Shared.h ImgEns_Index.h ImgEns_IOmx.h 
# ImgEns_Component.h ImgEns_Port.h ImgEns_ProcessingComponent.h ImgEns_Fsm.h ImgEns_Component_Fsm.h ImgEns_Port_Fsm.h ImgEns_FsmEvent.h ImgEns_Trace.h 
INSTALL_HEADER     += ImgEns_Lib.h
INSTALL_HEADERDIR  +=inc/omxil 
INSTALL_HEADERDIR  +=inc/Component 

#don't know why but INSTALL_LIB compulsory for having them in /work1/1220/out/ux600_svp/obj/mmbuildout/lib/cortexA15-x-gnu-x
INSTALL_SOLIB:=$(LIBNAME)

#WARNING 'todo' target is MANDATORY (exe and/or lib solib)
todo: solib

#solib part
LIBOBJ_$(LIBNAME) = ImgEns_Shared ImgEns_IOmx ImgEns_Port ImgEns_Component ImgEns_Port_Fsm ImgEns_Component_Fsm ImgEns_Fsm ImgEns_Trace 

FLAGS_$(LIBNAME) += -lpthread  -ltrace -lst_mmhwbuffer -lst_omxilosalservices -lste_osi_toolbox$(DBG_SUF)
ifneq ($(MMPROCESSOR),x86)
  FLAGS_$(LIBNAME) +=-lstelpcutils 
else
  FLAGS_$(LIBNAME) +=-llos
endif

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

print:
	@echo Display variables content for VARS=$(VARS)
	@echo -e '$(foreach VAR, $(VARS),$(VAR)=$($(VAR))\n)'

