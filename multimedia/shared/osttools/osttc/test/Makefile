# vim:syntax=make:
# -*- Mode: Makefile -*-
#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

#all: init
include make.ost_conf
include $(MM_MAKEFILES_DIR)/SharedDefines.mk
OST_SRC_DIR = src
include $(MM_MAKEFILES_DIR)/SharedTraceCompiler.mk
OST_SRC_DIR = tmp

TMPDIR:= tmp
init: | cleantest

$(TMPDIR):
	mkdir -p $(TMPDIR)
#	cp src/st_osttc_test1.c $(TMPDIR)/st_osttc_test1.c
#	cp src/st_osttc_test1.c $(TMPDIR)/st_osttc_test2.c

#init: | $(TMPDIR)
init1: | $(TMPDIR)
	@echo "*******TEST1********\n"
	cp src/st_osttc_test1.c $(TMPDIR)/st_osttc_test1.c

init2: | $(TMPDIR)
	@echo "*******TEST2********\n"
	cp src/st_osttc_test2.c $(TMPDIR)/st_osttc_test2.c

init3: | $(TMPDIR)
	@echo "*******TEST3********\n"
	cp src/st_osttc_test2.c $(TMPDIR)/st_osttc_test2.c && \
	echo "	OstTraceInt0 (TRACE_ERROR,  			\"ST_OSTTC_TEST3 TRACE_ERROR: 				OstTraceInt0\");" >> $(TMPDIR)/st_osttc_test2.c	

init4: | $(TMPDIR)
	@echo "*******TEST4********\n"
	echo "	OstTraceInt0 (TRACE_ERROR,  			\"ST_OSTTC_TEST4 TRACE_ERROR: 				OstTraceInt0\");" >> $(TMPDIR)/st_osttc_test1.c

init5: | $(TMPDIR)
	@echo "*******TEST5********\n"
	cp src/st_osttc_test3.c $(TMPDIR)/st_osttc_test3.c

# Test trace compiler with one file and all groups 
test1: init1
	@echo "OST_SRC_LIST: $(OST_SRC_LIST)"
	$(MAKE) -f Make.test all
	@perl verify.pl --action=verify --defines=refdir=ref/$@
	@if [ $$? -eq 0 ] ; then \
		echo "******************" ; \
		echo "TEST1: SUCESSFULL :)" ; \
		echo "******************" ; \
		echo "TEST1: SUCESSFULL" > test_log.txt; \
	else \
		echo "******************" ; \
    echo "TEST1: FAILED (-"; \
    echo "******************" ; \
    echo "TEST1: FAILED" > test_log.txt; \
  fi
	#rm $(TMPDIR)/autogentraces/tracecompile_$(OST_COMPONENT_NAME)_$(OST_COMPONENT_ID).done

# Test trace compiler with one more file added
test2: init2
	@echo "OST_SRC_LIST: $(OST_SRC_LIST)"
	$(MAKE) -f Make.test all
	$(VERCHAR)perl verify.pl --action=verify --defines=refdir=ref/$@
	@if [ $$? -eq 0 ] ; then \
		echo "******************" ; \
		echo "TEST2: SUCESSFULL :)" ; \
		echo "******************" ; \
		echo "TEST2: SUCESSFULL" >> test_log.txt; \
	else \
		echo "******************" ; \
    echo "TEST2: FAILED (-"; \
    echo "******************" ; \
    echo "TEST2: FAILED" >> test_log.txt; \
  fi	


# Test: Insure trace compiler correclty regenerate header file if last file is updated with trace added at the end
test3: init3
	@echo "OST_SRC_LIST: $(OST_SRC_LIST)"
	$(MAKE) -f Make.test all
	$(VERCHAR)perl verify.pl --action=verify --defines=refdir=ref/$@
	@if [ $$? -eq 0 ] ; then \
		echo "******************" ; \
		echo "TEST3: SUCESSFULL :)" ; \
		echo "******************" ; \
		echo "TEST3: SUCESSFULL" >> test_log.txt; \
	else \
		echo "******************" ; \
    echo "TEST3: FAILED (-"; \
    echo "******************" ; \
    echo "TEST3: FAILED" >> test_log.txt; \
  fi

# Test: Insure trace compiler correclty regenerate header file if last file is updated with trace added at the end
test4: init4
	@echo "OST_SRC_LIST: $(OST_SRC_LIST)"
	$(MAKE) -f Make.test all	
	$(VERCHAR)perl verify.pl --action=verify --defines=refdir=ref/$@
	@if [ $$? -eq 0 ] ; then \
		echo "******************" ; \
		echo "TEST4: SUCESSFULL :)" ; \
		echo "******************" ; \
		echo "TEST4: SUCESSFULL" >> test_log.txt; \
	else \
		echo "******************" ; \
    echo "TEST4: FAILED (-"; \
    echo "******************" ; \
    echo "TEST4: FAILED" >> test_log.txt; \
  fi

# Test: Insure trace compiler correclty regenerate header file if last file is updated with trace added at the end
test5: init5
	@echo "OST_SRC_LIST: $(OST_SRC_LIST)"
	$(MAKE) -f Make.test all
	$(VERCHAR)perl verify.pl --action=verify --defines=refdir=ref/$@
	@if [ $$? -eq 0 ] ; then \
		echo "******************" ; \
		echo "TEST5: SUCESSFULL :)" ; \
		echo "******************" ; \
		echo "TEST5: SUCESSFULL" >> test_log.txt; \
	else \
		echo "******************" ; \
    echo "TEST5: FAILED (-"; \
    echo "******************" ; \
    echo "TEST5: FAILED" >> test_log.txt; \
  fi

test: init test1 test2 test3 test4 test5
	cat test_log.txt

cleantest:
	@echo "Cleaning test directory..."
	rm -rf $(TMPDIR)


install: install_dico
uninstall: ostuninstall

realclean:
	$(VERCHAR)$(RM) $(TMPDIR) ; true

.PHONY: ostuninstall init1 init2 init3 init4 cleantest test1 test2 test3 test4












