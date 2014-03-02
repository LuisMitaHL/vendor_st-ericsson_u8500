# -*- Mode: Makefile -*-
# vim:syntax=make:
## The MMDSP code is board specific
MMDSP_CODE_IS_BOARD_SPECIFIC:=true

COMPONENT_TOP_DIR=..
ifeq ($(PLATFORM),ndk20-think)
target = x86_8820
execdir =arm11-think-rvct-ndk20
else ifeq ($(PLATFORM),mop500_ed-think)
target = x86_8500ed
execdir =cortexA9-think-rvct-mop500_ed
else
target = x86_8500ed
execdir =cortexA9-think-rvct-svp8500_ed
endif

export MODULE
export PLATFORM
export SVA_SUBSYSTEM
export MMROOT

RVD = source /sw/st/gnu_compil/comp/arm/rvds/3.1/linux/RVDS31env.posh && rvdebug

all clean realclean :
	make -f Make.proxy $@
	#make -f Make.proxy NO_HAMAC=1 $@

install:
	make -f Make.proxy $@
	#make -f Make.proxy NO_HAMAC=1 $@

rvd :
	$(RVD) -aws=- -nologo  -inc='./test/script/rvd_$(shell whoami).inc' --stdiolog='log.log'
t32 :
	$(MMROOT)/video/components/h264enc/arm_nmf/test_appli/script/launcher.pl $(PLATFORM) $(MMROOT)/video/components/h264enc/proxy_ddep/test/bin/$(execdir)/TestH264Enc.axf

rvdsw :
	$(RVD) -aws=- -nologo  -inc='./test/script/rvdsw_$(shell whoami).inc'


check_constraint:
	@ $(MMROOT)/video/videotools/check_constraint.sh mmgrep src
	@ $(MMROOT)/video/videotools/check_constraint.sh mmgrep inc

include $(MMROOT)/video/makefiles/Make.doc

