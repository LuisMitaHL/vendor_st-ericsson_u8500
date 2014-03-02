# -*- Mode: Makefile -*-
# vim:syntax=make:

## The MMDSP code is board specific
MMDSP_CODE_IS_BOARD_SPECIFIC:=true

include $(MMROOT)/shared/makefiles/Make.config
COMPONENT_TOP_DIR=..
ifeq ($(PLATFORM),ndk20-think)
execdir =arm11-think-rvct-ndk20
else ifeq ($(PLATFORM),mop500_ed-think)
execdir =cortexA9-think-rvct-mop500_ed
else ifeq ($(PLATFORM),mop500_v1-think)
execdir =cortexA9-think-rvct-mop500_v1
else ifeq ($(PLATFORM),x86-think)
execdir =x86
else
execdir =cortexA9-think-rvct-svp8500_ed
endif

export MODULE
export PLATFORM
export SVA_SUBSYSTEM
export MMROOT

RVD = source /sw/st/gnu_compil/comp/arm/rvds/3.1/linux/RVDS31env.posh && rvdebug

all clean realclean install:
ifeq ($(PLATFORM),x86-linux)
	make -f Make.proxy NO_HAMAC=1 $@
else
	make -f Make.proxy $@
endif


testit: 
ifeq ($(PLATFORM),x86-linux)
	make -C test NO_HAMAC=1 clean all
	test/bin/x86/TestH264Dec2_sw.exe
	cmp out_yuv.digest ref_yuv.digest
else
	make -C test clean all
	$(MMROOT)/video/components/h264dec/arm_nmf/test_appli/script/launcher.pl $(MMROOT)/video/components/h264dec/proxy_ddep/test/bin/$(execdir)/TestH264Dec2.axf
	cmp out_yuv.digest ref_yuv.digest
endif

include $(MMROOT)/video/makefiles/Make.doc

