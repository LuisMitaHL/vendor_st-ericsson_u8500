MMBOARD=x
MMSYSTEM=x

include $(COMPONENT_TOP_DIR)/make.ost_conf

include $(MM_MAKEFILES_DIR)/AudioConfig.mk

VPATH+=../../$(BASE_REPOSITORY)/host #../common
CPPFLAGS+= -I../../$(BASE_REPOSITORY) #-I../common

LIBOBJ = AMRWBEncoder AMRWBEnc_BitstreamPort AMRWBNmfHost_Encoder AMRWBHostEncAlgo AMRWBEnc_PcmPort
INSTALL_HEADER = AMRWBEncoder.h AMRWBEnc_BitstreamPort.h AMRWBHostEncAlgo.h AMRWBEnc_PcmPort.h

ifeq ($(MYMMSYSTEM),android)
CPPFLAGS+= -DANDRO -DFORCE_RTP_WHEN_FSF
endif

ifdef DEBUG
CFLAGS=-g -O0
LIBNAME  = st_AMRWBEncoder_debug
INSTALL_LIB = libst_AMRWBEncoder_debug.a 
CPPFLAGS += -DDEBUG -D__DEBUG
OBJ_EXT =odebug 
else
LIBNAME = st_AMRWBEncoder
INSTALL_LIB=libst_AMRWBEncoder.a
CPPFLAGS += -DRELEASE -D__RELEASE
endif


todo: lib
include $(MM_MAKEFILES_DIR)/SharedRules.mk
