# vim:syntax=make:
# -*- Mode: Makefile -*-

#-------------------------------
# Default Config
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedCheck.mk

all clean realclean install uninstall:
	$(MAKE) -C nmf/common $@
ifeq ($(CORE_NUMBER),2)
	$(MAKE) -C nmf/mpc $@;
endif
	$(MAKE) -C include $@
	$(MAKE) -C inc $@

test:
ifeq ($(CORE_NUMBER),2)
	$(MAKE) -C nmf/mpc/fsm/tests $@; \
	 testdbcreate --tag $(MMDSPPROCESSOR) --testlist nmf/mpc/fsm/tests/output.lst -o test_report_mpc_$(PLATFORM); \
	 testdbcreate --tag $(MMDSPPROCESSOR) --testlist nmf/mpc/fsm/tests/output.lst -o test_report_mpc_$(PLATFORM) --html;
endif

cleantest:
ifeq ($(CORE_NUMBER),2)
	- $(MAKE) -C nmf/mpc/fsm/tests $@
endif
	rm -rf test_report*$(PLATFORM)*
