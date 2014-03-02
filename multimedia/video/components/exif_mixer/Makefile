# -*- Mode: Makefile -*-
# vim:syntax=make:

#export MODULE
export PLATFORM
export SVA_SUBSYSTEM

#MODULE = exif_mixer

all : version
ifndef NO_OMX_BUILD
	make -C arm_nmf
	make -C proxy
endif
install :
ifndef NO_OMX_BUILD
	make -C arm_nmf $@
	make -C proxy $@
endif
clean realclean:
	rm -f version
ifndef NO_OMX_BUILD
	make -C arm_nmf $@
	make -C proxy $@
endif

version: 
	rm -f version

#### Checking constraint  ##############
check_constraint:
ifndef NO_OMX_BUILD
	make -C proxy $@
endif
#include $(MMROOT)/video/components/Make.doc

.PHONY : version
