# vim:syntax=make:
# -*- Mode: Makefile -*-

#WARNING COMPONENT_PATH is MANDATORY for Make.config and Make.rules
#-------------------------------
# Default Config
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedConfig.mk

#----------------------------
ifdef DEBUG
CPPFLAGS += -DOSI_DEBUG -D__DEBUG
LIBNAME		= st_variationif_debug
OBJEXT		= odebug
INSTALL_LIB	= libst_variationif_debug.a
else
LIBNAME		= st_variationif
INSTALL_LIB	= libst_variationif.a
endif

#VPATH 		+= src
LIBOBJ		= variationif_ste

CPPFLAGS  += -I$(MM_NMF_REPO_BOARD_DIR)

CPPFLAGS  += -I$(MM_HEADER_DIR)


todo: lib install 

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

ifeq ($(MMSYSTEM),symbian)
NO_VARIANT=true
include ${MMROOT}/imaging/symbian_makerules/mmh.mk
endif
