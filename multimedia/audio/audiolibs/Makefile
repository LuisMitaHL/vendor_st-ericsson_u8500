# -*- Mode: Makefile -*-
# vim:syntax=make:
include $(MM_MAKEFILES_DIR)/SharedConfig.mk

ifeq ($(NO_STOP), 1)
CONTINUE = -
endif


#-----------------------
# List of Directory
#-----------------------
DIRECTORIES= libaudiotables libaudioutils libaudiowindows libbitstream libbtrue libcrc libdBconv libfft libfwmalloc libimdct libmdct libmdcttables libmp3dequan libmp3hybrid libmp3hybrid_analysis libpcmfile libpolyanalysis libpolysyn libvector libsbrqmf libauread libgec libiir libtools libmath libsoftpfloat #libgeqz
DIRECTORIES_TEST_MMDSP:= $(strip $(shell cat testlist_mmdsp.lst | awk '{split($$1,a,"/");print a[1]}'))

DIRECTORIES_HOST= libaudiotables libaudioutils libaudiowindows libbitstream libbtrue libcrc libdBconv libfft libfwmalloc libimdct libmdct libmdcttables libmp3dequan libmp3hybrid libmp3hybrid_analysis libpcmfile libpolyanalysis libpolysyn libvector libsbrqmf libauread libgec libiir libtools libmath #libgeqz #libsoftpfloat
DIRECTORIES_TEST_HOST:= $(strip $(shell cat testlist_arm.lst | awk '{split($$1,a,"/");print a[1]}'))

all: all-std all-nmf
install: install-std install-nmf
uninstall: uninstall-std uninstall-nmf

clean: clean-std clean-nmf
realclean: realclean-std realclean-nmf

%-std:
#install all audiolibs headers
	@if $(MAKE) -f Make.install $* ; then :; else exit 1 ; fi ;

# first build for Host

	@for dir in $(DIRECTORIES_HOST); do \
	if test -e $$dir/Makefile ; then \
	   if $(MAKE) -C $$dir $* ; then :; else exit 1 ; fi ;\
	   fi ;\
	done

ifeq ($(STANDALONE_COMPILATION),true)
	@for dir in $(DIRECTORIES_TEST_HOST); do \
	if test -e $$dir/test/Makefile ; then \
	   if $(MAKE) -C $$dir/test $* ; then :; else exit 1 ; fi ;\
	   fi ;\
	done
endif

ifeq ($(CORE_NUMBER),2)
# Then eventually build for mpc
	@for dir in $(DIRECTORIES); do \
	if test -e $$dir/Makefile ; then \
	   if $(MAKE) -C $$dir $* $(CORE_FLAG) ; then :; else exit 1 ; fi ;\
	   fi ;\
	done

ifeq ($(STANDALONE_COMPILATION),true)
	@for dir in $(DIRECTORIES_TEST_MMDSP); do \
	if test -e $$dir/test/Makefile ; then \
	   if $(MAKE) -C $$dir/test $* $(CORE_FLAG) ; then :; else exit 1 ; fi ;\
	   fi ;\
	done
endif
endif

test: test_report_$(PLATFORM)
	@echo Test done


ifeq ($(CORE_NUMBER),2)
test_report_$(PLATFORM): test_report_$(MMDSPPROCESSOR) test_report_$(MMPROCESSOR)
	- testdbmerge -i test_report_$(MMDSPPROCESSOR) -i test_report_$(MMPROCESSOR) -o $@
	$(CONTINUE) testdbmerge --html -i test_report_$(MMDSPPROCESSOR) -i test_report_$(MMPROCESSOR) -o $@
else
test_report_$(PLATFORM): test_report_$(MMDSPPROCESSOR)
	- @mv test_report_$(MMDSPPROCESSOR) test_report_$(PLATFORM)
	- @mv test_report_$(MMDSPPROCESSOR).html test_report_$(PLATFORM).html
endif

ifeq ($(CORE_NUMBER),2)
test_report_$(MMDSPPROCESSOR):CORE=mmdsp
endif

test_report_$(MMDSPPROCESSOR):
ifeq ($(CORE_NUMBER),2)
	- testengine -i testlist_mmdsp.lst -o testlist_mmdsp.lst --tag $(MMDSPPROCESSOR) --dbname test_report
else
	- testengine -i testlist_mmdsp.lst -o testlist_mmdsp.lst --tag $(MMPROCESSOR) --dbname test_report
endif

test_report_$(MMPROCESSOR):
	- testengine -i testlist_arm.lst -o testlist_arm.lst --tag $(MMPROCESSOR) --dbname test_report

cleantest:
	@for dir in $(DIRECTORIES); do \
		if test -e $$dir/test/Makefile ; then \
		if $(MAKE) -C $$dir/test realclean $(CORE_FLAG) ; then :; else exit 1 ; fi ;\
		fi ;\
		done
ifeq ($(CORE_NUMBER),2)
	@for dir in $(DIRECTORIES_HOST); do \
		if test -e $$dir/Makefile/test ; then \
		if $(MAKE) -C $$dir/test realclean ; then :; else exit 1 ; fi ;\
		fi ;\
		done
endif
	cleantests -i testlist_mmdsp.lst -t $(MMDSPPROCESSOR) -d test_report_$(MMDSPPROCESSOR)
	cleantests -i testlist_arm.lst -t $(MMPROCESSOR) -d test_report_$(MMPROCESSOR)
	@rm -f test_report_*

%-nmf:

# first build for Host
	@if test -e Make.host.nmf ; then \
	if $(MAKE) -f Make.host.nmf $*; then :; else exit 1 ; fi ; \
	else echo Make.host.nmf not found; fi ;

ifeq ($(CORE_NUMBER),2)
# Then eventually build for mpc
	@if test -e Make.nmf ; then \
	if $(MAKE) -f Make.nmf $(CORE_FLAG)  $*; then :; else exit 1 ; fi ; \
	else echo Make.nmf not found; fi ;
endif

export CORE
