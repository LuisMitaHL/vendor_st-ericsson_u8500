MMBOARD=x
MMSYSTEM=x
include $(COMPONENT_TOP_DIR)/make.ost_conf

include $(MM_MAKEFILES_DIR)/AudioConfig.mk

VPATH+=../../$(BASE_REPOSITORY)/host #../common
CPPFLAGS+= -I../../$(BASE_REPOSITORY) #-I../common

LIBOBJ = GSMHREncoder GSMHREnc_BitstreamPort GSMHRNmfHost_Encoder GSMHRHostEncAlgo GSMHREnc_PcmPort
INSTALL_HEADER = GSMHREncoder.h GSMHREnc_BitstreamPort.h GSMHRHostEncAlgo.h GSMHREnc_PcmPort.h

ifdef DEBUG
CFLAGS=-g -O0
LIBNAME  = st_GSMHREncoder_debug
INSTALL_LIB = libst_GSMHREncoder_debug.a 
CPPFLAGS += -DDEBUG -D__DEBUG
OBJ_EXT =odebug 
else
LIBNAME = st_GSMHREncoder
INSTALL_LIB=libst_GSMHREncoder.a
CPPFLAGS += -DRELEASE -D__RELEASE
endif


todo: lib
include $(MM_MAKEFILES_DIR)/SharedRules.mk
