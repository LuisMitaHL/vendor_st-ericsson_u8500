# vim:syntax=make:
# -*- Mode: Makefile -*-
include $(MM_MAKEFILES_DIR)/AudioAll.mk

#all:
#	make -C common

doxygen: cscall.dox
	doxygen $<


publish:
	@echo "Checking cscall version"
	@sh update_release_version.sh
	@echo "Publishing cscall"
	@svn up .
	@rodos publish -c cscall --pktype="pack_8500_symbian,pack_8500_linux" -p none
