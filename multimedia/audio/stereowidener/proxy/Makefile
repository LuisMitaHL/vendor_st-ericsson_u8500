MMBOARD=x
MMSYSTEM=x

include $(COMPONENT_TOP_DIR)/make.ost_conf
# 0x20031554 537072981 is the ID reseverd for the audiocodec proxy
OST_COMPONENT_ID = 537072978
OST_DICO_NAME = $(OST_COMPONENT_NAME)_proxy
OST_SRC_DIR = .
INSTALL_DICO = $(OST_DICO_NAME).xml
#OST_TC_BYPASS = 1

include $(MM_MAKEFILES_DIR)/AudioConfig.mk

VPATH+=$(COMPONENT_TOP_DIR)/$(BASE_REPOSITORY)/host
CPPFLAGS+= -I$(COMPONENT_TOP_DIR)/$(BASE_REPOSITORY)

LIBOBJ  = AudioVirtualizer AudioVirtualizerNmfMpc 
LIBOBJ += stereowidener_cpp-user2mpc-stubs 
LIBNAME = st_AudioVirtualizer
INSTALL_LIB=libst_AudioVirtualizer.a

INSTALL_HEADER  = AudioVirtualizer.h  

todo: lib
include $(MM_MAKEFILES_DIR)/SharedRules.mk
