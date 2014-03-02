# vim:syntax=make:
# -*- Mode: Makefile -*-

#-------------------------------
# Default Config
#-------------------------------
MMSYSTEM=x
#MMBOARD=x

include ../make.ost_conf
#KStEnsOstTraceProxy   = KStUidCommonDefine018 = 0x2003064D = 537069133
OST_COMPONENT_ID = 537069133
OST_DICO_NAME = $(OST_COMPONENT_NAME)_proxy
OST_SRC_DIR = ENS_Fsm ENS_Component
CPPFLAGS+= -I$(OST_COMPONENT_INC)/autogentraces

INSTALL_DICO+=ENS_proxy.xml
include $(MM_MAKEFILES_DIR)/SharedConfig.mk

ifndef ENS_VERSION
$(error ENS_VERSION must be defined)
endif

ifndef MEDIUM
$(error MEDIUM must be defined)
endif

ifeq ($(MEDIUM),shared)
LIBSUFFIX =
else
LIBSUFFIX = _$(MEDIUM)
endif


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
INSTALL_HEADERDIR= include 
VPATH+= include

CPPFLAGS+=  -Iinclude -I$(NMF_TOOLS_DIR)/api/cpp \
            -I$(COMPONENT_TOP_DIR)/common

ifndef DISABLE_BACKWARD_COMPATIBILTY_MODE
CPPFLAGS+=-DBACKWARD_COMPATIBILTY_MODE
endif

VPATH+= ENS_Utils ENS_Component ENS_Fsm

LIBOBJ   = ENS_List  \
	   ENS_Component ENS_Port ENS_ResourcesDB ENS_Trace\
           ENS_Fsm ENS_Component_Fsm ENS_Port_Fsm NmfHost_ProcessingComponent 

ifeq ($(CORE_NUMBER),2)
# mpc relative code is added
LIBOBJ+= ENS_Nmf SharedBuffer NmfMpc_ProcessingComponent 
ifeq ($(MEDIUM),shared)
LIBOBJ+= ens_cpp-user2mpc-stubs ens_cpp-mpc2user-skels NmfHostMpc_ProcessingComponent
endif
CPPFLAGS+= -I$(COMPONENT_TOP_DIR)/nmf/mpc/$(BASE_REPOSITORY_MPC)
VPATH+= $(REPOSITORY_MPC)/host
endif

ifdef DEBUG
CFLAGS=-g -O0
LIBNAME  = st_ens_debug$(LIBSUFFIX)
CPPFLAGS += -DDEBUG -D__DEBUG
OBJ_EXT =odebug 
else
LIBNAME  = st_ens$(LIBSUFFIX)
CPPFLAGS += -DRELEASE -D__RELEASE
endif
INSTALL_LIB = lib$(LIBNAME).a

#WARNING todo target is MANDATORY (exe and/or lib)
todo: lib 

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

#-------------------------------
# local Rules
#-------------------------------







