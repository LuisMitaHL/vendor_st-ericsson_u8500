# -*- Mode: Makefile -*-
# vim:syntax=make:

export MODULE
export PLATFORM
export SVA_SUBSYSTEM
export MMROOT

## The MMDSP code is board specific
MMDSP_CODE_IS_BOARD_SPECIFIC:=true

all clean realclean :
ifeq ($(PLATFORM),x86-linux)
	make -f Make.proxy NO_HAMAC=1 $@	
else
	make -f Make.proxy $@
endif


install:
ifeq ($(PLATFORM),x86-linux)
	make -f Make.proxy NO_HAMAC=1 $@
else
	make -f Make.proxy $@
endif


t32 :
	$(MMROOT)/video/components/vc1dec/arm_nmf/test_appli/script/runlauterbach.dat $(MMROOT)/video/components/vc1dec/proxy_ddep/test/bin/arm11-think-rvct-ndk20/Testvc1Dec.axf

check_constraint:
	@ $(MMROOT)/video/videotools/check_constraint.sh mmgrep src
	@ $(MMROOT)/video/videotools/check_constraint.sh mmgrep inc

include $(MMROOT)/video/makefiles/Make.doc

