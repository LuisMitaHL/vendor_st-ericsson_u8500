# -*- Mode: Makefile -*-
# vim:syntax=make:

export MODULE

ifndef NO_NMF_BUILD
ifndef NO_STD_BUILD #!NO_STD_BUILD & !NO_NMF_BUILD
#in case makefile directive are not cumulative (other make version -> RPM-devel build)
all: std-all nmf-all std-install nmf-install
install: std-install nmf-install
clean: std-clean nmf-clean
uninstall: std-uninstall nmf-uninstall
realclean: std-realclean nmf-conf-clean nmf-realclean
else  #NO_STD_BUILD & !NO_NMF_BUILD
all: nmf-all std-install nmf-install
# std-install for nmf is used for idt installation (not yet possible with nmf generic makefiles)
install: std-install nmf-install
clean: nmf-clean
uninstall: nmf-uninstall
realclean: nmf-conf-clean nmf-realclean
endif
else   
ifndef NO_STD_BUILD  #!NO_STD_BUILD & NO_NMF_BUILD
all: std-all
install: std-install
clean: std-clean
uninstall: std-uninstall
realclean: std-realclean
else  #NO_STD_BUILD & NO_NMF_BUILD
#in that case all directive may be required 
all:
endif
endif

std-%:
	@if test -e Make.std; then if $(MAKE) -f Make.std $* ;\
	then :; else exit 1 ; fi ; fi;

nmf-%:
	@if test -e Make.nmf; then if $(MAKE) -f Make.nmf $* ;\
	then :; else exit 1 ; fi ; fi;

