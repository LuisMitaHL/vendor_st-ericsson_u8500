# -*- Mode: Makefile -*-
# vim:syntax=make:

include $(MM_MAKEFILES_DIR)/SharedMMDSPRules.mk
#include $(MM_MAKEFILES_DIR)/VideoDoc.mk

#install of idt files
$(INSTALL_IDT_DIR)/%.idt:
ifeq ($(UNAME),Linux)
	$(VERCHAR)if test ! -e  $@ ; then $(GEN_LN) $(PWD)/$(IDT_DIR)/$*.idt $@; fi 
else
	$(VERCHAR)$(CP) $< $@ 
endif
install-idt: $(addprefix $(INSTALL_IDT_DIR)/,$(INSTALL_IDT))


