#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

include $(MM_MAKEFILES_DIR)/SharedCheck.mk

all clean realclean install uninstall debug:
ifeq ($(CORE_NUMBER),2)
	$(MAKE) -C api $@
	$(MAKE) -C mmdsp $@
endif
	$(MAKE) -C arm $@
ifneq ($(findstring 5500,$(MYMMBOARD)),5500)
ifeq ($(BUILD_MMDSP_NMF),src)
	if [ -d test ] ; then $(MAKE) -C test/tu_ost_test_common $@ ; fi
	if [ -d test ] ; then $(MAKE) -C test/tu_ost_test1 $@ ; fi
ifneq ($(findstring x86,$(MYMMBOARD)),x86)
	if [ -d test ] ; then $(MAKE) -C test/tu_ost_test2 $@ ; fi
endif
endif
endif

ifeq ($(findstring x86,$(MYMMBOARD)),x86)
test1:
	export STE_OSTTRACE_TO_FILE=ost_trace_test1.txt && ./test/tu_ost_test1/linux/host/bin/x86/st_osttest1.exe
	perl -S test/verify.pl --action=diff --defines=input1=ost_trace_test1.txt --defines=input2=test/ref/tu_ost_test1/x86_linux/ost_trace_ref.txt
	@if [ $$? -eq 0 ] ; then \
		echo "******************" ; \
		echo "TEST1: SUCESSFULL :)" ; \
		echo "******************" ; \
		echo "TEST1: SUCESSFULL" > test_log.txt; \
	else \
		echo "******************" ; \
		echo "TEST: FAILED (-"; \
		echo "******************" ; \
		echo "TEST1: FAILED" > test_log.txt; \
	fi

test: test1
	@cat test_log.txt
else
test:
	@test/test.sh
	@cat test_log.txt
endif
.PHONY : test test1 clean_test







