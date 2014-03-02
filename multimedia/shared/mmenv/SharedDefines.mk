#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

#----------------------------------------------------------
# Verbosity management
#----------------------------------------------------------
export VERBOSE

VERCHAR=@
VERYVERCHAR=@

ifdef VERBOSE
  VERCHAR=
  MAKE_SILENT_OPTION =
else
  MAKE_OPTIONS += --no-print-directory
  MAKE_SILENT_OPTION = -s
endif

ifeq ($(VERBOSE),2)
  VERYVERCHAR=
endif

ifeq ($(VERBOSE),3)
  VERYVERCHAR=
  NMF_VERBOSE=true
  export NMF_VERBOSE
endif

ifeq ($(VERBOSE),4)
  VERYVERCHAR=
  NMF_VERBOSE=true
  export NMF_VERBOSE
  NMF_DEBUG=ALL
  export NMF_DEBUG
endif

#-----------------------------------
# Environment variables check
#-----------------------------------
ifndef PLATFORM
    $(error "Variable PLATFORM not set!")
endif

ifndef MMBUILDOUT
  $(error "Variable MMBUILDOUT not set!")
endif

## TARGET_OUT_INTERMEDIATES : directory in which we are expected to
## place all intermediates files created by build phase (.d, .o)
ifndef TARGET_OUT_INTERMEDIATES
  $(error "Variable TARGET_OUT_INTERMEDIATES not set!")
endif

ifndef PRODUCT_TOP_DIR
  $(error "Variable PRODUCT_TOP_DIR not set!")
endif

ifndef MMROOT
  $(error "Variable MMROOT not set!")
endif

#-----------------------------------
# MMPLATFORM elements
#-----------------------------------

## Get the variables from PLATFORM
MYMMBOARD  :=$(shell echo $(PLATFORM) | cut -d - -f1)
MYMMSYSTEM :=$(shell echo $(PLATFORM) | cut -d - -f2)

## PWD computing
UNAME=$(shell uname)
ifeq ($(UNAME),Linux)
  PWD:=$(shell pwd)
else
  UNAME=Win32
  PWD:=$(shell cmd /C echo %cd:\=/%)
endif #linux

ifneq ($(findstring $(MMROOT),$(PWD)), $(MMROOT))
  $(warning "You are not in a subpath of MMROOT")
endif

#-----------------------------------
# MEDIUM and MEDIUM_DIR computing
#-----------------------------------
MYMMROOT   := $(subst /,\/,$(shell echo $(MMROOT) | sed 's/\/$$//'))
MEDIUM    := $(shell echo $(CURDIR) | sed 's/$(MYMMROOT)//ig' | cut -d \/ -f2 )
MEDIUM_DIR:= $(MMROOT)/$(MEDIUM)

################################################################################

## Use 8 jobs when building NMF projects
NMF_JOBS=8
export NMF_JOBS

## Component top directory
ifndef COMPONENT_TOP_DIR
  COMPONENT_TOP_DIR := $(PWD)
  export COMPONENT_TOP_DIR
  ifdef VERBOSE
    $(info )
    $(info ==> COMPONENT_TOP_DIR=$(COMPONENT_TOP_DIR))
    $(info ==>   PRODUCT_TOP_DIR=$(PRODUCT_TOP_DIR))
    $(info )
  endif
endif

## Directory in which we generate usefull result of build (.a, .so, .exe)
ifndef MODULE
  MODULE = .
endif

## Get path from PRODUCT_TOP_DIR to have same dirs layout in the dest dir
REL_BUILD_INTERMEDIATES_DIR:=$(subst $(PRODUCT_TOP_DIR),,$(PWD))

## BUILD_TMP_DIR is always under out tree and never is source tree
BUILD_TMP_DIR:=$(TARGET_OUT_INTERMEDIATES)/$(REL_BUILD_INTERMEDIATES_DIR)/$(MODULE)

## BUILD_INTERMEDIATES_DIR is used by the rules to choose under which directory
## the result of the build is to be produced (out or source tree)
ifeq ($(MM_BUILD_FOR_PACKAGING),true)
  ## When building for packaging default to source tree
  BUILD_INTERMEDIATES_DIR:=$(MODULE)
else
  ## When NOT building for packaging default to out tree
  BUILD_INTERMEDIATES_DIR:=$(BUILD_TMP_DIR)
endif

ifeq ($(VERBOSE),2)
  $(foreach f,$(wordlist 2,$(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)),$(info ==> Include $f))
  $(info ==> Execute $(PWD)/$(firstword $(MAKEFILE_LIST)))
  $(info === MAKECMDGOALS=$(MAKECMDGOALS))
endif

################################################################################
## Variables used by mminstall read from environment
export PLATFORM
export MMBUILDOUT
export MAKEFILES_BACKWARD_COMPATIBLE

# Default to mminstall creating links
MMINSTALL_USES_INSTALL?=no
export MMINSTALL_USES_INSTALL

################################################################################

## Variables coding for important fixed names
NMF_REPO_DIR           :=nmf_repository
NMF_COMP_SRC_MMDSP_DIR :=nmf_comp_src_mmdsp
NMF_COMP_SRC_HOSTEE_DIR:=nmf_comp_src_hostee
NMF_COMP_SRC_COMMON_DIR:=nmf_comp_src_common
NMF_TMP_DIR            :=_nmf_tmpdir
OST_DICO_DIR           :=dictionaries
MAKEFILES_DIR_NAME     :=mkfiles
HEADER_DIR_NAME        :=include
LIB_DIR_NAME           :=lib
BIN_DIR_NAME           :=bin
DEP_DIR_NAME           :=dep
OBJ_DIR_NAME           :=obj
ROOTFS_DIR_NAME        :=rootfs
TOOLS_DIR_NAME         :=tools
NMFTOOLS_DIR_NAME      :=nmftools
TRACECOMPILER_DIR_NAME :=osttc

################################################################################

ifeq ($(KERNELRELEASE),)

# -include is because files are not yet installed in this directory when used by the mmenv Makefile
# Get MMDSPCORE variable - flags and tools only set if $(CORE)==mmdsp
-include $(MM_MAKEFILES_DIR)/SharedMMDSPFlags.mk

# Get the tools and flags for the defined PLATFORM
ifneq ($(CORE),mmdsp)
  -include $(MM_MAKEFILES_DIR)/SharedARMFlags.mk
endif

endif #ifeq ($(KERNELRELEASE),)

### This is for Audio makefiles to build NMF only for boards target
## could be cleaned out in audio makefiles
ifeq ($(findstring x86,$(MYMMBOARD)),x86)
  ## CORE_NUMBER is set to 1 to tell we only have a HOST core (no DSP)
  CORE_NUMBER=1
endif
ifeq ($(findstring 5500,$(MYMMBOARD)),5500)
  ## CORE_NUMBER is set to 1 to tell we only have a HOST core (no DSP)
  CORE_NUMBER=1
endif
ifeq ($(findstring 8500,$(MYMMBOARD)),8500)
  ## CORE_NUMBER is set to 2 to tell we have BOTH host core and DSPs
  CORE_NUMBER=2
  CORE_FLAG= CORE=mmdsp
endif
ifeq ($(findstring 9540,$(MYMMBOARD)),9540)
  ## CORE_NUMBER is set to 2 to tell we have BOTH host core and DSPs
  CORE_NUMBER=2
  CORE_FLAG= CORE=mmdsp
endif
ifeq ($(findstring 8540,$(MYMMBOARD)),8540)
  ## CORE_NUMBER is set to 2 to tell we have BOTH host core and DSPs
  CORE_NUMBER=2
  CORE_FLAG= CORE=mmdsp
endif
ifeq ($(CORE_NUMBER),)
  $(error CORE_NUMBER not defined for PLATFORM='$(PLATFORM)')
endif

## Default compiler to use
MMCOMPILER=rvct

ifeq ($(MYMMSYSTEM),linux)
  MMCOMPILER=gnu
endif

ifeq ($(MYMMSYSTEM),android)
  MMCOMPILER=android
endif

ifeq ($(findstring x86,$(MYMMBOARD)),x86)
  MMCOMPILER=gnu
  MMPROCESSOR=$(MYMMBOARD)
endif

ifeq ($(findstring 5500,$(MYMMBOARD)),5500)
  MMPROCESSOR=cortexA9
endif

ifeq ($(findstring 8500,$(MYMMBOARD)),8500)
  MMPROCESSOR=cortexA9
endif

ifeq ($(findstring 9540,$(MYMMBOARD)),9540)
  MMPROCESSOR=cortexA9
endif

ifeq ($(findstring 8540,$(MYMMBOARD)),8540)
  MMPROCESSOR=cortexA9
endif

ifeq ($(CORE),mmdsp)
  MMPROCESSOR=$(MMDSPPROCESSOR)
  ifneq ($(findstring x86,$(MYMMBOARD)),x86)
    MMCOMPILER=flexcc
  endif
endif

## MYMMDSPCORE is board specific as NMF repository on the target partition must be unique
MYMMDSPCORE:=$(MMDSPCORE)

ifeq ($(MMDSPCORE),)
  $(error MMDSPCORE not defined for PLATFORM='$(PLATFORM)')
else
  ifndef MMDSP_CODE_IS_BOARD_SPECIFIC
    MMDSPCORE:=mmdsp_x
  endif
endif

## MMDSPCORE and MMDSPPROCESSOR are used to in NMF generated code directory naming
MMDSPPROCESSOR:=$(MMDSPCORE)

ifndef MMSYSTEM
  MMSYSTEM=$(MYMMSYSTEM)
endif

ifndef MMBOARD
  MMBOARD=$(MYMMBOARD)
endif

## Following variables are here for historical reasons
## NMF_ENV used to detect a linux/android system
NMF_CONF=$(NMF_CONFIG)
NMF_ENV:=linux

## Default to use of GCC
## Makefile including this file will set to true to enable use of RVCT
USE_OPT_COMPILER?=false

## compiler used by linux platforms to link optimized rvct libraries with gnu-compiled proxies
MMOPTCOMPILER:=rvct4gnu

ifeq ($(CORE),mmdsp)
  ifeq ($(MMBOARD),x)
    MMPLATFORM   := mmdsp-x
  else
    MMPLATFORM   := $(MMDSPPROCESSOR)
  endif
  MMSEARCHPLAT := mmdsp*
  COMPATPLAT   := $(MMDSPPROCESSOR) mmdsp-x
  MMOPTCOMPILER:=none
else
  ifeq ($(findstring x86,$(MYMMBOARD)),x86)
    MMPLATFORM   := $(MYMMBOARD)
    MMSEARCHPLAT := $(MYMMBOARD)
    COMPATPLAT   :=
    MMOPTCOMPILER:=none
  else ifeq ($(MYMMSYSTEM),symbian)
    MMPLATFORM   :=generated
  else
    ifeq ($(USE_OPT_COMPILER),true)
      MMCOMPILER=$(MMOPTCOMPILER)
    endif

    # The long format name for the current component item
    MMPLATFORM   := $(MMPROCESSOR)-$(MMSYSTEM)-$(MMCOMPILER)-$(MMBOARD)

    # The long format name of compatible items for the current component item
    COMPATPLAT   := $(MMPROCESSOR)-$(MYMMSYSTEM)-$(MMOPTCOMPILER)-$(MYMMBOARD) \
                    $(MMPROCESSOR)-x-$(MMOPTCOMPILER)-x \
                    $(MMPROCESSOR)-$(MYMMSYSTEM)-$(MMOPTCOMPILER)-x \
                    $(MMPROCESSOR)-x-$(MMOPTCOMPILER)-$(MYMMBOARD) \
                    $(MMPROCESSOR)-$(MYMMSYSTEM)-$(MMCOMPILER)-$(MYMMBOARD) \
                    $(MMPROCESSOR)-x-$(MMCOMPILER)-x \
                    $(MMPROCESSOR)-$(MYMMSYSTEM)-$(MMCOMPILER)-x \
                    $(MMPROCESSOR)-x-$(MMCOMPILER)-$(MYMMBOARD)

    MMSEARCHPLAT := $(MMPROCESSOR)-*-$(MMCOMPILER)-$(MYMMBOARD) \
                    $(MMPROCESSOR)-*-$(MMCOMPILER)-x \
                    $(MMPROCESSOR)-*-$(MMOPTCOMPILER)-$(MYMMBOARD) \
                    $(MMPROCESSOR)-*-$(MMOPTCOMPILER)-x
  endif
endif

################################################################################
## multimedia content install directories
#  (where the build/install of a component will install things)
INSTALL_MAKEFILES_DIR  = $(MMBUILDOUT)/$(MAKEFILES_DIR_NAME)

INSTALL_TOOLS_DIR      = $(MMBUILDOUT)/$(TOOLS_DIR_NAME)

INSTALL_LIB_DIRNAME    = $(MMBUILDOUT)/$(LIB_DIR_NAME)
INSTALL_BIN_DIRNAME    = $(MMBUILDOUT)/$(BIN_DIR_NAME)

INSTALL_LIB_DIR        = $(INSTALL_LIB_DIRNAME)/$(MMPLATFORM)
INSTALL_SOLIB_DIR      = $(INSTALL_LIB_DIRNAME)/$(MMPLATFORM)
INSTALL_BIN_DIR        = $(INSTALL_BIN_DIRNAME)/$(MMPLATFORM)
INSTALL_NMF_REPO_DIR   = $(MMBUILDOUT)/$(NMF_REPO_DIR)

INSTALL_ROOTFS_DIR     = $(MMBUILDOUT)/$(ROOTFS_DIR_NAME)/$(MMPLATFORM)

INSTALL_HEADER_BASEDIR  = $(MMBUILDOUT)/$(HEADER_DIR_NAME)/$(PLATFORM)
INSTALL_HEADER_DIR      = $(INSTALL_HEADER_BASEDIR)/$(MEDIUM)
INSTALL_DICO_DIR        = $(MMBUILDOUT)/$(OST_DICO_DIR)/$(PLATFORM)

## Allowed to define a prefix when installing so libs (sub directory name in which to install)
INSTALL_SOLIB_PREFIX_DIR:=

## Allow to define a prefix when installing headers (sub directory name in which to install)
INSTALL_HEADER_PREFIX_DIR:=

################################################################################
## Definition of directories in which we find installed content from our dependencies

## In case MMBUILDIN is not set, we default to MMBUILDOUT
MMBUILDIN ?=$(MMBUILDOUT)

MM_MAKEFILES_DIR ?= $(MMBUILDIN)/$(MAKEFILES_DIR_NAME)

MM_TOOLS_DIR      = $(MMBUILDIN)/$(TOOLS_DIR_NAME)

MM_LIB_DIRNAME    = $(MMBUILDIN)/$(LIB_DIR_NAME)
MM_BIN_DIRNAME    = $(MMBUILDIN)/$(BIN_DIR_NAME)

MM_LIB_DIR        = $(MM_LIB_DIRNAME)/$(MMPLATFORM)
MM_SOLIB_DIR      = $(MM_LIB_DIRNAME)/$(MMPLATFORM)
MM_BIN_DIR        = $(MMBUILDIN)/$(BIN_DIR_NAME)/$(MMPLATFORM)
MM_NMF_REPO_DIR   = $(MMBUILDIN)/$(NMF_REPO_DIR)

MM_ROOTFS_DIR     = $(MMBUILDIN)/$(ROOTFS_DIR_NAME)/$(MMPLATFORM)

MM_HEADER_BASEDIR = $(MMBUILDIN)/$(HEADER_DIR_NAME)/$(PLATFORM)
MM_HEADER_DIR     = $(MM_HEADER_BASEDIR)/$(MEDIUM)
MM_DICO_DIR       = $(MMBUILDIN)/$(OST_DICO_DIR)/$(PLATFORM)

################################################################################

MMINSTALL  =$(MM_MAKEFILES_DIR)/mminstall -c $(MMINSTALL_ARGS)
GEN_LN     =$(MMINSTALL)
CP         =cp -f
RM         =rm -vrf
LN         =ln -fs
MKDIR      =mkdir -p
RM_LINK    =rm -vf
RM_FILE    =rm -vf
RM_DIRS    =$(RM)

################################################################################

INSTALL_NMF_COMP_SRC_MMDSP_DIR  = $(MMBUILDOUT)/$(NMF_COMP_SRC_MMDSP_DIR)/$(PLATFORM)
MM_NMF_COMP_SRC_MMDSP_DIR       = $(MMBUILDIN)/$(NMF_COMP_SRC_MMDSP_DIR)/$(PLATFORM)

INSTALL_NMF_COMP_SRC_HOSTEE_DIR = $(MMBUILDOUT)/$(NMF_COMP_SRC_HOSTEE_DIR)/$(PLATFORM)
MM_NMF_COMP_SRC_HOSTEE_DIR      = $(MMBUILDIN)/$(NMF_COMP_SRC_HOSTEE_DIR)/$(PLATFORM)

INSTALL_NMF_COMP_SRC_COMMON_DIR = $(MMBUILDOUT)/$(NMF_COMP_SRC_COMMON_DIR)/$(PLATFORM)
MM_NMF_COMP_SRC_COMMON_DIR      = $(MMBUILDIN)/$(NMF_COMP_SRC_COMMON_DIR)/$(PLATFORM)

## Directories in which we install the IDT files (keep them board specific)
INSTALL_IDT_DIR                 = $(MMBUILDOUT)/$(NMF_COMP_SRC_COMMON_DIR)/$(MYMMDSPCORE)
MM_IDT_DIR                      = $(MMBUILDIN)/$(NMF_COMP_SRC_COMMON_DIR)/$(MYMMDSPCORE)

## Directories in which we install the NMF repositories (keep them board specific)
INSTALL_NMF_REPO_BOARD_DIR      = $(INSTALL_NMF_REPO_DIR)/$(MYMMDSPCORE)
MM_NMF_REPO_BOARD_DIR           = $(MM_NMF_REPO_DIR)/$(MYMMDSPCORE)

################################################################################
## Directories in which component we are currently building is installing files

INSTALL_SHARED_NMF_REPO_DIR  = $(INSTALL_NMF_REPO_DIR)
INSTALL_IMAGING_NMF_REPO_DIR = $(INSTALL_NMF_REPO_DIR)
INSTALL_VIDEO_NMF_REPO_DIR   = $(INSTALL_NMF_REPO_DIR)
INSTALL_AUDIO_NMF_REPO_DIR   = $(INSTALL_NMF_REPO_DIR)

################################################################################
## Directories in which component we depend upon have installed their files

MM_SHARED_NMF_REPO_DIR  = $(MM_NMF_REPO_DIR)
MM_IMAGING_NMF_REPO_DIR = $(MM_NMF_REPO_DIR)
MM_VIDEO_NMF_REPO_DIR   = $(MM_NMF_REPO_DIR)
MM_AUDIO_NMF_REPO_DIR   = $(MM_NMF_REPO_DIR)

ifeq ($(MAKEFILES_BACKWARD_COMPATIBLE),yes)

  SHARED := $(MMROOT)/shared
  LINUX  := $(MMROOT)/linux

  MMSHARED := $(SHARED)
  MMLINUX  := $(LINUX)

  NMF := $(MMSHARED)/nmf

  NMF_TOOLS_DIR := $(NMF)/tools
  NMF_LINUX_API_DIR := $(NMF)/linux/api

  ## Only needed when building executable to run on DSP
  ## $(NMF)/mpcee/src can be removed when nmf.mpc installs source where expected
  NMF_MPCEE_SRC := $(NMF)/mpcee/src $(MM_NMF_COMP_SRC_MMDSP_DIR)/mpcee

  TRACECOMPILERDIR = $(MMSHARED)/osttools/osttc

else # ifeq ($(MAKEFILES_BACKWARD_COMPATIBLE),yes)

  NMF_TOOLS_DIR := $(MM_TOOLS_DIR)/$(NMFTOOLS_DIR_NAME)

  TRACECOMPILERDIR = $(MM_TOOLS_DIR)/$(TRACECOMPILER_DIR_NAME)

  NMF_MPCEE_SRC := $(MM_NMF_COMP_SRC_MMDSP_DIR)/mpcee

endif # ifeq ($(MAKEFILES_BACKWARD_COMPATIBLE),yes)

ifdef DUMP_VARIABLES
  $(info )
  $(info -------------------------------------- SharedDefines.mk --------------------------------------)
  $(info MAKECMDGOALS=$(MAKECMDGOALS))
  $(info )
  $(info MM_BUILD_FOR_PACKAGING=$(MM_BUILD_FOR_PACKAGING))
  $(info MMCOMPILER=$(MMCOMPILER)) 
  $(info PRODUCT_TOP_DIR=$(PRODUCT_TOP_DIR))
  $(info TARGET_OUT_INTERMEDIATES=$(TARGET_OUT_INTERMEDIATES))
  $(info BUILD_INTERMEDIATES_DIR=$(BUILD_INTERMEDIATES_DIR))
  $(info REL_BUILD_INTERMEDIATES_DIR=$(REL_BUILD_INTERMEDIATES_DIR))
  $(info )
  $(info INSTALL_LIB_DIR=$(INSTALL_LIB_DIR))
  $(info -------------------------------------- SharedDefines.mk --------------------------------------)
  $(info )
endif
