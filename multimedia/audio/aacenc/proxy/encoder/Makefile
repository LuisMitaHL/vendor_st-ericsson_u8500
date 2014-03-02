MMBOARD=x
MMSYSTEM=x



include $(MM_MAKEFILES_DIR)/AudioConfig.mk

VPATH+=../../$(BASE_REPOSITORY)/host #../common
CPPFLAGS+= -I../../$(BASE_REPOSITORY) #-I../common


LIBOBJ = AACEncoder AACEnc_BitstreamPort AACNmfHost_Encoder AACHostEncAlgo
INSTALL_HEADER = AACEncoder.h AACEnc_BitstreamPort.h AACHostEncAlgo.h

ifdef DEBUG
CFLAGS=-g -O0
LIBNAME  = st_AACEncoder_debug
INSTALL_LIB = libst_AACEncoder_debug.a 
CPPFLAGS += -DDEBUG -D__DEBUG
OBJ_EXT =odebug 
else
LIBNAME = st_AACEncoder
INSTALL_LIB=libst_AACEncoder.a
CPPFLAGS += -DRELEASE -D__RELEASE
endif


todo: lib
include $(MM_MAKEFILES_DIR)/SharedRules.mk
