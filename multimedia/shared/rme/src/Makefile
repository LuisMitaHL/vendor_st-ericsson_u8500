#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

MMSYSTEM=x
#MMBOARD=x

# Since the generic Make.config add the NMF and medium includes to CPPFLAGS
# and we don't want those, make sure we include omxil headers first as 
# we want to be able to be able to compile rme while having the ens installed

CPPFLAGS+= -I../include -I$(MM_HEADER_DIR)/omxil -I$(MM_HEADER_DIR)

include $(MM_MAKEFILES_DIR)/SharedConfig.mk

INSTALL_HEADERDIR= ../include 
VPATH+= ../include

LIBOBJ   = RMEimpl ORPComp ORPCompHalfline ORPCompNetwork \
           ResManager ResLevels ResEstimator ResAlp \
           PfRoles PfDomManager \
           NmfDomains \
           AGov

#CPPFLAGS += -DNODEFAULTDOMAIN
CPPFLAGS += -DNOCREATEDOMAINFORCLIENT

#TEMPORARY definition of SW_VARIANT_ANDROID => shall come from upper layers
ifeq ($(MMSYSTEM),android)
CPPFLAGS += -DSW_VARIANT_ANDROID
endif
ifeq ($(SW_VARIANT),android)
CPPFLAGS += -DSW_VARIANT_ANDROID
endif

ifdef DEBUG
LIBNAME  = st_rme_debug
INSTALL_LIB = libst_rme_debug.a
CPPFLAGS += -DDEBUG -D__DEBUG -g
OBJ_EXT =odebug 
else
LIBNAME  = st_rme
INSTALL_LIB = libst_rme.a
endif

todo: lib 

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk








