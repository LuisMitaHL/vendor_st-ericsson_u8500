# -*- Mode: Makefile -*-
# vim:syntax=make:
## The MMDSP code is board specific
MMDSP_CODE_IS_BOARD_SPECIFIC:=true

export MODULE
export PLATFORM
export SVA_SUBSYSTEM
MODULE = .

COMPONENT_TOP_DIR=.
include $(MMROOT)/video/makefiles/Make.config

all :
# BIG HACK
ifneq ($(findstring 5500,$(PLATFORM)),5500)
	mkdir -p mpc/algo/src/ndk8500_a0/ mpc/algo_hdtv/src/ndk8500_a0/
	cp -f ../hardware/src/ndk8500_a0/mtf_api.c mpc/algo/src/ndk8500_a0
	cp -f ../hardware/src/ndk8500_a0/dma_api.c mpc/algo/src/ndk8500_a0
	cp -f ../hardware/src/ndk8500_a0/bpu_api.c mpc/algo/src/ndk8500_a0
	cp -f ../common/src/t1xhv_audiolib_buffer_reset.c mpc/algo/src
	cp -f ../hardware/src/ndk8500_a0/mtf_api.c mpc/algo_hdtv/src/ndk8500_a0
	cp -f ../hardware/src/ndk8500_a0/dma_api.c mpc/algo_hdtv/src/ndk8500_a0
	cp -f ../hardware/src/ndk8500_a0/bpu_api.c mpc/algo_hdtv/src/ndk8500_a0
	cp -f ../common/src/t1xhv_audiolib_buffer_reset.c mpc/algo_hdtv/src
endif
ifeq ($(SVA_SUBSYSTEM),ndk8500_a0)
	$(MAKE) -C mpc/algo FLAVOR="std"
	$(MAKE) -C mpc/algo FLAVOR="hdtv"
	$(MAKE) -C mpc/algo_hdtv
endif
ifndef NO_NMF_BUILD
ifneq ($(findstring 5500,$(PLATFORM)),5500)
	$(MAKE) -C mpc/ddep
	$(MAKE) -C mpc/adapter
endif
#	$(MAKE) -C arm_nmf lib-arm realclean # Hack for building test_appli !!!
	$(MAKE) -C arm_nmf lib-arm
ifneq ($(findstring android,$(PLATFORM)),android)
#	$(MAKE) -C arm_nmf/test_appli
endif
ifndef NO_OMX_BUILD
ifeq ($(findstring symbian,$(PLATFORM)),symbian)
	$(MAKE) -C arm_nmf lib-armstub
	$(MAKE) -C proxy_ddep NO_HAMAC=1
endif
	$(MAKE) -C arm_nmf ddep
	$(MAKE) -C proxy_ddep
endif
endif

install :
	$(MAKE) -C mpc/algo $@
ifeq ($(SVA_SUBSYSTEM),ndk8500_a0)
	$(MAKE) -C mpc/algo_hdtv $@
endif
	$(MAKE) -C mpc/ddep $@
	$(MAKE) -C mpc/adapter $@
	$(MAKE) -C arm_nmf $@
ifndef NO_OMX_BUILD
	$(MAKE) -C proxy_ddep $@
endif

clean:
	rm -f obj/*/*.o
ifneq ($(findstring 5500,$(PLATFORM)),5500)
	rm -f mpc/algo/src/ndk8500_a0/mtf_api.c
	rm -f mpc/algo/src/ndk8500_a0/dma_api.c
	rm -f mpc/algo/src/ndk8500_a0/bpu_api.c
	rm -f mpc/algo/src/t1xhv_audiolib_buffer_reset.c
	rm -f mpc/algo_hdtv/src/ndk8500_a0/mtf_api.c
	rm -f mpc/algo_hdtv/src/ndk8500_a0/dma_api.c
	rm -f mpc/algo_hdtv/src/ndk8500_a0/bpu_api.c
	rm -f mpc/algo_hdtv/src/t1xhv_audiolib_buffer_reset.c
endif
	$(MAKE) -C mpc/algo $@
ifeq ($(SVA_SUBSYSTEM),ndk8500_a0)
	$(MAKE) -C mpc/algo_hdtv $@
endif
	$(MAKE) -C mpc/ddep $@
	$(MAKE) -C mpc/adapter $@
	$(MAKE) -C arm_nmf $@
	$(MAKE) -C proxy_ddep $@


realclean:
	rm -rf inc #trace
	find . -name "*.gconf" | sed -e "s/gconf/conf/g" | xargs rm -rf # BIG HACK
	rm -f obj/*/*.o
ifneq ($(findstring 5500,$(PLATFORM)),5500)
	rm -f mpc/algo/src/ndk8500_a0/mtf_api.c
	rm -f mpc/algo/src/ndk8500_a0/dma_api.c
	rm -f mpc/algo/src/ndk8500_a0/bpu_api.c
	rm -f mpc/algo/src/t1xhv_audiolib_buffer_reset.c
	rm -f mpc/algo_hdtv/src/ndk8500_a0/mtf_api.c
	rm -f mpc/algo_hdtv/src/ndk8500_a0/dma_api.c
	rm -f mpc/algo_hdtv/src/ndk8500_a0/bpu_api.c
	rm -f mpc/algo_hdtv/src/t1xhv_audiolib_buffer_reset.c
endif
	$(MAKE) -C mpc/algo $@
ifeq ($(SVA_SUBSYSTEM),ndk8500_a0)
	$(MAKE) -C mpc/algo_hdtv $@
endif
	$(MAKE) -C mpc/ddep $@
	$(MAKE) -C mpc/adapter $@
	$(MAKE) -C arm_nmf $@
	$(MAKE) -C proxy_ddep $@

#### Checking constraint from Symbian ##############
check_constraint:
	$(MAKE) -C arm_nmf $@
	$(MAKE) -C proxy_ddep $@
cleantest:
	@echo cleantest
test:
	rm -rf $(MMROOT)/mmbuildout/rootfs/cortexA9-x-gnu-x/usr/video
	make -C $(MMROOT)/video/launcher_video install
	touch $(MMROOT)/shared/mmte/src/ate_cmd.c	
	$(MAKE) -C test/launcher_video omx_test_all_hudson

omx_test:
	$(MAKE) -C test/launcher_video omx_test_all

.PHONY : version.h test
