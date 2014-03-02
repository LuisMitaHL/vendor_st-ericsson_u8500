# vim:syntax=make:
# -*- Mode: Makefile -*-

#WARNING COMPONENT_PATH is MANDATORY for Make.config and Make.rules
#-------------------------------
# Default Config
#-------------------------------
include make.ost_conf
include $(MM_MAKEFILES_DIR)/SharedConfig.mk

#----------------------------


#DEBUG=1
#export DEBUG

ifdef LATENCY_LOG_ENABLE
# enable latencies measurement
CPPFLAGS += -DLATENCY_LOG_ENABLE
endif

VPATH += src/opmode_transition 

ifdef DEBUG
CPPFLAGS += -DOSI_DEBUG -D__DEBUG
LIBNAME   = st_ispproc_debug
CFLAGS=-g -O0
INSTALL_LIB = libst_ispproc_debug.a
OBJ_EXT =odebug 
else
CPPFLAGS += -DRELEASE -D__RELEASE
LIBNAME   = st_ispproc
INSTALL_LIB = libst_ispproc.a
endif


#CPPFLAGS+=-D__DBGLOG


LIBOBJ   = 	hsmispproc \
			ispproc \
			omxispproc \
			boot_statemachine \
			isp_configuring_pipe \
			opmode_statemachine \
			port_statemachine \
			freed_statemachine \
			capture_statemachine \
			memgrabctl \
			ispprocport

ifeq ($(MM_BUILD_CAM_OMX_3A_EXTENSION),true)
CPPFLAGS += -DCAMERA_ENABLE_OMX_3A_EXTENSION
endif

INSTALL_HEADER = $(subst include/,,$(shell find include -type f -name "*.h" ))

CPPFLAGS+= -I $(MM_HEADER_DIR) \
	-I $(MM_NMF_REPO_DIR) 

CPPFLAGS+= -I ${MEDIUM_DIR}/$(BASE_REPOSITORY) 

# CR337836 Activate dynamic trace
CPPFLAGS += -DCONFIG_DEBUG -DDBGT_VAR=mDebugTraceVarIspProc -DCONFIG_DEBUG_AUTOVAR

CPPFLAGS+= -I$(OST_COMPONENT_INC)/autogentraces

# Generation of solibs
ifneq ($(findstring symbian,$(PLATFORM)),symbian)
ifneq ($(PLATFORM),x86-linux)
IS_OMX_COMPONENT_LIB:=true
include $(MM_MAKEFILES_DIR)/STEOMXLoaderConfig.mk

SOLIBNAMES = ste_ens_ispproc
LIBOBJ_$(SOLIBNAMES) = ste_omx_ens_component
IMAGE_LIBS = $(LIBNAME)

EXTRALIBS_$(SOLIBNAMES) = $(IMAGE_LIBS)
FLAGS_$(SOLIBNAMES) += -lste_ens_image_common -lste_ens_image_tuningdatabase
FLAGS_$(SOLIBNAMES) += $(LDFLAGS_STE_ENS_OMX_COMPONENTS)
FLAGS_$(SOLIBNAMES) += -lste_isp8500_firmware_primary
INSTALL_SOLIB = $(SOLIBNAMES)

endif
endif

todo: lib solib

TOBECLEAN = 

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk


ifeq ($(findstring symbian,$(PLATFORM)),symbian)
 NO_VARIANT=true
 include ${MEDIUM_DIR}/symbian_makerules/mmh.mk
endif
