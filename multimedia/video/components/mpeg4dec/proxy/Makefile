# -*- Mode: Makefile -*-
# vim:syntax=make:

export MODULE
export PLATFORM
MODULE.=..
MODULE_NAME = mpeg4dec
all clean realclean install:
	make -f Make.proxy $@
	make -f Make.stubskel $@
	make -f Make.stubskel_sw $@
ifdef BUILD_TEST
	make -C test_arm $@
endif

check_constraint:
	@ $(MMROOT)/video/videotools/check_constraint.sh mmgrep src
	@ $(MMROOT)/video/videotools/check_constraint.sh mmgrep inc

#include $(MMROOT)/video/makefiles/Make.doc

