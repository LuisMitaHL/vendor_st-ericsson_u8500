# vim:syntax=make:
# -*- Mode: Makefile -*-

#WARNING COMPONENT_PATH is MANDATORY for Make.config and Make.rules
#-------------------------------
# Default Config
#-------------------------------
MMSYSTEM=x
MMBOARD=x

include $(MM_MAKEFILES_DIR)/SharedConfig.mk

BUILD_INCLUDES = -I ./
BUILD_INCLUDES+= -I ./include

CPPFLAGS += -I $(MM_HEADER_DIR)
CPPFLAGS += -I $(MM_IMAGING_NMF_REPO_DIR)/host
CPPFLAGS += -I $(SHARED_REPOSITORY_HOST)/openmax_processor_wrp/src
CPPFLAGS += ${BUILD_INCLUDES}

ifdef DEBUG
	CPPFLAGS += -DDEBUG -D__DEBUG -DOSI_DEBUG -DIFM_TRACE=1
	LIBNAME   = st_wrapper_openmax_debug
	CFLAGS=-g -O0
	OBJ_EXT =odebug
else
	CPPFLAGS += -DRELEASE -D__RELEASE
	LIBNAME   = st_wrapper_openmax
endif

# CR337836 Activate dynamic trace
CPPFLAGS += -DCONFIG_DEBUG -DDBGT_VAR=mDebugTraceVarWrapper -DCONFIG_DEBUG_AUTOVAR

LIBOBJ= wrapper_openmax_tools  openmax_component openmax_component_proxy 

SOLIBOBj = $(LIBOBJ)

INSTALL_HEADER = openmax_component.h openmax_component_proxy.h wrapper_openmax_lib.h wrapper_openmax_tools.h SmartPointer.h
INSTALL_LIB    = $(addprefix lib, $(addsuffix .a, $(LIBNAME)))

#WARNING todo target is MANDATORY (exe and/or lib)
todo: lib 


#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

ifeq ($(MMSYSTEM),symbian) 
NO_VARIANT=true
include ${MEDIUM_DIR}/symbian_makerules/mmh.mk
endif

