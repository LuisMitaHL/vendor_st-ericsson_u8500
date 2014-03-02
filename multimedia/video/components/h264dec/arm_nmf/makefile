include $(MMROOT)/shared/makefiles/Make.config
include $(MMROOT)/video/makefiles/Make.config

COMPONENT_TOP_DIR=..

help :
	@echo "HELP ON LINE:\n"
	@echo "Make all     : compile all ARM nmf modules : arm ddep h264"
	@echo "Make clean   : clean these modules"
	@echo "Make install : install these modules"
	@echo ""
	@echo "Make test_appli          : Build test_appli + Arm-nmf codec component"
	@echo ""
	@echo "Make test_appli-install  : Install hw Arm-Nmf codec component + test appli"
	@echo "Make test_appli-all      : Build test-appli and arm nmf codec "
	@echo "Make test_appli-clean    : Clean hw one"
	@echo ""
	@echo "Make run                 : Test nmf codec hw version "
	@echo "Make test_high       	     : Test nmf codec high profile version on SVP (ndk8500a0)"



##### Top NMF build rules ######
all clean realclean install:
ifeq ($(PLATFORM),x86-linux)
	@$(MAKE) -C proxynmf_sw  $@
else
	@$(MAKE) -C proxynmf  $@
endif



##### Build test appli
test_appli:
	@$(MAKE) test_appli-all

##### Generic rule for install all clean of test_appli
test_appli-%:
	$(MAKE) codec-$* 
	$(MAKE) -C test_appli -f makefile $* 


##### Codec rules #####
codec-%:
ifeq ($(PLATFORM),x86-linux)
	$(MAKE) -C h264dec_sw $*
else
	$(MAKE) -C h264dec $*
endif


##### Running a basictest #####
run:
	@$(MAKE) testit


test_high:
	perl ./test_appli/script/testdec_SVPv1.pl ./test_appli/script/high_light.txt
  

###### Lauch test on all platform ######
TEST_STREAM=/prj/multimedia_shared/video/bitstreams/H264/ALLEGRO/Syntax/BP_L30_Syntax_80x96_v6.0/80x96/Allegro_Inter_CAVLC_B01_L30_5x6_6.0.26l

#will be simplified as sson as fix in launcher is done...
testit: dut


#####  Reference code ####
refcode:
	@$(MAKE) -C refcode

include make-test

.PHONY : gcc-testall clean install testit realclean refcode test_appli
#include $(MMROOT)/shared/makefiles/Make.rules

