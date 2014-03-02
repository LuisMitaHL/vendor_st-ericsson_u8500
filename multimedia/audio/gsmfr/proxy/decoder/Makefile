MMBOARD=x
MMSYSTEM=x

include $(COMPONENT_TOP_DIR)/make.ost_conf

include $(MM_MAKEFILES_DIR)/AudioConfig.mk

VPATH+=../../$(BASE_REPOSITORY)/host ../common
CPPFLAGS+= -I../../$(BASE_REPOSITORY) -I../common

LIBOBJ  =GSMFRDec_BitStreamPort GSMFRDecoder GSMFRNmfHost_Decoder GSMFRHostDecAlgo  GSMFRDec_PcmPort
INSTALL_HEADER  =  GSMFRDecoder.h GSMFRDec_BitStreamPort.h GSMFRHostDecAlgo.h GSMFRDec_PcmPort.h

ifdef DEBUG
CFLAGS=-g -O0
LIBNAME  = st_GSMFRDecoder_debug
INSTALL_LIB = libst_GSMFRDecoder_debug.a 
CPPFLAGS += -DDEBUG -D__DEBUG
OBJ_EXT =odebug 
else
LIBNAME = st_GSMFRDecoder
INSTALL_LIB=libst_GSMFRDecoder.a
CPPFLAGS += -DRELEASE -D__RELEASE
endif

todo: lib
include $(MM_MAKEFILES_DIR)/SharedRules.mk
