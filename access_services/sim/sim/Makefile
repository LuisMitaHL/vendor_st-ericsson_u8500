######################### -*- Mode: Makefile-Gmake -*- ########################
## Copyright (C) 2010, ST-Ericsson
## 
## File name       : Makefile
## Description     : For cat system.
## 
## Author          : Mats Bergstrom
## 
###############################################################################


MODEM_DIR := ../sim_msl

SUBDIRS := simd cat libpc libsimcom libsim libapdu tcat tapdu \
		uiccd tuicc \
		$(MODEM_DIR)

COVDIRS_tcat := libapdu libsim libsimcom simd cat
COVDIRS_tcat_all_pc := libapdu libpc libsim libsimcom simd uiccd cat
COVDIRS_tcat_client := cat libapdu libpc libsim libsimcom simd uiccd
COVDIRS_tuicc := cat libapdu libpc libsim libsimcom simd uiccd
COVDIRS_tapdu := libapdu libsimcom uiccd

gcovinfo_tcat := $(COVDIRS_tcat:%= -a %.info)
gcovinfo_tcat_all_pc := $(COVDIRS_tcat_all_pc:%= -a %.info)
gcovinfo_tcat_client := $(COVDIRS_tcat_client:%= -a %.info)
gcovinfo_tuicc := $(COVDIRS_tuicc:%= -a %.info)
gcovinfo_tapdu := $(COVDIRS_tapdu:%= -a %.info)

TARGETS := all clean clean_logs coverage coverity doxygen help instrument_funcs splint test

.PHONY: $(SUBDIRS) $(TARGETS)

all: $(SUBDIRS)
		@echo ""
		@echo "available targets: $(TARGETS)"
		@echo ""
all: TARGET=all

clean: $(SUBDIRS)
		@rm -fr dox		#remove doxygen files
		@rm -f dox.err	#remove doxygen files
		@rm -fr *.info	#remove code coverage files
		@rm -fr */*.gcda	#remove code coverage files
		@rm -fr */*.gcno	#remove code coverage files
clean: TARGET=clean

clean_logs:
		@rm -fr */*.log
		@rm -f *.log

coverage:
	@make clean
	@make
ifneq ($(TOP),)
		@./script/runAllModuleTests.sh tcat
		for dir in $(COVDIRS_tcat) ; do  \
			lcov -d $$dir -b $$dir -c -o $$dir.info ; \
		done
		lcov $(gcovinfo_tcat) -o covtest1.info

		@./script/runAllModuleTests.sh tcat_all_pc
		for dir in $(COVDIRS_tcat_all_pc) ; do  \
			lcov -d $$dir -b $$dir -c -o $$dir.info ; \
		done
		lcov $(gcovinfo_tcat_all_pc) -o covtest2.info

		@./script/runAllModuleTests.sh tcat_client
		for dir in $(COVDIRS_tcat_client) ; do  \
			lcov -d $$dir -b $$dir -c -o $$dir.info ; \
		done
		lcov $(gcovinfo_tcat_client) -o covtest3.info

		@./script/runAllModuleTests.sh tuicc
		for dir in $(COVDIRS_tuicc) ; do  \
			lcov -d $$dir -b $$dir -c -o $$dir.info ; \
		done
		lcov $(gcovinfo_tuicc) -o covtest4.info

		@./script/runAllModuleTests.sh tapdu
		for dir in $(COVDIRS_tapdu) ; do  \
			lcov -d $$dir -b $$dir -c -o $$dir.info ; \
		done
		lcov $(gcovinfo_tapdu) -o covtest5.info

		genhtml -o ../covtestfolder/ covtest1.info covtest2.info covtest3.info covtest4.info covtest5.info
		firefox ../covtestfolder/index.html &
else
		@echo "$TOP does not appear to be an Android top directory..."
endif

coverity:
ifneq ($(TOP),)
		@sh script/coverity.sh sim_sat_coverity
		firefox https://seldx080.lud.stericsson.com:1759
else
		@echo "$TOP does not appear to be an Android top directory..."
endif

doxygen:
ifneq ($(TOP),)
		@make clean
		doxygen doxygen.conf
		firefox ./dox/html/index.html
else
		@echo "$TOP does not appear to be an Android top directory..."
endif

help:
		@echo "available targets: $(TARGETS)"
		@echo ""

instrument_funcs: $(SUBDIRS)
instrument_funcs: TARGET=instrument_funcs

splint: $(SUBDIRS)
splint: TARGET=splint

test:
		@./script/runAllModuleTests.sh tcat
		@./script/runAllModuleTests.sh tcat_all_pc
		@./script/runAllModuleTests.sh tcat_client
		@./script/runAllModuleTests.sh tuicc
		@./script/runAllModuleTests.sh tapdu

?: help

$(SUBDIRS):
	$(MAKE) -C $@ $(MFLAGS) $(TARGET)


simd: libsim libapdu libsimcom cat libpc uiccd $(MODEM_DIR)

cat: libsimcom libsim libapdu libpc

libsim: $(MODEM_DIR) libsimcom
libuicc: $(MODEM_DIR) libsimcom

tcat: libsim libapdu libsimcom
tuicc: libsimcom libuicc
tapdu: libapdu
