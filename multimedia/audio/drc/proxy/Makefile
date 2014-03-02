MMBOARD=x
MMSYSTEM=x
include $(MM_MAKEFILES_DIR)/AudioConfig.mk

VPATH+=$(COMPONENT_TOP_DIR)/$(BASE_REPOSITORY)/host
CPPFLAGS+= -I$(COMPONENT_TOP_DIR)/$(BASE_REPOSITORY)

LIBOBJ = DRCPcmProcessing DRCNmfHost_PcmProcessing
INSTALL_HEADER  = DRCPcmProcessing.h OMX_DRCExt.h 

ifeq ($(MMCOMPILER),gnu)
ifeq ($(findstring x86,$(MMPROCESSOR)),x86)
CFLAGS  += -fno-strict-aliasing
endif
endif

ifdef DRC_DEBUG
CFLAGS  += -g -O0
CPPFLAGS+= -DDEBUG -D__DEBUG -DAFM_DRC_DEBUG
OBJ_EXT  = odebug 
else
CPPFLAGS += -DRELEASE -D__RELEASE
endif

LIBNAME = st_Drc
INSTALL_LIB=libst_Drc.a

todo: lib
include $(MM_MAKEFILES_DIR)/SharedRules.mk
