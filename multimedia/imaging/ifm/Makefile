# vim:syntax=make:
# -*- Mode: Makefile -*-

#WARNING COMPONENT_PATH is MANDATORY for Make.config and Make.rules
#-------------------------------
# Default Config
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedConfig.mk


ifeq ($(MMSYSTEM),symbian) 
#CPPFLAGS += -DSTAB #disabled for symbian because of buffer size incompatibility with GfxSink 
else ifeq ($(MMSYSTEM),android) 
  CPPFLAGS += -DSTAB 
  CPPFLAGS += -DFREQ_SCALING_ALLOWED
else 
  CPPFLAGS += -DSTAB 
endif

# very ugly hack needed because *x*rvct* is not "properly" in include path (with or without "host/")

CPPFLAGS+=-I$(MM_SHARED_NMF_REPO_DIR)/$(MMPROCESSOR)-x-$(MMCOMPILER)-x \
	  -I$(MM_SHARED_NMF_REPO_DIR)/$(MMPROCESSOR)-x-$(MMCOMPILER)-x/host
CPPFLAGS+= -I ${MEDIUM_DIR}/$(BASE_REPOSITORY) 
CPPFLAGS	+= -DFREQ_SCALING_ALLOWED

ifdef DEBUG
CPPFLAGS    += -DDEBUG
CPPFLAGS    += -D__DEBUG
CPPFLAGS    += -DOSI_DEBUG
CPPFLAGS    += -DDISPLAY_EXIF_METADATA
LIBNAME     = st_ifm_debug
OBJ_EXT     = odebug 
INSTALL_LIB = libst_ifm_debug.a
else
CPPFLAGS    += -DRELEASE
CPPFLAGS    += -D__RELEASE
LIBNAME     = st_ifm
INSTALL_LIB = libst_ifm.a
endif


ifeq ($(PLATFORM),x86-linux)
#To Do: this should be enabled for ifmport extradata exif
else
# CR337836 Activate dynamic trace
CPPFLAGS += -DCONFIG_DEBUG -DDBGT_VAR=mDebugTraceVarIfm -DCONFIG_DEBUG_AUTOVAR
endif

LIBOBJ   = ifmport extradata exif
ifeq ($(PLATFORM),x86-linux)
else
LIBOBJ   += IFM_HostNmfProcessingComp IFM_NmfProcessingComp imgTest_draw variationif sensorconfigurable ExifReader sensor
endif
ifeq ($(MM_BUILD_CAM_OMX_3A_EXTENSION),true)
LIBOBJ += \
	extradata_wrapper
endif
LIBNAME_2 = st_sensor
INSTALL_LIB += libst_sensor.a
LIBOBJ_2 = sensor

INSTALL_HEADER = ifmport.h IFM_Index.h IFM_Types.h IFM_Trace.h extradata.h exif.h imgTest_draw.h ExifReader.h

ifeq ($(PLATFORM),x86-linux)
else
INSTALL_HEADER += IFM_HostNmfProcessingComp.h IFM_NmfProcessingComp.h sensor.h comI2C.h variationif.h sensorconfigurable.h
endif

#WARNING todo target is MANDATORY (exe and/or lib)
todo: lib

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

ifeq ($(MMSYSTEM),symbian) 
NO_VARIANT=true
include ${MMROOT}/imaging/symbian_makerules/mmh.mk
endif

