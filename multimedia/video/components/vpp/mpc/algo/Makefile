# -*- Mode: Makefile -*-
# vim:syntax=make:

export PLATFORM
export SVA_SUBSYSTEM

all : std-exe nmf-all
install: std-install nmf-install
clean : std-clean nmf-clean

realclean : std-realclean nmf-realclean nmf-conf-clean

std-exe std-install std-clean std-realclean header todo myexe:
ifdef NO_NMF_BUILD
	make -C ../../ -f Make.std $(@:std-%=%)
endif  #NO_NMF_BUILD

nmf-all nmf-install nmf-idl-doc nmf-clean nmf-realclean nmf-conf-clean:
ifndef NO_NMF_BUILD
	make -f Make.nmf $(@:nmf-%=%) 
endif

#include $(MMROOT)/video/components/Make.doc

