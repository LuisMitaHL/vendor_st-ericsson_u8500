# -*- Mode: Makefile -*-
# vim:syntax=make:

export PLATFORM
export SVA_SUBSYSTEM

all : std-exe nmf-all
install: std-install nmf-install
clean : std-clean nmf-clean

realclean : std-realclean nmf-realclean nmf-conf-clean

std-exe std-install std-clean std-realclean header todo myexe:
ifndef NO_STD_BUILD
	make -C ../../ -f Make.std $(@:std-%=%) 
endif #NO_STD_BUILD

nmf-all nmf-install nmf-idl-doc nmf-clean nmf-realclean nmf-conf-clean nmf-doc-clean:
ifndef NO_NMF_BUILD
	make -f Make.nmf $(@:nmf-%=%)
endif #NO_NMF_BUILD

#include $(MMROOT)/video/components/Make.doc

