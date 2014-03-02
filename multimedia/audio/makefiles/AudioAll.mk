# vim:syntax=make:
# -*- Mode: Makefile -*-
CORE=mmdsp
include $(MM_MAKEFILES_DIR)/SharedCheck.mk


all clean realclean install uninstall:
ifeq ($(CORE_NUMBER),2)
	@if test -e mpc;    then $(MAKE) -C mpc    $@ ; fi;
endif
	@if test -e nmf;       then $(MAKE) -C nmf      $@ ; fi;
	@if test -e common;    then $(MAKE) -C common   $@ ; fi;
	@if test -e host;      then $(MAKE) -C host     $@ ; fi;
	@if test -e proxy;     then $(MAKE) -C proxy    $@ ; fi;
	@if test -e bellagio;  then $(MAKE) -C bellagio $@ ; fi;
	@if test -e inc/Make.install;       then $(MAKE) -C inc -f Make.install     $@ ; fi;

test: 
ifeq ($(CORE_NUMBER),2)
	- if test -e mpc;    then $(MAKE) -C mpc    $@ ; fi;
endif
	- if test -e host;   then $(MAKE) -C host   $@ ; fi;
	- if test -e common; then $(MAKE) -C common $@ ; fi;
	- if test -e nmf;    then $(MAKE) -C nmf    $@ ; fi;
	- if test -e proxy;  then $(MAKE) -C proxy  $@ ; fi;
	@$(MAKE) mergereports

cleantest:
ifeq ($(CORE_NUMBER),2)
	- if test -e mpc;    then $(MAKE) -C mpc    $@ ; fi;
endif
	- if test -e host;   then $(MAKE) -C host   $@ ; fi;
	- if test -e common; then $(MAKE) -C common $@ ; fi;
	- if test -e nmf;    then $(MAKE) -C nmf    $@ ; fi;
	- if test -e proxy;  then $(MAKE) -C proxy  $@ ; fi;
	rm -rf test_report*$(PLATFORM)*

#TESTDBS computation must be done after reports generation
TESTDBS=$(wildcard nmf/mpc/mpc_$(MMDSPPROCESSOR) proxy/proxy_$(PLATFORM) nmf/host/host_$(MMDSPPROCESSOR) nmf/hst/hst_$(PLATFORM))
mergereports:
ifneq ($(words $(TESTDBS)),0)
	- testdbmerge -o test_report_$(PLATFORM) $(addprefix -i , $(TESTDBS) ) --details --html
	testdbmerge -o test_report_$(PLATFORM) $(addprefix -i , $(TESTDBS) ) --details
else
	@echo "No testdb found, no merge"
endif


