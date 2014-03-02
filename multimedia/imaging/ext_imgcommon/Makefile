# vim:syntax=make:
# -*- Mode: Makefile -*-

#WARNING COMPONENT_PATH is MANDATORY for Make.config and Make.rules
#-------------------------------
# Default Config
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedConfig.mk

#----------------------------

#enable dynamic debug trac	
DEBUG=1

ifdef DEBUG
# dynamic debug: can be enable/disable by setprop command
CPPFLAGS += -D__DEBUG
LIBNAME   = st_ext_imgcommon
INSTALL_LIB = libst_ext_imgcommon.a
else
CPPFLAGS += -DRELEASE -D__RELEASE
LIBNAME   = st_ext_imgcommon
INSTALL_LIB = libst_ext_imgcommon.a
endif

CPPFLAGS  += -I$(MULTIMEDIA_PATH)/shared/utils/include
CPPFLAGS  += -I$(MULTIMEDIA_PATH)/shared/host_trace
CPPFLAGS  += -DDBGT_CONFIG_DEBUG -DDBGT_CONFIG_AUTOVAR
CPPFLAGS  += -DDBGT_TAG=\"ExtCamera\"
LDFLAGS += -llog -lcutils -lstelpcutils

VPATH += src/framework \
		 src/isp_common

LIBOBJ   = 	qhsm \
			grabctlcommon \
			ext_ispctl_component_manager\
			ext_ispctl_communication_manager\
			opmode_transition \
			hsm \
			error \
			omx_state_manager


INSTALL_HEADER = $(subst include/,,$(shell find include -type f -name "*.h" ))

CPPFLAGS+= \
		-I . \
		-I $(MMROOT)/imaging/include \
		-I $(REPOSITORY) 


# Generation of solibs
IS_OMX_COMPONENT_LIB:=false
include $(MM_MAKEFILES_DIR)/STEOMXLoaderConfig.mk

SOLIBNAMES+= ste_ext_ens_image_common
LIBOBJ_$(SOLIBNAMES) = ste_ens_imaging_weakcomponents
IMAGE_LIBS = $(LIBNAME) st_ifm st_ext_sia_itfs

SOLIBS_$(SOLIBNAMES) += $(IMAGING_NMF_HOST_COMPONENTS_LIBS)
EXTRALIBS_$(SOLIBNAMES)= $(IMAGE_LIBS)

FLAGS_$(SOLIBNAMES) = $(LDFLAGS_STE_ENS_OMX_COMPONENTS)
INSTALL_SOLIB = $(SOLIBNAMES)
		
todo: lib solib

TOBECLEAN = 

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk
