MMBOARD=x
MMSYSTEM=x

include $(COMPONENT_TOP_DIR)/make.ost_conf

#OST_COMPONENT_ID = 537072974
OST_COMPONENT_ID = $(KUidDicStAudioPcmSplitterProxy)
OST_DICO_NAME = $(OST_COMPONENT_NAME)_proxy
OST_SRC_DIR = .
CPPFLAGS+= -I$(OST_COMPONENT_INC)/autogentraces
INSTALL_DICO = $(OST_DICO_NAME).xml

OST_TOP_DICO_LIST = $(OST_COMPONENT_NAME)::$(KUidTopDicStPcmSplitter)
OST_TOP_DICO_LIST += $(OST_COMPONENT_NAME)_host::$(KUidTopDicStAudioPcmSplitterHost)

include $(MM_MAKEFILES_DIR)/AudioConfig.mk

VPATH+=$(COMPONENT_TOP_DIR)/$(BASE_REPOSITORY)/host
CPPFLAGS+= -I$(COMPONENT_TOP_DIR)/$(BASE_REPOSITORY) -I$(COMPONENT_TOP_DIR)/..

CPPFLAGS+= -I$(MM_HEADER_DIR)/samplerateconv -I$(MM_HEADER_DIR)/volctrl -I$(MM_HEADER_DIR)/updownmix

LIBOBJ  = PcmSplitter_PcmPort PcmSplitter PcmSplitterNmfHost

ifeq ($(CORE_NUMBER),2)
LIBOBJ += pcm_splitter_cpp-user2mpc-stubs PcmSplitterNmfMpc
endif

LIBNAME = st_PcmSplitter
INSTALL_LIB=libst_PcmSplitter.a

INSTALL_HEADER  =  PcmSplitter.h

todo: lib
include $(MM_MAKEFILES_DIR)/SharedRules.mk
