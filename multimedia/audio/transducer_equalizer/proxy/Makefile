MMBOARD=x
MMSYSTEM=x

include $(COMPONENT_TOP_DIR)/make.ost_conf

OST_COMPONENT_ID = $(KUidTopDicStTransducerEqualizer)
OST_DICO_NAME = $(OST_COMPONENT_NAME)_proxy
OST_SRC_DIR = .
INSTALL_DICO = $(OST_DICO_NAME).xml

OST_TOP_DICO_LIST = $(OST_COMPONENT_NAME)::$(KUidTopDicStTransducerEqualizer)

include $(MM_MAKEFILES_DIR)/AudioConfig.mk

VPATH +=../$(BASE_REPOSITORY)/host
CPPFLAGS += -I../$(BASE_REPOSITORY) -I$(COMPONENT_TOP_DIR)/effect/include 
CPPFLAGS+=-I$(MM_NMF_COMP_SRC_COMMON_DIR)

LIBOBJ  = TransducerEqualizer TEQNmfHost
ifeq ($(CORE_NUMBER),2)
LIBOBJ += TEQNmfMpc transducer_equalizer_cpp-user2mpc-stubs 
endif
LIBNAME = st_TransducerEqualizer

LIBOBJ_2 = TEQ_AudioEffect TEQMpc_AudioEffect
LIBNAME_2 = st_TEQ_AudioEffect

INSTALL_LIB = libst_TransducerEqualizer.a libst_TEQ_AudioEffect.a

INSTALL_HEADER = TransducerEqualizer.h TEQ_AudioEffectCreateMethod.h

todo: lib
include $(MM_MAKEFILES_DIR)/SharedRules.mk
