#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

include $(MM_MAKEFILES_DIR)/SharedDefines.mk

RELPATH=$(PWD)/relpath

MK_FILES := $(addprefix $(INSTALL_MAKEFILES_DIR)/, $(wildcard *.mk))

.PHONY: all install uninstall clean realclean test create_links

all install clean realclean test: $(MK_FILES) create_links

uninstall :
ifeq ($(MAKEFILES_BACKWARD_COMPATIBLE),yes)
	$(RM_LINK) Make.check
	$(RM_LINK) Make.config
	$(RM_LINK) Make.dispatch
	$(RM_LINK) Make.mmdsp.config
	$(RM_LINK) Make.mmdsp.rules
	$(RM_LINK) Make.nmf.arch.config
	$(RM_LINK) Make.nmf.config
	$(RM_LINK) Make.nmf.rules
	$(RM_LINK) Make.rules
	$(RM_LINK) Make.symbian.rules
	$(RM_LINK) Make.tc
	$(RM_LINK) gen_rel_link
	$(RM_LINK) $(MMROOT)/shared/$(OST_DICO_DIR)
	for medium in shared audio video imaging linux ; do \
          if [ -d $(MMROOT)/$$medium ] ; then \
            $(RM_LINK) $(MMROOT)/$$medium/$(HEADER_DIR_NAME) ; \
            $(RM_LINK) $(MMROOT)/$$medium/$(LIB_DIR_NAME) ; \
            $(RM_LINK) $(MMROOT)/$$medium/$(BIN_DIR_NAME) ; \
            $(RM_LINK) $(MMROOT)/$$medium/$(NMF_REPO_DIR) ; \
            $(RM_LINK) $(MMROOT)/$$medium/$(ROOTFS_DIR_NAME) ; \
          fi ; \
        done
endif
	$(RM_FILE) $(MK_FILES)
	$(RM_DIRS) $(INSTALL_MAKEFILES_DIR)

$(INSTALL_MAKEFILES_DIR)/%: %
	@echo "  MMINSTALL $(REL_BUILD_INTERMEDIATES_DIR)/$<"
	$(VERCHAR)$(MMINSTALL) $< $@

create_links:
ifeq ($(MAKEFILES_BACKWARD_COMPATIBLE),yes)
	ln -fs SharedCheck.mk         Make.check
	ln -fs SharedConfig.mk        Make.config
	ln -fs SharedDispatch.mk      Make.dispatch
	ln -fs SharedMMDSPConfig.mk   Make.mmdsp.config
	ln -fs SharedMMDSPRules.mk    Make.mmdsp.rules
	ln -fs SharedNmfArchConfig.mk Make.nmf.arch.config
	ln -fs SharedNmfConfig.mk     Make.nmf.config
	ln -fs SharedNmfRules.mk      Make.nmf.rules
	ln -fs SharedRules.mk         Make.rules
	ln -fs SharedSymbianRules.mk  Make.symbian.rules
	ln -fs SharedTraceCompiler.mk Make.tc
	ln -fs $(MM_MAKEFILES_DIR)/mminstall gen_rel_link
	mkdir -p $(INSTALL_HEADER_BASEDIR)
	mkdir -p $(INSTALL_LIB_DIRNAME)
	mkdir -p $(INSTALL_BIN_DIRNAME)
	mkdir -p $(INSTALL_NMF_REPO_DIR)
	mkdir -p $(MMBUILDOUT)/$(ROOTFS_DIR_NAME)
	mkdir -p $(INSTALL_DICO_DIR)
	$(RM_LINK) $(MMROOT)/shared/$(OST_DICO_DIR)
	ln -fs `$(RELPATH) $(MMROOT)/shared/$(OST_DICO_DIR) $(INSTALL_DICO_DIR)` $(MMROOT)/shared/$(OST_DICO_DIR)
	for medium in shared audio video imaging linux ; do \
          if [ -d $(MMROOT)/$$medium ] ; then \
	    mkdir -p $(INSTALL_HEADER_BASEDIR)/$$medium ; \
            $(RM_LINK) $(MMROOT)/$$medium/$(HEADER_DIR_NAME) ; \
            ln -fs `$(RELPATH) $(MMROOT)/$$medium/$(HEADER_DIR_NAME) $(INSTALL_HEADER_BASEDIR)/$$medium` $(MMROOT)/$$medium/$(HEADER_DIR_NAME) ; \
            $(RM_LINK) $(MMROOT)/$$medium/$(LIB_DIR_NAME) ; \
            ln -fs `$(RELPATH) $(MMROOT)/$$medium/$(LIB_DIR_NAME) $(INSTALL_LIB_DIRNAME)` $(MMROOT)/$$medium/$(LIB_DIR_NAME) ; \
            $(RM_LINK) $(MMROOT)/$$medium/$(BIN_DIR_NAME) ; \
            ln -fs `$(RELPATH) $(MMROOT)/$$medium/$(BIN_DIR_NAME) $(INSTALL_BIN_DIRNAME)` $(MMROOT)/$$medium/$(BIN_DIR_NAME) ; \
            $(RM_LINK) $(MMROOT)/$$medium/$(NMF_REPO_DIR) ; \
            ln -fs `$(RELPATH) $(MMROOT)/$$medium/$(NMF_REPO_DIR) $(INSTALL_NMF_REPO_DIR)` $(MMROOT)/$$medium/$(NMF_REPO_DIR) ; \
            $(RM_LINK) $(MMROOT)/$$medium/$(ROOTFS_DIR_NAME) ; \
            ln -fs `$(RELPATH) $(MMROOT)/$$medium/$(ROOTFS_DIR_NAME) $(MMBUILDOUT)/$(ROOTFS_DIR_NAME)` $(MMROOT)/$$medium/$(ROOTFS_DIR_NAME) ; \
          fi ; \
        done
endif

