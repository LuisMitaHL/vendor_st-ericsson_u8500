MMBOARD=x
MMSYSTEM=x

include $(COMPONENT_TOP_DIR)/make.ost_conf

include $(MM_MAKEFILES_DIR)/AudioConfig.mk

VPATH+=$(COMPONENT_TOP_DIR)/$(BASE_REPOSITORY)/host

LIBOBJ = AMREncoder AMREnc_BitstreamPort AMRNmfHost_Encoder AMRHostEncAlgo AMREnc_PcmPort
INSTALL_HEADER = AMREncoder.h AMREnc_BitstreamPort.h AMRHostEncAlgo.h AMREnc_PcmPort.h

ifeq ($(CORE_NUMBER),2)
LIBOBJ += amr_cpp-user2mpc-stubs AMRNmfMpc_Encoder AMREncAlgo
INSTALL_HEADER  += AMREncAlgo.h
endif

ifeq ($(MYMMSYSTEM),android)
CPPFLAGS+= -DFORCE_RTP_WHEN_FSF -DFORCE_DTXOFF
endif

ifdef DEBUG
CFLAGS=-g -O0
LIBNAME  = st_AMREncoder_debug
INSTALL_LIB = libst_AMREncoder_debug.a 
CPPFLAGS += -DDEBUG -D__DEBUG
OBJ_EXT =odebug 
else
LIBNAME = st_AMREncoder
INSTALL_LIB=libst_AMREncoder.a
CPPFLAGS += -DRELEASE -D__RELEASE
endif


todo: lib
include $(MM_MAKEFILES_DIR)/SharedRules.mk
