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
all: all-std all-nmf
install: install-std install-nmf
uninstall: uninstall-std uninstall-nmf
clean: clean-std clean-nmf
realclean: realclean-std realclean-nmf

%-std:
#install libeffects headers
	if $(MAKE) -f Make.install $* ; then :; else exit 1 ; fi ;\
# compile the libraries for host (should be usable for both standalone and nmf)
	if $(MAKE) -C libmixer $* ; then :; else exit 1 ; fi ;\
	if $(MAKE) -C libresampling $* ; then :; else exit 1 ; fi ;\
	if $(MAKE) -C libvolctrl $* ; then :; else exit 1 ; fi ;\
	if $(MAKE) -C libspl $* ; then :; else exit 1 ; fi ;\
	if $(MAKE) -C libtransducer_equalizer $* ; then :; else exit 1 ; fi ;\
	if $(MAKE) -C libmdrc5b $* ; then :; else exit 1 ; fi ;

ifeq ($(CORE_NUMBER),2)
# then compile the libraries for mpc
	if $(MAKE) -C libmixer $* $(CORE_FLAG) ; then :; else exit 1 ; fi ;\
	if $(MAKE) -C libresampling $* $(CORE_FLAG) ; then :; else exit 1 ; fi ;\
	if $(MAKE) -C libvolctrl $* $(CORE_FLAG) ; then :; else exit 1 ; fi ;\
	if $(MAKE) -C libspl $* $(CORE_FLAG) ; then :; else exit 1 ; fi ;\
	if $(MAKE) -C libtransducer_equalizer $* $(CORE_FLAG) ; then :; else exit 1 ; fi ;\
	if $(MAKE) -C libmdrc5b $* $(CORE_FLAG) ; then :; else exit 1 ; fi ;
	
endif
	
%-nmf:
ifeq ($(CORE_NUMBER),2)
	if $(MAKE) -C mpc $* ; then :; else exit 1 ; fi ;\
	
endif
	if $(MAKE) -C inc $* ; then :; else exit 1 ; fi ;\


# -------------
# Test
# -------------
TESTTAG = $(MMDSPPROCESSOR)

test: test-std test-nmf
	@$(MAKE) mergereports
	@echo Test done

# add mpc nmf components testing if the platform supports it
ifeq ($(CORE_NUMBER),2)
NMF_DIRECTORIES += $(wildcard mpc)
endif

STD_DIRECTORIES = $(wildcard */standalone)

REGRESSION_DIRECTORIES = $(wildcard lib*/test )

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
cleantest: cleantest-std cleantest-reports

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

cleantest-reports:
	rm -f test_report_*
	rm -f mpc/*.html
	rm -f lib*/standalone/.mmdsp_history
	rm -f lib*/standalone/*.html
	rm -f lib*/standalone/*_mmdsp_x*
	rm -f lib*/test/.mmdsp_history
	rm -rf lib*/test/out/
	rm -f lib*/test/simcmd_cortexA9
	rm -rf mpc/lib*/test/build/	
	rm -f mpc/lib*/test/.mmdsp_history
	rm -f mpc/lib*/test/cmd
	rm -f mpc/lib*/test/*.log
	rm -f mpc/lib*/test/*.dcv
	rm -f mpc/lib*/test/*.pcv
	rm -f mpc/lib*/test/*.prf
	rm -f mpc/lib*/test/*.sort

#TESTDBS computation must be done after reports generation
TESTDBS = $(wildcard lib*/standalone/misc_std_$(TESTTAG) )
TESTDBS += $(wildcard mpc/mpc_$(MMDSPPROCESSOR) )

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
