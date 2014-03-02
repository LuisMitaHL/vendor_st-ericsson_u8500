MMBOARD=x
MMSYSTEM=x

include $(COMPONENT_TOP_DIR)/make.ost_conf
include $(MM_MAKEFILES_DIR)/AudioConfig.mk

OST_COMPONENT_ID = $(KUidDicStComfortNoiseProxy)
OST_DICO_NAME = $(OST_COMPONENT_NAME)_proxy
OST_SRC_DIR = .
INSTALL_DICO = $(OST_DICO_NAME).xml

VPATH+=../$(BASE_REPOSITORY)/host
CPPFLAGS+= -I ../$(BASE_REPOSITORY) -I ../effect/include

ifeq ($(CORE_NUMBER),2)
#For MMDSP
LIBOBJ  = Cng CngNmfMpc
LIBOBJ += comfortnoise_cpp-user2mpc-stubs 
LIBNAME = st_ComfortNoise
INSTALL_LIB = libst_ComfortNoise.a
endif

INSTALL_HEADER  = Cng.h OMX_CNGExt.h

ifdef CNG_DEBUG
CFLAGS  += -g -O0
CPPFLAGS+= -DAFM_CNG_DEBUG
OBJ_EXT  = odebug 
endif

todo: lib
include $(MM_MAKEFILES_DIR)/SharedRules.mk
