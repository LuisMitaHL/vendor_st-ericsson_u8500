MMBOARD=x
MMSYSTEM=x

include $(COMPONENT_TOP_DIR)/make.ost_conf

include $(MM_MAKEFILES_DIR)/AudioConfig.mk

VPATH+=../../$(BASE_REPOSITORY)/host ../common
CPPFLAGS+= -I../../$(BASE_REPOSITORY) -I../common

LIBOBJ  =GSMFREnc_BitStreamPort GSMFREncoder GSMFRNmfHost_Encoder GSMFRHostEncAlgo  GSMFREnc_PcmPort
INSTALL_HEADER  =  GSMFREncoder.h GSMFREnc_BitStreamPort.h GSMFRHostEncAlgo.h GSMFREnc_PcmPort.h

ifdef DEBUG
CFLAGS=-g -O0
LIBNAME  = st_GSMFREncoder_debug
INSTALL_LIB = libst_GSMFREncoder_debug.a 
CPPFLAGS += -DDEBUG -D__DEBUG
OBJ_EXT =odebug 
else
LIBNAME = st_GSMFREncoder
INSTALL_LIB=libst_GSMFREncoder.a
CPPFLAGS += -DRELEASE -D__RELEASE
endif

todo: lib
include $(MM_MAKEFILES_DIR)/SharedRules.mk
