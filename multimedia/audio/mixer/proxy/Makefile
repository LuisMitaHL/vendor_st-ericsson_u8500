MMBOARD=x
MMSYSTEM=x

include $(COMPONENT_TOP_DIR)/make.ost_conf

OST_COMPONENT_ID = $(KUidDicStAudioMixerProxy)
OST_DICO_NAME = $(OST_COMPONENT_NAME)_proxy
OST_SRC_DIR = .
INSTALL_DICO = $(OST_DICO_NAME).xml

OST_TOP_DICO_LIST = $(OST_COMPONENT_NAME)::$(KUidTopDicStAudioMixer)
OST_TOP_DICO_LIST += $(OST_COMPONENT_NAME)_host::$(KUidTopDicStAudioHostMixer)


include $(MM_MAKEFILES_DIR)/AudioConfig.mk

VPATH+=$(COMPONENT_TOP_DIR)/$(BASE_REPOSITORY)/host
CPPFLAGS+= -I$(COMPONENT_TOP_DIR)/$(BASE_REPOSITORY) -I$(COMPONENT_TOP_DIR)/..

CPPFLAGS+= -I$(COMPONENT_TOP_DIR)/common/include -I$(COMPONENT_TOP_DIR)/effect/include

LIBOBJ  = MixerEffectsRegister MixerPort MixerInputPort MixerOutputPort Mixer MixerNmfHost 

ifeq ($(CORE_NUMBER),2)
LIBOBJ += mixer_cpp-user2mpc-stubs mixer_cpp-mpc2user-skels MixerNmfMpc 
endif

LIBNAME = st_Mixer
INSTALL_LIB=libst_Mixer.a

todo: lib
include $(MM_MAKEFILES_DIR)/SharedRules.mk
