MMBOARD=x
MMSYSTEM=x

ifndef COMPONENT_TOP_DIR
  COMPONENT_TOP_DIR := $(PWD)/..
endif

include $(COMPONENT_TOP_DIR)/make.ost_conf
OST_COMPONENT_ID = 537069232
OST_DICO_NAME = $(OST_COMPONENT_NAME)_proxy
CPPFLAGS+= -I$(OST_COMPONENT_INC)/autogentraces
OST_SRC_DIR = .

INSTALL_DICO = $(OST_DICO_NAME).xml



include $(MM_MAKEFILES_DIR)/AudioConfig.mk

VPATH+=../../spl/$(BASE_REPOSITORY)/host
CPPFLAGS+= -I../../spl/$(BASE_REPOSITORY)

LIBOBJ  = SPL SPLNmfMpc 
LIBOBJ += spl_cpp-user2mpc-stubs
LIBNAME = st_SPL

LIBOBJ_2 = SPL_AudioEffect SPLMpc_AudioEffect
LIBNAME_2 = st_SPL_AudioEffect

INSTALL_LIB=libst_SPL.a libst_SPL_AudioEffect.a

INSTALL_HEADER  = SPL.h SPL_AudioEffectCreateMethod.h

todo: lib
include $(MM_MAKEFILES_DIR)/SharedRules.mk
