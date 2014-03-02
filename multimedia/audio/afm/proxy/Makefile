# vim:syntax=make:
# -*- Mode: Makefile -*-

#-------------------------------
# Default Config
#-------------------------------
MMBOARD=x
MMSYSTEM=x

include $(COMPONENT_TOP_DIR)/make.ost_conf

include $(MM_MAKEFILES_DIR)/AudioConfig.mk

OST_COMPONENT_ID = $(shell expr $(OST_MODULE_ID) + 1 )
OST_DICO_NAME = $(OST_COMPONENT_NAME)_proxy
OST_SRC_DIR = base bindings
CPPFLAGS+= -I$(OST_COMPONENT_INC)/autogentraces
INSTALL_DICO=$(OST_DICO_NAME).xml

#----------------------------
#     Entry for Make.rules
#
#VPATH    : Search Path for prerequisites.(see default value in Make.rules)
#CFLAGS   : Extra flags to give to the C compiler. (see default value in Make.rules)
#CXXFLAGS : Extra flags to give to the C++ compiler. (see default value in Make.rules)
#CPPFLAGS : Extra flags to give to the C preprocessor.(see default value in Make.rules)
#
#DEP_DEP  : Dependencies of dependency files. 
#           (for example generated header file before generating *.d)
#TOBECLEAN: List of files you want to delete on realclean target.
#
#LIBNAME  : Name of the library without the prefix lib.
#LIBOBJ   : List of object file to put in the lib
#
#EXENAME  : Name of executable
#EXEOBJ   : List of object file needed to build executable
#         For multiple executable, suffix these to variable with _1, _2 ...  
#
#----------------------------
CPPFLAGS+= -I$(MM_AUDIO_NMF_REPO_DIR) -I../nmf/mpc/$(BASE_REPOSITORY) -Iinclude -Ibindings -Ibase

VPATH+= bindings base ../nmf/mpc/$(BASE_REPOSITORY)/host $(INSTALL_NMF_REPO_BOARD_DIR)/host

LIBOBJ   = \
		AFM_Component \
		AFM_Port \
		AFM_PcmPort \
		AFM_Decoder \
		AFM_Encoder \
		AFM_PcmProcessing \
		AFM_PcmProcessingInputPort \
		AFM_Utils \
		AFMNmfHost_ProcessingComp \
		AFMNmfHost_Decoder \
		AFMNmfHost_Encoder \
		AFMNmfHost_PcmProcessing \
		AFM_HostController \
		HostBindings \
		HostSHMin \
		HostSHMout \
		HostSHM_PcmIn \
		HostSHM_PcmOut \
		NmfHostPcmProcessing \
		NmfComponent \
		HostPcmAdapter \
		AudioEffect

ifeq ($(CORE_NUMBER),2) # means the mpc processing components need to be built
LIBOBJ +=  afm_cpp-user2mpc-stubs afm_cpp-mpc2user-skels libeffects_cpp-user2mpc-stubs \
		AFM_nmf \
		AFM_MpcController \
		AFMNmfMpc_ProcessingComp \
		AFMNmfMpc_Decoder \
		AFMNmfMpc_Encoder \
		AFMNmfMpc_PcmProcessing \
		NmfMpcPcmProcessing \
		NmfMpcPcmAdapter \
		MpcBindings \
		PcmAdapter \
		SHMin \
		SHMout \
		SHM_PcmIn \
		SHM_PcmOut \
		AFMNmfHostMpc_ProcessingComp \
		AFM_MpcHeapMgr \
		AFM_MpcPcmProbe \
		AudioEffectMpc \
		AudioEffectMpcUtils
endif

ifdef DEBUG
CFLAGS=-g -O0
LIBNAME   = st_afm_debug
INSTALL_LIB= libst_afm_debug.a
CPPFLAGS += -DDEBUG -D__DEBUG
OBJ_EXT =odebug 
else
LIBNAME   = st_afm
INSTALL_LIB= libst_afm.a
CPPFLAGS += -DRELEASE -D__RELEASE
endif

ifeq ($(PLATFORM),x86-linux)
CPPFLAGS += -DAFM_X86
endif

ifeq (u5500,$(filter u5500%,$(STE_PLATFORM)))
  CPPFLAGS+=-DSTE_PLATFORM_U5500
endif

ifeq ($(MMPROBE_ENABLE_FEATURE_MMPROBE),true)
  CPPFLAGS+=-DMMPROBE_ENABLED
endif

INSTALL_HEADER= AFM_Component.h  AFM_Decoder.h  AFM_Encoder.h \
		AFM_Index.h  AFM_Port.h  AFM_Types.h AFM_Core.h \
		AFM_PcmPort.h NmfComponent.h \
		NmfMpcPcmAdapter.h NmfMpcPcmProcessing.h \
		AFM_PcmProcessing.h AFMNmfMpc_ProcessingComp.h \
		AFMNmfMpc_Decoder.h AFMNmfMpc_Encoder.h \
		AFMNmfHost_Decoder.h AFMNmfHost_Encoder.h \
		AFMNmfMpc_PcmProcessing.h AFM_MemoryPreset.h \
		AFM_nmf.h AFM_PcmProcessingInputPort.h \
		AFM_Utils.h AFM_macros.h \
		AFM_Controller.h AFM_MpcController.h AFM_HostController.h \
		NmfHostPcmProcessing.h AFMNmfHost_ProcessingComp.h \
		AFMNmfHost_PcmProcessing.h HostBindings.h Bindings.h \
		AFM_Symbian_AudioExt.h AFMNmfHostMpc_ProcessingComp.h \
		AFM_MpcHeapMgr.h AFM_MpcPcmProbe.h AudioEffect.h AudioEffectMpc.h \
		AudioEffectMpcConfigs.h AudioEffectMpcUtils.h OMX_STE_AudioEffectsExt.h

#WARNING todo target is MANDATORY (exe and/or lib)
todo: lib 

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

#-------------------------------
# local Rules
#-------------------------------
test cleantest:
	@echo "Test to be implemented ..."
