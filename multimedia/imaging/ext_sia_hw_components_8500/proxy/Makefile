# vim:syntax=make:
# -*- Mode: Makefile -*-

#WARNING COMPONENT_PATH is MANDATORY for Make.config and Make.rules
#-------------------------------
# Default Config
#-------------------------------

MMBOARD=x

include $(MMROOT)/shared/makefiles/Make.config

CPPFLAGS +=-I ../nmf/mpc/$(BASE_REPOSITORY)

VPATH    =../nmf/mpc/$(BASE_REPOSITORY)/host
LIBNAME  = st_ext_sia_itfs
LIBOBJ   = imaging-mpc2user-skels imaging-user2mpc-stubs imaging_cpp-mpc2user-skels imaging_cpp-user2mpc-stubs register

ifeq ($(MMSYSTEM),think)
ARMCCFLAGS=--cpu=ARM926EJ-S
endif
ifeq ($(MMSYSTEM),linux)
ARMCCFLAGS=-mcpu=arm926ej-s
endif
ifeq ($(MMSYSTEM),android)
ARMCCFLAGS=-mcpu=arm926ej-s
endif

CPPFLAGS += -D_nOS1
CFLAGS += -D_nOS1

INSTALL_LIB    = libst_ext_sia_itfs.a
INSTALL_HEADER    = register.h


export EXPORT_FILEDIR = nmf/mpc/nmf_repository/mmdsp_8500/host

# Generation of solibs
ifeq ($(findstring linux,$(PLATFORM)),linux)
ifneq ($(PLATFORM),x86-linux)
IS_OMX_COMPONENT_LIB=false
include $(MMROOT)/linux/bellagio_omxcore/Make.config
CPPFLAGS+=-I$(STE_BELLAGIO_SRC_ROOT)/ens -I../common
SOLIBNAMES+=ext_ste_sia_itfs
IMAGE_LIBS = $(LIBNAME)
IMAGING_NMF_HOST_COMPONENTS = resource_sharerWrapped ext_img_network
EXTRALIBS_$(SOLIBNAMES) := $(IMAGE_LIBS)
SOEXTRALIBS_$(SOLIBNAMES) += $(IMAGING_NMF_HOST_COMPONENTS_LIBS)
PLATFORM_LIST = cortexA9-linux-gnu-href_v1 cortexA9-linux-gnu-x cortexA9-x-rvct-x cortexA9-x-gnu-href_v1 cortexA9-x-rvct4gnu-x cortexA9-x-gnu-x
LIB_DIR_LIST =  $(foreach media, shared imaging linux, $(foreach mmpath, $(PLATFORM_LIST), $(addprefix -L,$(MMROOT)/$(media)/rootfs/$(mmpath)$(COMPONENT_PATH))))
SOLIBS_LIST = ste_stab
SOLIBS_FLAG = $(foreach tmp, $(SOLIBS_LIST),  $(addprefix -l,$(tmp)))
INSTALL_SOLIB = $(SOLIBNAMES)
FLAGS_$(SOLIBNAMES) = $(LIB_DIR_LIST) $(SOLIBS_FLAG)
endif
endif

#WARNING todo target is MANDATORY (exe and/or lib)
todo: lib 
#solib

$(warning "BASE_REPOSITORY : ${BASE_REPOSITORY}")
#-------------------------------
# Generic Rules
#-------------------------------
include $(MMROOT)/shared/makefiles/Make.rules

SYMBIAN_INCLUDE+= ../nmf/mpc/nmf_repository/mmdsp_8500/
ifeq ($(findstring symbian,$(PLATFORM)),symbian)
 include ${MMROOT}/imaging/symbian_makerules/mmh.mk
endif

