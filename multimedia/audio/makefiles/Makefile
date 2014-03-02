#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

include $(MM_MAKEFILES_DIR)/SharedDefines.mk

MK_FILES := $(addprefix $(INSTALL_MAKEFILES_DIR)/, $(filter-out BuildSpec.mk,$(wildcard *.mk)))

all install : $(MK_FILES)
ifeq ($(MAKEFILES_BACKWARD_COMPATIBLE),yes)
	@echo "Creating links for backward compatibility"
	ln -fs AudioAlgoConfig.mk     Make.algo.config
	ln -fs AudioAlgoAll.mk        Make.algo.all
	ln -fs AudioAll.mk            Make.all
	ln -fs AudioConfig.mk         Make.config
	ln -fs AudioDispatch.mk       Make.dispatch
	ln -fs AudioFlags.mk          Make.flags
	ln -fs AudioMmdspConfig.mk    Make.mmdsp.config
	ln -fs AudioNmfArch.mk        Make.nmf.arch
	ln -fs AudioNmfConfig.mk      Make.nmf.config
	ln -fs AudioTest.mk           Make.test
	ln -fs AudioRules.mk          Make.rules
	ln -fs AudioUid.mk            Make.uid
	ln -fs AudioMakefileSetup.csh sourceme
	ln -fs AudioMakefileSetup.sh  sourceme.sh
endif

$(INSTALL_MAKEFILES_DIR)/%: %
	$(MMINSTALL) $< $@

## Do not clean the links otherwise backward compatibility is broken
clean realclean :

uninstall :
ifeq ($(MAKEFILES_BACKWARD_COMPATIBLE),yes)
	@echo "Unlinks backward compatibilities"
	$(RM) Make.algo.config
	$(RM) Make.algo.all
	$(RM) Make.all
	$(RM) Make.config
	$(RM) Make.dispatch
	$(RM) Make.flags
	$(RM) Make.mmdsp.config
	$(RM) Make.nmf.arch
	$(RM) Make.nmf.config
	$(RM) Make.test
	$(RM) Make.rules
	$(RM) Make.uid
	$(RM) sourceme
	$(RM) sourceme.sh
endif
	$(RM) $(MK_FILES)

test cleantest:


