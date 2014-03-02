# -*- Mode: Makefile -*-
# vim:syntax=make:

export MODULE
export PLATFORM
MODULE:=.
all clean realclean install:
	make -f Make.proxy $@
	make -f Make.stubskel $@
	
ifdef BUILD_TEST
	make -C test_arm $@
endif

check_constraint:
	@ $(MMROOT)/video/videotools/check_constraint.sh mmgrep src
	@ $(MMROOT)/video/videotools/check_constraint.sh mmgrep inc

#include $(MMROOT)/video/makefiles/Make.doc

