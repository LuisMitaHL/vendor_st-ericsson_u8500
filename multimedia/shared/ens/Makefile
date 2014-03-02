# -*- Mode: Makefile -*-
# vim:syntax=make:

include $(MM_MAKEFILES_DIR)/SharedCheck.mk

all clean realclean install uninstall: proxy/include/ENS_ThisVersion.h
	$(MAKE) -C common $@
	$(MAKE) -C nmf/host $@
	$(MAKE) -C proxy $@
	$(MAKE) -C inc $@

test:
	- $(MAKE) -C proxy/tests $@
	- testdbcreate --tag $(PLATFORM) --testlist proxy/tests/testlist.out -o test_report_proxy_$(PLATFORM)
	- testdbcreate --tag $(PLATFORM) --testlist proxy/tests/testlist.out -o test_report_proxy_$(PLATFORM) --html
	testdbmerge -o test_report_$(PLATFORM) -i test_report_proxy_$(PLATFORM) --html; \
	testdbmerge -o test_report_$(PLATFORM) -i test_report_proxy_$(PLATFORM);

cleantest:
ifeq ($(CORE_NUMBER),2)
	- $(MAKE) -C nmf/mpc/fsm/tests $@
endif
	- $(MAKE) -C proxy/tests $@
	rm -rf test_report*$(PLATFORM)*

eabidef:
	@echo "Generating Symbian DEF file"
	export PERCEP=`pwd`/perceps
	rm -rf eabi-tmp
	mkdir eabi-tmp
	@if [ ! -f proxy/lib/cortexA9-x-rvct-u8500_v2/libst_ens.a ] ; then echo 'Library libst_ens.a for u8500_v2 platform not built'; exit 1; fi;
	@if [ ! -f nmf/host/lib/cortexA9-x-rvct-x/libst_ensfsm_host.a ] ; then echo 'Library libst_ensfsm_host.a for u8500_v2 platform not built'; exit 1; fi;
	nm proxy/lib/cortexA9-x-rvct-u8500_v2/libst_ens.a > eabi-tmp/lib.map
	nm nmf/host/lib/cortexA9-x-rvct-x/libst_ensfsm_host.a >> eabi-tmp/lib.map
	./update_dlldef.pl --lib ./eabi-tmp/lib.map --def ./eabi/st_ensu.def --wkd ./eabi-tmp  ./proxy/include ./proxy/ENS_Component \
		./proxy/ENS_Fsm/ ./proxy/ENS_Utils/ ./nmf/host/fsm/component ./nmf/host/fsm/controller ./nmf/host/fsm/generic

proxy/include/ENS_ThisVersion.h:
ifeq ($(MEDIUM),shared)
	@echo "#define THIS_ENS_VERSION ENS_VERSION_SHARED" > $@
else ifeq ($(MEDIUM),video)
	@echo "#define THIS_ENS_VERSION ENS_VERSION_VIDEO" > $@
else ifeq ($(MEDIUM),audio)
	@echo "#define THIS_ENS_VERSION ENS_VERSION_AUDIO" > $@
else ifeq ($(MEDIUM),imaging)
	@echo "#define THIS_ENS_VERSION ENS_VERSION_IMAGING" > $@
else 
	$(error MEDIUM wrongly defined as $(MEDIUM))
endif
