# -*- Mode: Makefile -*-
# vim:syntax=make:
 
include $(MM_MAKEFILES_DIR)/SharedDefines.mk

all: install_dirs
	make -C mpc/dsp_printf
 
install uninstall: install_dirs
	make -C mpc/dsp_printf $@

clean realclean:
	make -C mpc/dsp_printf $@

DIRS_TO_INSTALL := mpc/api

TO_INSTALL := $(addprefix $(INSTALL_NMF_COMP_SRC_COMMON_DIR)/debug/, $(DIRS_TO_INSTALL))

install_dirs: $(TO_INSTALL)

$(INSTALL_NMF_COMP_SRC_COMMON_DIR)/debug/%: %
	mkdir -p $(INSTALL_NMF_COMP_SRC_COMMON_DIR)/debug
	mkdir -p $(INSTALL_NMF_COMP_SRC_COMMON_DIR)/debug/mpc
	if [ -d $< ] ; then $(MMINSTALL) -d $< $@ ; else $(MMINSTALL) $< $@ ; fi

