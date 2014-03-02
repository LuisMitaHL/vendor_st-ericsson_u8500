# vim:syntax=make:
# -*- Mode: Makefile -*-

#WARNING COMPONENT_PATH is MANDATORY for Make.config and Make.rules
#-------------------------------
# Default Config
#-------------------------------

MMBOARD=x

include $(MM_MAKEFILES_DIR)/SharedConfig.mk

CPPFLAGS +=-I ../nmf/mpc/$(BASE_REPOSITORY)

VPATH    =../nmf/mpc/$(BASE_REPOSITORY)/host
LIBNAME  = st_sia_itfs
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

INSTALL_LIB    = libst_sia_itfs.a
INSTALL_HEADER    = register.h

export EXPORT_FILEDIR = nmf/mpc/nmf_repository/mmdsp_8500/host

EXTRA_INSTALL += install_resources
EXTRA_UNINSTALL += uninstall_resources

uninstall_resources:
	@echo "Removing  $(INSTALL_NMF_REPO_BOARD_DIR)/preload_imaging.txt"
	$(VERCHAR)$(RM) $(INSTALL_NMF_REPO_BOARD_DIR)/preload_imaging.txt

install_resources: $(INSTALL_NMF_REPO_BOARD_DIR)/preload_imaging.txt

$(INSTALL_NMF_REPO_BOARD_DIR)/preload_imaging.txt :  preload_imaging.txt
	@echo "  MMINSTALL preload_imaging.txt"
	$(VERCHAR)$(MMINSTALL) -i -f $< $@

#WARNING todo target is MANDATORY (exe and/or lib)
todo: lib install_resources

$(warning "BASE_REPOSITORY : ${BASE_REPOSITORY}")
#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

SYMBIAN_INCLUDE+= ../nmf/mpc/nmf_repository/mmdsp_8500/
ifeq ($(findstring symbian,$(PLATFORM)),symbian)
 NO_VARIANT=true
 include ${MMROOT}/imaging/symbian_makerules/mmh.mk
endif

