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

VPATH += src/resource_sharer_manager \
		src/framework \
		src/debug \
		src/isp_common \
		src/writeI2C \
		src/protothread \
		src/

ifdef DEBUG
#CPPFLAGS += -DOSI_DEBUG -D__DEBUG
#LIBNAME   = st_imgcommon_debug
CFLAGS=-g -O0
#INSTALL_LIB = libst_imgcommon_debug.a
#OBJ_EXT =odebug 
endif
#else
CPPFLAGS += -DRELEASE -D__RELEASE
LIBNAME   = st_imgcommon
INSTALL_LIB = libst_imgcommon.a
#endif

#CPPFLAGS += -DDEBUG_CHECKALIVE

#CR337836 Activate dynamic trace
CPPFLAGS += -DCONFIG_DEBUG -DDBGT_VAR=mDebugTraceVarImgcommon -DCONFIG_DEBUG_AUTOVAR

ifeq ($(MYMMBOARD),svp8500_v1)
CPPFLAGS += -D_SVP_
endif

#CPPFLAGS+=-D__DBGLOG
#CPPFLAGS+=-D__CAMERA_TRACE_ALL
CPPFLAGS += -D__CAMERA_ALL


LIBOBJ   = 	qhsm \
			qhsmtran \
			grabctlcommon \
			ispctl_eventclbk \
			ispctl_component_manager\
			ispctl_communication_manager\
			isp_configuring_pipe \
			isp_state_controlling \
			hsm \
			set_feature\
			trace \
			debugging \
			opmode_transition \
			resource_sharer_manager \
			protothread \
			writeI2C \
			comI2C \
			error \
			deferredeventmgr \
			omx_state_manager \
			defcor_damper \
			noise_filter_damper \
			noise_model_damper \
			rso_damper \
			scorpio_damper \
			scythe_damper \
			sharpening_damper_lr \
			sharpening_damper_hr \
			isp_dampers \
			picture_settings


INSTALL_HEADER = $(subst include/,,$(shell find include -type f -name "*.h" ))

CPPFLAGS+= -I $(MM_HEADER_DIR) \
	-I $(MM_NMF_REPO_DIR) 

CPPFLAGS+= -I ${MEDIUM_DIR}/$(BASE_REPOSITORY) 

ifeq ($(strip $(MM_BUILD_CAM_OMX_3A_EXTENSION)),true)
CPPFLAGS += -DCAMERA_ENABLE_OMX_3A_EXTENSION
LIBOBJ += \
	isp_dampers_ext
endif

# Generation of solibs
ifneq ($(findstring symbian,$(PLATFORM)),symbian)
ifneq ($(PLATFORM),x86-linux)
IS_OMX_COMPONENT_LIB:=false
include $(MM_MAKEFILES_DIR)/STEOMXLoaderConfig.mk

CPPFLAGS+= -I$(OST_COMPONENT_INC)/autogentraces

#CPPFLAGS+=-I$(STE_BELLAGIO_SRC_ROOT)/ens -I../common
SOLIBNAMES = ste_ens_image_common
LIBOBJ_$(SOLIBNAMES) = ste_ens_imaging_weakcomponents
IMAGE_LIBS = $(LIBNAME) st_ifm st_sia_itfs st_mmio st_damper
IMAGE_LIBS += st_i2c st_variationif
ifeq ($(DEBUG),1)
	IMAGE_LIBS += host_trace
endif
IMAGING_NMF_HOST_COMPONENTS = resource_sharerWrapped

SOLIBS_$(SOLIBNAMES) += $(IMAGING_NMF_HOST_COMPONENTS_LIBS)
EXTRALIBS_$(SOLIBNAMES)= $(IMAGE_LIBS)

FLAGS_$(SOLIBNAMES) = $(LDFLAGS_STE_ENS_OMX_COMPONENTS) -lste_ens_image_tuningdatabase
FLAGS_$(SOLIBNAMES) += -lste_isp8500_firmware_primary -lste_isp8500_firmware_secondary
INSTALL_SOLIB = $(SOLIBNAMES)
endif
endif


todo: lib solib

TOBECLEAN = 

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

ifeq ($(MMSYSTEM),symbian) 
NO_VARIANT=true
include ${MEDIUM_DIR}/symbian_makerules/mmh.mk
endif
