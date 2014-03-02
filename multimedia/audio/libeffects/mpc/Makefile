# -*- # -*- Mode: Makefile -*-
# vim:syntax=make:
include $(MM_MAKEFILES_DIR)/SharedNmfConfig.mk
include $(MM_MAKEFILES_DIR)/AudioFlags.mk
include $(COMPONENT_TOP_DIR)/Make.ost_conf

SRCPATH+=$(MM_NMF_COMP_SRC_MMDSP_DIR)/mpcee

SRCPATH+=$(COMPONENT_TOP_DIR)/..
SRCPATH+=$(COMPONENT_TOP_DIR)/mpc/include

CPPFLAGS+=-D_NMF_MPC_

OST_COMPONENT_ID = $(KUidDicStAudioLibeffectsMpc)
OST_DICO_NAME = $(OST_COMPONENT_NAME)_mpc
OST_SRC_DIR = libupdownmix libmalloc
OST_SRC_DIR += libvolctrlapi libresamplingapi libsplapi libteqapi libmdrc5bapi
INSTALL_DICO = $(OST_DICO_NAME).xml

CPPFLAGS+=-DNVERBOSE

COMPONENTS= libeffects.mpc.libeffects

USER2MPCITFS_CPP= libeffects.mpc.libmalloc.init

INSTALL_NMF_COMP_SRC_COMMON_PREFIX:=libeffects/
INSTALL_NMF_COMP_SRC_COMMON += mpc/libmalloc/libmalloc.idt \
				libtransducer_equalizer/include/config.idt \
				libmdrc5b/include/config.idt

ITF_PREFIX = libeffects
TOBECLEAN  += $(ITF_PREFIX)

include $(MM_MAKEFILES_DIR)/SharedNmfRules.mk

TESTLISTS = testlist.lst
DATABASE  = mpc
TEST_TAG  = $(MMDSPPROCESSOR)
include $(MM_MAKEFILES_DIR)/AudioTest.mk

