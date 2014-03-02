#
# Copyright (C) ST-Ericsson SA 2011. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

include $(MM_MAKEFILES_DIR)/SharedNmfConfig.mk 

## Sources required when executing on simulator
SRCPATH  += $(NMF_MPCEE_SRC)

## FIXME: breaking backward compatibility of MMDSP simulator execution ??
#SRCPATH  += $(MMSHARED)
#SHAREPATH = $(NMF)/$(NMF_ENV)/common
