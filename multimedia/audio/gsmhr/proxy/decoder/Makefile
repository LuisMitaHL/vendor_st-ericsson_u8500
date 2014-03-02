MMBOARD=x
MMSYSTEM=x
include $(COMPONENT_TOP_DIR)/make.ost_conf

include $(MM_MAKEFILES_DIR)/AudioConfig.mk

VPATH+=../../$(BASE_REPOSITORY)/host ../common
CPPFLAGS+= -I../../$(BASE_REPOSITORY) -I../common

LIBOBJ = GSMHRDecoder GSMHRDec_BitstreamPort GSMHRNmfHost_Decoder GSMHRHostDecAlgo GSMHRDec_PcmPort
INSTALL_HEADER = GSMHRDecoder.h GSMHRDec_BitstreamPort.h GSMHRHostDecAlgo.h GSMHRDec_PcmPort.h

ifdef DEBUG
CFLAGS=-g -O0
LIBNAME  = st_GSMHRDecoder_debug
INSTALL_LIB = libst_GSMHRDecoder_debug.a 
CPPFLAGS += -DDEBUG -D__DEBUG
OBJ_EXT =odebug 
else
LIBNAME = st_GSMHRDecoder
INSTALL_LIB=libst_GSMHRDecoder.a
CPPFLAGS += -DRELEASE -D__RELEASE
endif


todo: lib
include $(MM_MAKEFILES_DIR)/SharedRules.mk
