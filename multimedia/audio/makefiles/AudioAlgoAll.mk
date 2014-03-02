# -*- Mode: Makefile -*-
# vim:syntax=make:
include $(MM_MAKEFILES_DIR)/SharedConfig.mk

ifeq ($(NO_STOP), 1)
CONTINUE = -
endif	

ifeq ($(MMPROCESSOR),x86)
NMF_NOT_SUPPORTED = 1
endif
ifeq ($(MMPROCESSOR),x86_mmdsp)
NMF_NOT_SUPPORTED = 1
endif
ifeq ($(MMPROCESSOR),x86_cortexA9)
NMF_NOT_SUPPORTED = 1
endif


# -------------
# Compilation
# -------------
ifneq ($(MYMMSYSTEM), symbian)
all: all-std all-nmf
install: install-std install-nmf
uninstall: uninstall-std uninstall-nmf
clean: clean-std clean-nmf
realclean: realclean-std realclean-nmf
else # symbian
all: all-nmf
install: install-nmf
uninstall: uninstall-nmf
clean: clean-nmf
realclean: realclean-nmf
endif

%-std:
	if test -e Make.install ; then \
		if $(MAKE) -f Make.install $* ; then :; else exit 1 ; fi ;\
        fi ;

ifneq ($(MYMMSYSTEM), symbian)
# compile the libraries for host (should be usable for both standalone and nmf)
	@if test -d codec ; then \
	   if $(MAKE) -C codec $* ; then :; else exit 1 ; fi ;\
	fi ;
	@if test -d effect ; then \
	   if $(MAKE) -C effect $* ; then :; else exit 1 ; fi ;\
	fi ;
	@if test -d cil ; then \
	   if $(MAKE) -C cil $* ; then :; else exit 1 ; fi ;\
	fi ;
	@if test -d eil ; then \
	   if $(MAKE) -C eil $* ; then :; else exit 1 ; fi ;\
	fi ;

ifeq ($(CORE_NUMBER),2)
# then compile the libraries for mpc
	@if test -d codec ; then \
	   if $(MAKE) -C codec $* $(CORE_FLAG) ; then :; else exit 1 ; fi ;\
	fi ;
	@if test -d effect ; then \
	   if $(MAKE) -C effect $*  $(CORE_FLAG) ; then :; else exit 1 ; fi ;\
	fi ;
	@if test -d cil ; then \
	   if $(MAKE) -C cil $*  $(CORE_FLAG) ; then :; else exit 1 ; fi ;\
	fi ;
	@if test -d eil ; then \
	   if $(MAKE) -C eil $*  $(CORE_FLAG) ; then :; else exit 1 ; fi ;\
	fi ;
	@if test -d doppler ; then \
	   if $(MAKE) -C doppler $*  $(CORE_FLAG) ; then :; else exit 1 ; fi ;\
	fi ;
	@if test -d st3dpos ; then \
	   if $(MAKE) -C st3dpos $*  $(CORE_FLAG) ; then :; else exit 1 ; fi ;\
	fi ;
endif
else
	echo "Standalone build skipped for x-symbian platform"
endif

%-nmf:
ifeq ($(CORE_NUMBER),2)
	@if test -e Make.nmf ; then \
		if $(MAKE) -f Make.nmf $* ; then :; else exit 1 ; fi ;\
	fi ;
endif	
	@if test -e  inc/Makefile; then \
		if $(MAKE) -C inc -f Makefile $* ; then :; else exit 1 ; fi ;\
	fi ;

ifneq ($(NMF_NOT_SUPPORTED),1)
	@if test -e Make.host.nmf ; then \
		if $(MAKE) -f Make.host.nmf $* ; then :; else exit 1 ; fi ;\
	fi ;

  ifneq ($(MYMMSYSTEM), symbian) # do not compile proxies for symbian platforms...
	@if test -d host ; then \
		if $(MAKE) -C host $* ; then :; else exit 1 ; fi ;\
	fi ;

     ifneq ($(MMPROCESSOR), x86)
	@if test -d proxy ; then \
		if $(MAKE) -C proxy $* ; then :; else exit 1 ; fi ;\
        fi ;
     else
	@if test -e Make.host.nmf ; then \
		if test -d proxy ; then \
			if $(MAKE) -C proxy $* ; then :; else exit 1 ; fi ;\
		fi ;\
	fi ;
     endif
	@if test -d bellagio ; then \
		if $(MAKE) -C bellagio $* ; then :; else exit 1 ; fi ;\
        fi ;

  endif # symbian
else # x86_mmdsp
	@echo "Proxies and NMF on Host compilations skipped (x86_mmdsp or x86_cortexA9 targets)"
endif


# -------------
# Test
# -------------
TESTTAG = $(MMDSPPROCESSOR)

ifneq ($(MYMMSYSTEM), symbian)
test: test-std test-nmf
	@$(MAKE) mergereports
else # symbian
test:
endif

# nmf component unitary testing (host components only)
NMF_DIRECTORIES = $(wildcard nmfil/host/decoder/test nmfil/host/encoder/test nmfil/host/wrapper/test nmfil/host/effect/test)

# add mpc nmf components testing if the platform supports it
ifeq ($(CORE_NUMBER),2)
NMF_DIRECTORIES += $(wildcard nmfil/decoder/test nmfil/encoder/test nmfil/wrapper/test nmfil/effect/test \
                     */nmfil/*/test */nmfil/effect/test)
endif

#addition of eventual omx tests on svp or x86
ifeq ($(findstring svp,$(MMPLATFORM)),svp)
NMF_DIRECTORIES += $(wildcard mmte_script)
endif
ifeq ($(MMPLATFORM), x86)
NMF_DIRECTORIES += $(wildcard mmte_script)
endif
ifeq ($(findstring href_v1,$(MMPLATFORM)),href_v1)
NMF_DIRECTORIES += $(wildcard mmte_script)
endif
ifeq ($(findstring u8500_v2,$(MMPLATFORM)),u8500_v2)
NMF_DIRECTORIES += $(wildcard mmte_script)
endif

STD_DIRECTORIES = $(wildcard */standalone)

REGRESSION_DIRECTORIES = $(wildcard codec/test effect/test )


test-nmf:
ifneq ($(NMF_NOT_SUPPORTED),1)
	@echo "NMF_DIRECTORIES=$(NMF_DIRECTORIES)"
	- @for dir in  $(NMF_DIRECTORIES); do \
	    $(MAKE) -C $$dir test LSF=$(LSF) ;\
	done
else
	@echo "NMF test skipped (x86_mmdsp or x86_cortexA9 target)"
endif

test-std :
	- @for dir in $(STD_DIRECTORIES); do \
	    $(MAKE) -C $$dir test LSF=$(LSF) ;\
	done
ifeq ($(CORE_NUMBER),2)
	- @for dir in $(STD_DIRECTORIES); do \
	    $(MAKE) -C $$dir test $(CORE_FLAG) LSF=$(LSF) ;\
	done
endif

ifneq ($(NMF_NOT_SUPPORTED),1)
cleantest: cleantest-nmf
endif
cleantest: cleantest-std

cleantest-std:
	- @for dir in  $(STD_DIRECTORIES) ; do \
        $(MAKE) -C $$dir cleantest; \
	done
ifeq ($(CORE_NUMBER),2)
	- @for dir in  $(STD_DIRECTORIES); do \
	    $(MAKE) -C $$dir $(CORE_FLAG) cleantest ;\
	done
endif

cleantest-nmf:
	- @for dir in  $(NMF_DIRECTORIES); do \
        $(MAKE) -C $$dir cleantest; \
	done
ifeq ($(CORE_NUMBER),2)
	- @for dir in  $(NMF_DIRECTORIES); do \
	    $(MAKE) -C $$dir $(CORE_FLAG) cleantest ;\
	done
endif

#TESTDBS computation must be done after reports generation
TESTDBS = $(wildcard nmfil/decoder/test/misc_nmf_$(TESTTAG) )
TESTDBS+= $(wildcard nmfil/encoder/test/misc_nmf_$(TESTTAG) )
TESTDBS+= $(wildcard nmfil/effect/test/misc_nmf_$(TESTTAG) )
TESTDBS+= $(wildcard nmfil/wrapper/test/misc_nmf_$(TESTTAG) )
TESTDBS+= $(wildcard codec/test/regression_std_$(TESTTAG) )
TESTDBS+= $(wildcard codec/test/regression_std_$(MMPROCESSOR) )
TESTDBS+= $(wildcard effect/test/regression_std_$(TESTTAG) )
TESTDBS+= $(wildcard effect/test/regression_std_$(MMPROCESSOR) )
TESTDBS+= $(wildcard mmte_script/omx_$(PLATFORM))
TESTDBS+= $(wildcard */nmfil/*/test/misc_nmf_$(TESTTAG) )
TESTDBS+= $(wildcard */standalone/misc_std_$(TESTTAG) )
TESTDBS+= $(wildcard */standalone/misc_std_$(MMPROCESSOR) )

mergereports:
ifneq ($(words $(TESTDBS)),0)
	- testdbmerge -o test_report_$(PLATFORM) $(addprefix -i , $(TESTDBS)) --details  --html 
	$(CONTINUE) testdbmerge -o test_report_$(PLATFORM) $(addprefix -i , $(TESTDBS)) --details 
else
	@echo "No testdb found, no merge"
endif


regression:
	- @for dir in  $(REGRESSION_DIRECTORIES) ; do \
	   if $(MAKE) -C $$dir test $(CORE_FLAG) LSF=$(LSF); then :; else exit 1 ; fi ;\
	   if $(MAKE) -C $$dir test LSF=$(LSF); then :; else exit 1 ; fi ;\
	   testdbmerge -o regression_report_$(PLATFORM) -i $$dir/regression_std_$(TESTTAG) --details  --html ;\
       testdbmerge -o regression_report_$(PLATFORM) -i $$dir/regression_std_$(TESTTAG) --details ;\
	done


cleanregression:
	- @for dir in  $(REGRESSION_DIRECTORIES) ; do \
        $(MAKE) -C $$dir cleantest; \
	done
