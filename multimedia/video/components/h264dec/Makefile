# -*- Mode: Makefile -*-
# vim:syntax=make:

export MODULE
export PLATFORM
export SVA_SUBSYSTEM
MODULE = .
COMPONENT_TOP_DIR=.


all :
ifneq ($(PLATFORM),x86-linux)
ifneq ($(PLATFORM),svp5500_v1-linux)
	$(MAKE) -C mpc/algo
endif
endif
ifndef NO_NMF_BUILD
ifneq ($(PLATFORM),x86-linux)
ifneq ($(PLATFORM),svp5500_v1-linux)
	$(MAKE) -C mpc/algo
	$(MAKE) -C mpc/ddep
	$(MAKE) -C mpc/adapter
	$(MAKE) -C mpc/cabac
endif
endif
ifndef NO_OMX_BUILD
	$(MAKE) -C arm_nmf all
	$(MAKE) -C proxy_ddep
endif
endif

install :
ifneq ($(PLATFORM),x86-linux)
ifneq ($(PLATFORM),svp5500_v1-linux)
	$(MAKE) -C mpc/algo $@
	$(MAKE) -C mpc/ddep $@
	$(MAKE) -C mpc/adapter $@
endif
endif
	$(MAKE) -C arm_nmf $@

ifndef NO_OMX_BUILD
	$(MAKE) -C proxy_ddep $@
endif

install_test:
ifeq ($(PLATFORM),href_v1-linux)
	$(MAKE) -C test/launcher_video install_test
endif

clean realclean:
	$(MAKE) -C mpc/algo $@
	$(MAKE) -C mpc/ddep $@
	$(MAKE) -C mpc/adapter $@
	$(MAKE) -C mpc/cabac $@
	$(MAKE) -C arm_nmf $@
	$(MAKE) -C proxy_ddep $@

disp:
	@echo $(PLATFORM)
	@echo $(SVA_SUBSYSTEM)

test:
	rm -rf $(MMROOT)/mmbuildout/rootfs/cortexA9-x-gnu-x/usr/video
	make -C $(MMROOT)/video/launcher_video install
	touch $(MMROOT)/shared/mmte/src/ate_cmd.c	
	$(MAKE) -C test/launcher_video omx_test_all_hudson


omx_test:
	$(MAKE) -C test/launcher_video omx_test_all

.PHONY : version.h test
