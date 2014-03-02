MMBOARD=x
MMSYSTEM=x
include $(MM_MAKEFILES_DIR)/AudioConfig.mk

VPATH+=$(COMPONENT_TOP_DIR)/$(BASE_REPOSITORY)/host
CPPFLAGS+= -I$(COMPONENT_TOP_DIR)/$(BASE_REPOSITORY)

LIBOBJ = VirtualSurroundPcmProcessing VirtualSurroundNmfHost_PcmProcessing VirtualSurround_PcmPort
INSTALL_HEADER  = VirtualSurroundPcmProcessing.h OMX_STE_VirtualSurroundExt.h

ifeq ($(MMCOMPILER),gnu)
ifeq ($(findstring x86,$(MMPROCESSOR)),x86)
CFLAGS  += -fno-strict-aliasing
endif
endif

ifdef VIRTUAL_SURROUND_DEBUG
CFLAGS  += -g -O0
CPPFLAGS+= -DDEBUG -D__DEBUG -DAFM_VIRTUAL_SURROUND_DEBUG -DAFM_VIRTUAL_SURROUND_PORT_DEBUG
OBJ_EXT  = odebug
else
CPPFLAGS += -DRELEASE -D__RELEASE
endif

LIBNAME = st_VirtualSurround
INSTALL_LIB=libst_VirtualSurround.a

todo: lib
include $(MM_MAKEFILES_DIR)/SharedRules.mk
