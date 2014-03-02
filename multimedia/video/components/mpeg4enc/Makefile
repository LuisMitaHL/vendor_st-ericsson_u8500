# -*- Mode: Makefile -*-
# vim:syntax=make:
export PLATFORM
export SVA_SUBSYSTEM

all :
ifneq ($(PLATFORM),x86-linux)
ifneq ($(findstring 5500,$(PLATFORM)),5500)
	make -C mpc/algo $@
endif
endif	
ifndef NO_OMX_BUILD
ifneq ($(PLATFORM),x86-linux)
ifneq ($(findstring 5500,$(PLATFORM)),5500)
	make -C mpc/ddep $@
	make -C mpc/brc $@
endif
endif	
	make -C arm_nmf $@
	make -C proxy $@
endif
	 @if test -e Makefile.group; then if $(MAKE) -f Makefile.group $* ;\
	 then :; else exit 1 ; fi ; fi;


install :
ifneq ($(PLATFORM),x86-linux)
ifneq ($(findstring 5500,$(PLATFORM)),5500)
	make -C mpc/algo $@
endif
endif	
ifndef NO_OMX_BUILD
ifneq ($(PLATFORM),x86-linux)
ifneq ($(findstring 5500,$(PLATFORM)),5500)
	make -C mpc/ddep $@
	make -C mpc/brc $@
endif	
endif
	make -C arm_nmf $@
	make -C proxy $@
endif


clean realclean:
ifneq ($(PLATFORM),x86-linux)	
ifneq ($(findstring 5500,$(PLATFORM)),5500)
	make -C mpc/algo $@
endif	
endif
ifndef NO_OMX_BUILD
ifneq ($(PLATFORM),x86-linux)
ifneq ($(findstring 5500,$(PLATFORM)),5500)
	make -C mpc/ddep $@
	make -C mpc/brc $@
endif	
endif	
	make -C arm_nmf $@
	make -C proxy $@
endif

#### Checking constraint ##############
check_constraint:
ifndef NO_OMX_BUILD
	make -C proxy $@
endif

.PHONY : version
