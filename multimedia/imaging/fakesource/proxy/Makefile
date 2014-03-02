# vim:syntax=make:
# -*- Mode: Makefile -*-

#WARNING COMPONENT_PATH is MANDATORY for Make.config and Make.rules
#-------------------------------
# Default Config
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedConfig.mk


CPPFLAGS += -I $(MM_HEADER_DIR)

ifeq ($(PLATFORM),x86-linux)
CPPFLAGS += -DNO_DEPENDENCY_ON_IFM_NMF
endif
ifdef NO_IFM_NMF
CPPFLAGS += -DNO_DEPENDENCY_ON_IFM_NMF
endif

ifdef IMG_CONFIG
	ifdef NO_IFM
	CPPFLAGS += -DNO_DEP_WITH_IFM
	endif
else
	CPPFLAGS += -DNO_DEP_WITH_IFM
endif

CFLAGS+=-O0

ifdef DEBUG
CPPFLAGS += -DDEBUG -D__DEBUG
LIBNAME   = fakesource_debug
OBJ_EXT =odebug 
else
CPPFLAGS += -DRELEASE -D__RELEASE
LIBNAME   = fakesource
endif

LIBOBJ   = fakesource 


INSTALL_HEADER = fakesource.h
INSTALL_LIB    = $(addprefix lib, $(addsuffix .a, $(LIBNAME)))

ifneq ($(findstring symbian,$(PLATFORM)),symbian)
ifneq ($(PLATFORM),x86-linux)
IS_OMX_COMPONENT_LIB:=true
include $(MM_MAKEFILES_DIR)/STEOMXLoaderConfig.mk

SOLIBNAMES = ste_fakesource
LIBOBJ_$(SOLIBNAMES) = ste_omx_ens_component
IMAGE_LIBS = $(LIBNAME)
IMAGING_NMF_HOST_COMPONENTS = fake_source_wrp
SOLIBS_$(SOLIBNAMES) += $(IMAGING_NMF_HOST_COMPONENTS_LIBS)
EXTRALIBS_$(SOLIBNAMES)= $(IMAGE_LIBS) 
FLAGS_$(SOLIBNAMES) += -L$(INSTALL_SOLIB_DIR)/.. -lste_ens_image_common 
FLAGS_$(SOLIBNAMES) += $(LDFLAGS_STE_ENS_OMX_COMPONENTS) -llos 

INSTALL_SOLIB = $(SOLIBNAMES)
endif
endif

#WARNING todo target is MANDATORY (exe and/or lib)
todo: lib solib

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

ifeq ($(findstring symbian,$(PLATFORM)),symbian)
 NO_VARIANT=true
 COMPONENT_TYPE=TEST
 include ${MEDIUM_DIR}/symbian_makerules/mmh.mk
endif

