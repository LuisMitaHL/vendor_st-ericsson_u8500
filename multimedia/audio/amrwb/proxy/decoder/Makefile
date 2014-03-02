MMBOARD=x
MMSYSTEM=x
include $(COMPONENT_TOP_DIR)/make.ost_conf

include $(MM_MAKEFILES_DIR)/AudioConfig.mk

VPATH+=../../$(BASE_REPOSITORY)/host ../common
CPPFLAGS+= -I../../$(BASE_REPOSITORY) -I../common

LIBOBJ = AMRWBDec_BitstreamPort AMRWBDecoder AMRWBNmfHost_Decoder AMRWBHostDecAlgo AMRWBDec_PcmPort
INSTALL_HEADER  =  AMRWBDecoder.h AMRWBDec_BitstreamPort.h AMRWBHostDecAlgo.h AMRWBDec_PcmPort.h

ifeq ($(MYMMSYSTEM),android)
CPPFLAGS+= -DANDRO
endif

ifdef DEBUG
CFLAGS=-g -O0
LIBNAME  = st_AMRWBDecoder_debug
INSTALL_LIB = libst_AMRWBDecoder_debug.a 
CPPFLAGS += -DDEBUG -D__DEBUG
OBJ_EXT =odebug 
else
LIBNAME = st_AMRWBDecoder
INSTALL_LIB=libst_AMRWBDecoder.a
CPPFLAGS += -DRELEASE -D__RELEASE
endif

todo: lib
include $(MM_MAKEFILES_DIR)/SharedRules.mk
