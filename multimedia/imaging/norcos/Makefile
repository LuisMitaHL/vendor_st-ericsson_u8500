# vim:syntax=make:
# -*- Mode: Makefile -*-

#WARNING COMPONENT_PATH is MANDATORY for Make.config and Make.rules
#-------------------------------
# Default Config
#-------------------------------
MMSYSTEM=x
MMBOARD=x
include $(MM_MAKEFILES_DIR)/SharedConfig.mk


CPPFLAGS += -I $(MM_NMF_COMP_SRC_HOSTEE_DIR)/nmf/host
CPPFLAGS += -I $(MM_HEADER_DIR)
CPPFLAGS += -I $(MM_HEADER_DIR)/openmax_processor_wrp

# these flags can be set (on an individual basis) to trace memory usage and performance of the component.
# beware that aiq_common shall be recompiled with the same set of flags beforehands
#CPPFLAGS += -DAIQ_MEMTRACE_ENABLE -DAIQ_PERFTRACE_ENABLE

# this flag is set to support norcos dampers and extra data parsing 
CPPFLAGS += -DNORCOS_DAMPERS_SUPPORT

# CR337836 Activate dynamic trace
CPPFLAGS += -DCONFIG_DEBUG -DDBGT_VAR=mDebugTraceVarNorcos -DCONFIG_DEBUG_AUTOVAR 

# this flag must be set only when running MMTE unitary tests
# CPPFLAGS += -DNORCOS_MMTE_UNIT_TEST

ifdef DEBUG
	CPPFLAGS += -DDEBUG -D__DEBUG -DOSI_DEBUG
	LIBNAME   = st_omxnorcos_debug
	SOLIBNAME = st_omxnorcos_debug
	CFLAGS=-g -O0
	OBJ_EXT =odebug
else
	CPPFLAGS += -DRELEASE -D__RELEASE
	LIBNAME   = st_omxnorcos
	SOLIBNAME = st_omxnorcos
endif

LIBOBJ   = norcos_proxy norcos_process norcos norcos_dampers 

INSTALL_HEADER = OMX_extensions/norcos_OMX_extensions.h
INSTALL_LIB    = $(addprefix lib, $(addsuffix .a, $(LIBNAME)))



ifneq ($(MYMMSYSTEM), symbian)
ifneq ($(PLATFORM),x86-linux)
# We are generating an OMX component .so - request to be installed in correct place
IS_OMX_COMPONENT_LIB:=true
include $(MM_MAKEFILES_DIR)/STEOMXLoaderConfig.mk

SOLIBNAMES+=${LIBNAME}
LIBOBJ_$(SOLIBNAMES) = ste_omx_ens_component
IMAGE_LIBS =$(LIBNAME) st_wrapper_openmax st_aiq_common
IMAGING_NMF_HOST_COMPONENTS = openmax_processor_wrp
# WA as no immediate completion for ifm 
IFM_LIB = $(wildcard $(foreach mmpath, $(MMSEARCHPLAT), $(MM_LIB_DIRNAME)/$(mmpath)/libst_ifm.a )) 
IFM_LIB += $(wildcard $(foreach mmpath, $(MMSEARCHPLAT), $(MM_LIB_DIRNAME)/$(mmpath)/libst_damper.a )) 

SOLIBS_${SOLIBNAMES} += $(IMAGING_NMF_HOST_COMPONENTS_LIBS)
EXTRALIBS_${SOLIBNAMES}= $(IMAGE_LIBS)
PLATFORM_LIST = cortexA9-linux-gnu-$(MYMMBOARD) cortexA9-linux-gnu-x cortexA9-x-rvct-x cortexA9-x-gnu-$(MYMMBOARD) cortexA9-x-rvct4gnu-x cortexA9-x-gnu-x  cortexA9-linux-gnu-href_v1
LIB_DIR_LIST += $(foreach mmpath, $(PLATFORM_LIST), $(addprefix -L,$(MM_LIB_DIRNAME)/$(mmpath)))
SOLIB_DIR_LIST =  $(foreach mmpath, $(PLATFORM_LIST), $(addprefix -L,$(MMBUILDIN)/$(ROOTFS_DIR_NAME)/$(mmpath)$(COMPONENT_PATH)))

FLAGS_$(SOLIBNAMES) += $(IFM_LIB) $(LIB_DIR_LIST) $(SOLIB_DIR_LIST) 
FLAGS_$(SOLIBNAMES) += $(LDFLAGS_STE_ENS_OMX_COMPONENTS) -llos
FLAGS_$(SOLIBNAMES) += -lste_ens_image_tuningdatabase

ifneq ($(MMCOMPILER),android)
FLAGS_ste_norcos += -lrt
endif
INSTALL_SOLIB = ${SOLIBNAMES}
endif
endif


#WARNING todo target is MANDATORY (exe and/or lib)
todo: lib solib

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk


print:
	@echo Display variables content for VARS=$(VARS)
	@echo -e "$(foreach VAR, $(VARS),$(VAR)=$($(VAR))\n)"
