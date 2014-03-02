# -*- Mode: Makefile -*-
# vim:syntax=make:
export PLATFORM
export SVA_SUBSYSTEM
export MODULE
MODULE = .

#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#IF libhardware.a is NOT rebuilt, set (or unset) NO_STD_BUILD and NO_NMF_BUILD 
#using -> source $MMROOT/video/makefiles/VideoMakefileSetup.csh fw/nmf/omx/all
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#all : std-lib
#clean : std-clean
#realclean : std-realclean
#install: std-install
#uninstall: std-uninstall
#
#std-install std-lib std-clean std-realclean std-uninstall header todo mylib:
#ifndef NO_STD_BUILD
#	$(MAKE) -f Make.std $(@:std-%=%)
#endif

include $(MM_MAKEFILES_DIR)/VideoAll.mk


