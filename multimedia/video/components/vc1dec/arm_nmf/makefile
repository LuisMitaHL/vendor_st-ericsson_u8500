## The MMDSP code is board specific
MMDSP_CODE_IS_BOARD_SPECIFIC:=true

include $(MMROOT)/shared/makefiles/Make.config

help :
	@echo "HELP ON LINE:\n"
	@echo "Make all     : compile all ARM nmf modules : arm ddep vc1dec and vc1dec_sw"
	@echo "Make clean   : clean these modules"
	@echo "Make install : install these modules"
	@echo ""
	@echo "Make test_appli		           : Build SW and HW version of test_appli + Arm-nmf codec component"
	@echo ""
	@echo "Make test_appli-install         : Install hw Arm-Nmf codec component + test appli"
	@echo "Make test_appli-all        	   : Build test-appli and hw arm nmf codec "
	@echo "Make test_appli-clean           : Clean hw one"
	@echo ""
	@echo "Make test_appli-install SW=1    : Install sw Arm-Nmf codec component + test appli"
	@echo "Make test_appli-all SW=1        : Build test-appli and sw arm nmf codec "
	@echo "Make test_appli-clean SW=1      : Clean sw one"

########## Force SW version if #############
export SELECTED_CONF SW
ifeq ($(PLATFORM),x86-linux)
SELECTED_CONF=_sw
endif

ifeq ($(SW),1)
SELECTED_CONF=_sw
endif

##### Top NMF build rules ######

all clean realclean install:
ifeq ($(PLATFORM),x86-linux)
	@$(MAKE) -C proxynmf_sw  $@
else
	@$(MAKE) -C proxynmf  $@
endif




##### Local rule to work only on test appli wo datadep #####

##### Build test appli for SW and HW version
test_appli:
ifeq ($(PLATFORM),x86-linux)
	@$(MAKE) test_appli-all SW=1
else
	@$(MAKE) test_appli-all
endif


##### Generic rule for install all clean of test_appli
test_appli-%:
	$(MAKE) codec-$* 
	$(MAKE) -C test_appli -f makefile $* 


##### Codec rules #####
codec-%:
	$(MAKE) -C vc1dec$(SELECTED_CONF) $*  



#####  Reference code ####
refcode:
	@$(MAKE) -C refcode

#include make-test

.PHONY : gcc-testall clean install testit realclean refcode test_appli
#include $(MMROOT)/shared/makefiles/Make.rules

