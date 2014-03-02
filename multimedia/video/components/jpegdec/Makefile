# -*- Mode: Makefile -*-
# vim:syntax=make:

#export MODULE
export PLATFORM
export SVA_SUBSYSTEM

#MODULE = jpegdec

all : version
ifneq ($(PLATFORM),x86-linux)
	make -C mpc/algo $@
endif
ifndef NO_OMX_BUILD
ifneq ($(PLATFORM),x86-linux)
	make -C mpc/ddep $@
endif

	make -C arm_nmf $@
	make -C proxy $@
endif
	 @if test -e Makefile.group; then if $(MAKE) -f Makefile.group $* ;\
	 then :; else exit 1 ; fi ; fi;


install :
ifneq ($(PLATFORM),x86-linux)
	make -C mpc/algo $@
endif
ifndef NO_OMX_BUILD
ifneq ($(PLATFORM),x86-linux)
	make -C mpc/ddep $@
endif	

	make -C arm_nmf $@
	make -C proxy $@
endif
clean realclean:
	rm -f version
ifneq ($(PLATFORM),x86-linux)	
	make -C mpc/algo $@
endif
ifndef NO_OMX_BUILD
ifneq ($(PLATFORM),x86-linux)
	make -C mpc/ddep $@
endif	

	make -C arm_nmf $@
	make -C proxy $@
endif

version: 
	rm -f version

#### Checking constraint ##############
check_constraint:
ifndef NO_OMX_BUILD
	make -C proxy $@
endif
#include $(MMROOT)/video/components/Make.doc

.PHONY : version
