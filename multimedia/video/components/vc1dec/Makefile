# -*- Mode: Makefile -*-
# vim:syntax=make:
## The MMDSP code is board specific
MMDSP_CODE_IS_BOARD_SPECIFIC:=true

export MODULE
export PLATFORM
export SVA_SUBSYSTEM
MODULE = .

all : 
ifneq ($(PLATFORM),x86-linux)
	$(MAKE) -C mpc/algo
endif
ifndef NO_NMF_BUILD
ifneq ($(PLATFORM),x86-linux)
	$(MAKE) -C mpc/ddep
	$(MAKE) -C mpc/adapter
endif
ifndef NO_OMX_BUILD
	$(MAKE) -C arm_nmf all
	$(MAKE) -C proxy_ddep
endif
ifdef NO_OMX_BUILD
	$(MAKE) -C arm_nmf test_appli
endif
endif

install :
ifneq ($(PLATFORM),x86-linux)
	$(MAKE) -C mpc/algo $@
	$(MAKE) -C mpc/ddep $@
	$(MAKE) -C mpc/adapter $@
endif
	$(MAKE) -C arm_nmf $@
ifndef NO_OMX_BUILD
	$(MAKE) -C proxy_ddep $@
endif

clean realclean:
	$(MAKE) -C mpc/algo $@
	$(MAKE) -C mpc/ddep $@
	$(MAKE) -C mpc/adapter $@
	$(MAKE) -C arm_nmf $@
	$(MAKE) -C proxy_ddep $@
	$(MAKE) -C arm_nmf test_appli-clean


#### Checking constraint from Symbian ##############
check_constraint:
	$(MAKE) -C arm_nmf $@
	#$(MAKE) -C proxy $@
	
test:
	$(MAKE) -C test/launcher_video omx_test_all

omx_test:
	$(MAKE) -C test/launcher_video omx_test_all

.PHONY : version.h test
