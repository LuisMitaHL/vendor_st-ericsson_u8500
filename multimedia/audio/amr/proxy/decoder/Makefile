MMBOARD=x
MMSYSTEM=x

include $(COMPONENT_TOP_DIR)/make.ost_conf

include $(MM_MAKEFILES_DIR)/AudioConfig.mk

VPATH+=$(COMPONENT_TOP_DIR)/$(BASE_REPOSITORY)/host $(COMPONENT_TOP_DIR)/common
CPPFLAGS+= -I$(COMPONENT_TOP_DIR)$(BASE_REPOSITORY) -I$(COMPONENT_TOP_DIR)/common

LIBOBJ = AMRDec_BitstreamPort AMRDecoder AMRNmfHost_Decoder AMRHostDecAlgo AMRDec_PcmPort
INSTALL_HEADER  =  AMRDecoder.h AMRDec_BitstreamPort.h AMRHostDecAlgo.h AMRDec_PcmPort.h

ifeq ($(CORE_NUMBER),2)
#add mpc processing components
LIBOBJ += amr_cpp-user2mpc-stubs AMRNmfMpc_Decoder AMRDecAlgo
INSTALL_HEADER += AMRDecAlgo.h
endif

ifeq ($(MYMMSYSTEM),android)
CPPFLAGS+= -DFORCE_RTP_WHEN_FSF
endif

ifdef DEBUG
CFLAGS=-g -O0
LIBNAME  = st_AMRDecoder_debug
INSTALL_LIB = libst_AMRDecoder_debug.a 
CPPFLAGS += -DDEBUG -D__DEBUG
OBJ_EXT =odebug 
else
LIBNAME = st_AMRDecoder
INSTALL_LIB=libst_AMRDecoder.a
CPPFLAGS += -DRELEASE -D__RELEASE
endif

todo: lib
include $(MM_MAKEFILES_DIR)/SharedRules.mk
