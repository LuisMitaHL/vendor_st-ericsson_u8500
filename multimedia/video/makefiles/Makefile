# -*- Mode: Makefile -*-
# vim:syntax=make:

include $(MM_MAKEFILES_DIR)/SharedDefines.mk

MK_FILES := $(addprefix $(INSTALL_MAKEFILES_DIR)/, $(filter-out BuildSpec.mk,$(wildcard *.mk)))

create_links:
ifeq ($(MAKEFILES_BACKWARD_COMPATIBLE),yes)
	@echo "Creating links for backward compatibility"
	ln -fs VideoAll.mk           Make.all
	ln -fs VideoConfig.mk        Make.config
	ln -fs VideoNmfConfig.mk     Make.nmf.config
	ln -fs VideoNmfRules.mk      Make.nmf.rules
	ln -fs VideoStdRules.mk      Make.std.rules
	ln -fs VideoStdConfig.mk     Make.std.config
	ln -fs VideoSymbianRules.mk  Make.symbian.rules
	ln -fs VideoDoc.mk           Make.doc
	ln -fs VideoMakefileSetup.csh     vsourceme
	$(RM) $(MMROOT)/video/idt
	ln -fs $(MMBUILDOUT)/$(NMF_COMP_SRC_COMMON_DIR) $(MMROOT)/video/idt
endif

$(INSTALL_MAKEFILES_DIR)/%: %
	@echo "  MMINSTALL $<"
	$(MMINSTALL) $< $@

## Do not clean the links otherwise backward compatibility is broken
all install clean realclean test: $(MK_FILES) create_links

uninstall:
ifeq ($(MAKEFILES_BACKWARD_COMPATIBLE),yes)
	@echo "Unlinks backward compatibilities"
	$(RM) Make.all
	$(RM) Make.config
	$(RM) Make.nmf.config
	$(RM) Make.nmf.rules
	$(RM) Make.std.rules
	$(RM) Make.std.config
	$(RM) Make.symbian.rules
	$(RM) Make.doc
	$(RM) vsourceme
	$(RM) $(MMROOT)/video/idt
endif
	$(RM) $(MK_FILES)
