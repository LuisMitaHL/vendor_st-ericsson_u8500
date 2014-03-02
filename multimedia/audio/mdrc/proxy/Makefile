MMBOARD=x
MMSYSTEM=x

include $(COMPONENT_TOP_DIR)/make.ost_conf

# 0x20031554 537072981 is the ID reseverd for the audiocodec proxy
OST_COMPONENT_ID = 537072964
OST_DICO_NAME = $(OST_COMPONENT_NAME)_proxy
OST_SRC_DIR = .
INSTALL_DICO = $(OST_DICO_NAME).xml
#OST_TC_BYPASS = 1

include $(MM_MAKEFILES_DIR)/AudioConfig.mk

VPATH+=../../mdrc/$(BASE_REPOSITORY)/host
CPPFLAGS+= -I../../mdrc/$(BASE_REPOSITORY) -I$(COMPONENT_TOP_DIR)/effect/include
CPPFLAGS+=-I$(MM_NMF_COMP_SRC_COMMON_DIR)/libeffects/libmdrc5b/include/

LIBOBJ  = Mdrc MdrcNmfHost MdrcPcmPort
ifeq ($(CORE_NUMBER),2)
LIBOBJ += MdrcNmfMpc mdrc_cpp-user2mpc-stubs 
endif
LIBNAME = st_Mdrc

LIBOBJ_2 = MDRC_AudioEffect MDRCMpc_AudioEffect
LIBNAME_2 = st_MDRC_AudioEffect

INSTALL_LIB=libst_Mdrc.a libst_MDRC_AudioEffect.a

INSTALL_HEADER  = Mdrc.h MDRC_AudioEffectCreateMethod.h

todo: lib
include $(MM_MAKEFILES_DIR)/SharedRules.mk
