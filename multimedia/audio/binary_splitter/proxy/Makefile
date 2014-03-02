MMBOARD=x
MMSYSTEM=x

include $(COMPONENT_TOP_DIR)/make.ost_conf

OST_COMPONENT_ID = $(KUidDicStAudioBinarySplitterProxy)
OST_DICO_NAME = $(OST_COMPONENT_NAME)_proxy
OST_SRC_DIR = .
CPPFLAGS+= -I$(OST_COMPONENT_INC)/autogentraces
INSTALL_DICO = $(OST_DICO_NAME).xml

OST_TOP_DICO_LIST = $(OST_COMPONENT_NAME)::$(KUidTopDicStBinarySplitter)
OST_TOP_DICO_LIST += $(OST_COMPONENT_NAME)_HOST::$(KUidTopDicStBinarySplitterHost)

include $(MM_MAKEFILES_DIR)/AudioConfig.mk

VPATH+=$(COMPONENT_TOP_DIR)/$(BASE_REPOSITORY)/host
CPPFLAGS+= -I$(COMPONENT_TOP_DIR)/$(BASE_REPOSITORY)


LIBOBJ  = BinarySplitter BinarySplitterNmfHost BinarySplitter_PcmPort
INSTALL_HEADER  =  BinarySplitter.h BinarySplitter_PcmPort.h

ifeq ($(CORE_NUMBER),2)
LIBOBJ  += BinarySplitterNmfMpc
LIBOBJ += binary_splitter_cpp-user2mpc-stubs 
#INSTALL_HEADER  =  
endif

ifeq (u5500,$(filter u5500%,$(STE_PLATFORM)))
  CPPFLAGS+=-DSTE_PLATFORM_U5500
endif

ifdef DEBUG
CFLAGS=-g -O0
LIBNAME = st_BinarySplitter
INSTALL_LIB=libst_BinarySplitter.a
CPPFLAGS += -DDEBUG -D__DEBUG
OBJ_EXT =odebug 
else
LIBNAME = st_BinarySplitter
INSTALL_LIB=libst_BinarySplitter.a
CPPFLAGS += -DRELEASE -D__RELEASE
endif

todo: lib
include $(MM_MAKEFILES_DIR)/SharedRules.mk

