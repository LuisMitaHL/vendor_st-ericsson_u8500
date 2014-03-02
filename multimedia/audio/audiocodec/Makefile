# vim:syntax=make:
# -*- Mode: Makefile -*-

all clean realclean install uninstall:
	$(MAKE) -C mpc $@
	$(MAKE) -C proxy $@
	$(MAKE) -C bellagio $@

test cleantest regression:
	$(MAKE) -C mmte_script $@

all:

doxygen: audiocodec.dox
	doxygen $<

documentation:
	$(MAKE) doxygen

