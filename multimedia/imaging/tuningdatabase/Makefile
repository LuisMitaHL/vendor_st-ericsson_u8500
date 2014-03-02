# vim:syntax=make:
# -*- Mode: Makefile -*-

#WARNING COMPONENT_PATH is MANDATORY for Make.config and Make.rules
#-------------------------------
# Default Config
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedConfig.mk

LIBNAME     = st_tuningdatabase
INSTALL_LIB = libst_tuningdatabase.a

ifdef DEBUG
CPPFLAGS += -DOSI_DEBUG -D__DEBUG
CFLAGS    = -g -O0
OBJ_EXT   = odebug 
else
CPPFLAGS += -DRELEASE -D__RELEASE
endif

CPPFLAGS+= -I$(MM_HEADER_BASEDIR)/shared/omxil

LIBOBJ         = tuning_data_base
INSTALL_HEADER = tuning_data_base.h

# Generation of solibs
ifneq ($(findstring symbian,$(PLATFORM)),symbian)
ifneq ($(PLATFORM),x86-linux)
IS_OMX_COMPONENT_LIB:=false
include $(MM_MAKEFILES_DIR)/STEOMXLoaderConfig.mk

SOLIBNAMES = ste_ens_image_tuningdatabase
IMAGE_LIBS = $(LIBNAME) st_tuning st_omxilosalservices
EXTRALIBS_$(SOLIBNAMES)= $(IMAGE_LIBS) 
ifeq ($(MMCOMPILER),android)
FLAGS_$(SOLIBNAMES) = -llos -lstelpcutils -lbassapp
else
FLAGS_$(SOLIBNAMES) = -llos -lstelpcutils
endif
INSTALL_SOLIB = $(SOLIBNAMES)
endif
endif

#WARNING todo target is MANDATORY (exe and/or lib)
todo: lib solib

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk
ifeq ($(MMSYSTEM),symbian) 
NO_VARIANT=true
include ${MMROOT}/imaging/symbian_makerules/mmh.mk
endif

