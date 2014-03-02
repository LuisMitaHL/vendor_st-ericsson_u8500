

CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))../)
include ./config/config.mk

.PHONY: help
help:
	@echo "#################################### HELP ########################################"
	@echo "#                                                                                #"
	@echo "# Following are the targets built :-                                             #"
	@echo "#                                                                                #"
	@echo "##################################################################################"
	@echo "#                                                                                #"
	@echo "# lbsstelptest                                                                   #"
	@echo "# libtestengine                                                                  #"
	@echo "# lbsd                                                                           #"
	@echo "#                                                                                #"
	@echo "##################################################################################"
	@echo "#                                                                                #"
	@echo "# Build Options can be changed from command line to enable/disable features      #"
	@echo "# By defualt, all options are in the enabled (true) state                        #"
	@echo "#                                                                                #"
	@echo "##################################################################################"
	@echo "#                                                                                #"
	@echo "# 1. ENABLE_CP : true/false                                                      #"
	@echo "# 2. ENABLE_UP : true/false                                                      #"
	@echo "# 3. ENABLE_FREQ_AIDING : true/false                                             #"
	@echo "# 4. GENERIC_SOLUTION : true/false                                               #"
	@echo "# 5. DEBUG : true/false                                                          #"
	@echo "#                                                                                #"
	@echo "##################################################################################"
	@echo "#                                                                                #"
	@echo "# To use cross compiler, use CROSS_COMPILE=<> in the command line                #"
	@echo "# For example, to use arm-eabi-gcc to build lbsd, use the command                #"
	@echo "#              make CROSS_COMPILE=arm-eabi- lbsd                                 #"
	@echo "#                                                                                #"
	@echo "##################################################################################"

.PHONY: all
all: liblbstestengine lbsstelptest lbsd

.PHONY: lbsstelptest 
lbsstelptest: clean-stelp
	@$(MAKE) -C lbsstelptest
	
.PHONY: liblbstestengine
liblbstestengine: clean-testengine
	$(MAKE) -C liblbstestengine

ifeq ($(LBS_UBUNTU),true)
.PHONY: lbsd
lbsd: clean-lbsd cgps lsim gns halgps agpscodec libagpsosa lbscfg agpsmain
else
ifeq ($(UBUNTU_X86),true)
.PHONY: lbsd
ifeq ($(ENABLE_UP),true)
lbsd: clean-lbsd cgps lsim gns halgps agpscodec libagpsosa supl lbscfg agpsmain
else
lbsd: clean-lbsd cgps lsim gns halgps agpscodec libagpsosa lbscfg agpsmain
endif
else

.PHONY: lbsd
lbsd: clean-lbsd cgps lsim gns halgps agpscodec libagpsosa supl agpsmain
endif
endif

.PHONY: cgps
cgps:
	@$(MAKE) -C lbsd/cgps
ifeq ($(LBS_UBUNTU),true)
.PHONY: lbsclient
lbsclient: clean-lbsclient
	$(MAKE) -C lbsclient
endif
ifeq ($(UBUNTU_X86),true)
.PHONY: lbsclient
lbsclient: clean-lbsclient
	$(MAKE) -C lbsclient
endif
.PHONY: lsim
lsim:
	@$(MAKE) -C lbsd/lsim

.PHONY: gns
gns:
	@$(MAKE) -C lbsd/gns

.PHONY: halgps
halgps:
	@$(MAKE) -C lbsd/halgps

.PHONY: libagpsosa
libagpsosa:
	@$(MAKE) -C lbsd/libagpsosa

.PHONY: agpscodec
agpscodec:
	@$(MAKE) -C lbsd/agpscodec
ifeq ($(LBS_UBUNTU),true)
.PHONY: lbscfg
lbscfg:
	$(MAKE) -C lbsd/lbscfg
endif
ifeq ($(UBUNTU_X86),true)
.PHONY: lbscfg
lbscfg:
	$(MAKE) -C lbsd/lbscfg
endif
.PHONY: agpsmain
agpsmain:
	@$(MAKE) -C lbsd/agpsmain

.PHONY: supl
supl:
	@$(MAKE) -C lbsd/supl

.PHONY: clean-all
ifeq ($(LBS_UBUNTU),true)
clean-all: clean-stelp clean-testengine clean-lbsclient clean-lbsd
else
ifeq ($(UBUNTU_X86),true)
clean-all: clean-stelp clean-testengine clean-lbsclient clean-lbsd
else
clean-all: clean-stelp clean-testengine clean-lbsd
endif
endif

.PHONY: clean-stelp
clean-stelp:
	@$(RM) lbsstelptest/lib
	@$(RM) lbsstelptest/obj

.PHONY: clean-testengine
clean-testengine:
	$(RM) liblbstestengine/lib
	$(RM) liblbstestengine/obj

ifeq ($(LBS_UBUNTU),true)
.PHONY: clean-lbsclient
clean-lbsclient:
	$(RM) lbsclient/lib
	$(RM) lbsclient/obj
endif
ifeq ($(UBUNTU_X86),true)
.PHONY: clean-lbsclient
clean-lbsclient:
	$(RM) lbsclient/lib
	$(RM) lbsclient/obj
endif

.PHONY: clean-lbsd
clean-lbsd:
	@$(RM) lbsd/bin
	@$(RM) lbsd/cgps/lib
	@$(RM) lbsd/lsim/lib
	@$(RM) lbsd/halgps/lib
	@$(RM) lbsd/agpsmain/lib
	@$(RM) lbsd/libagpsosa/lib
	@$(RM) lbsd/agpscodec/lib
	@$(RM) lbsd/gns/lib
	@$(RM) lbsd/supl/lib
ifeq ($(LBS_UBUNTU),true)
	@$(RM) lbsd/lbscfg/lib
	@$(RM) lbsd/lbscfg/obj
endif
ifeq ($(UBUNTU_X86),true)
	@$(RM) lbsd/lbscfg/lib
	@$(RM) lbsd/lbscfg/obj
endif
	@$(RM) lbsd/cgps/obj
	@$(RM) lbsd/lsim/obj
	@$(RM) lbsd/halgps/obj
	@$(RM) lbsd/agpsmain/obj
	@$(RM) lbsd/libagpsosa/obj
	@$(RM) lbsd/agpscodec/obj
	@$(RM) lbsd/gns/obj
	@$(RM) lbsd/supl/obj
	@$(RM) lbsd/agpsmain/srce/*.o
