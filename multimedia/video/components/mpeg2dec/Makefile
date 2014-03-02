# -*- Mode: Makefile -*-
# vim:syntax=make:

#export MODULE
export PLATFORM
export SVA_SUBSYSTEM

#MODULE = jpegdec

all : version
	make -C mpc/algo
ifndef NO_OMX_BUILD
	make -C mpc/ddep
	make -C arm_nmf
	make -C proxy
endif
	 @if test -e Makefile.group; then if $(MAKE) -f Makefile.group $* ;\
	 then :; else exit 1 ; fi ; fi;


install :
	make -C mpc/algo $@
ifndef NO_OMX_BUILD
	make -C mpc/ddep $@
	make -C arm_nmf $@
	make -C proxy $@
endif
clean realclean:
	rm -f version
	make -C mpc/algo $@
ifndef NO_OMX_BUILD
	make -C mpc/ddep $@
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
