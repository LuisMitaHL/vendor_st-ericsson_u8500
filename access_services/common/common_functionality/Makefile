###########################################################
# Copyright (C) ST-Ericsson SA 2011. All rights reserved. #
# This code is ST-Ericsson proprietary and confidential.  #
# Any use of the code for whatever purpose is subject to  #
# specific written permission of ST-Ericsson SA.          #
###########################################################

TARGETS=cloc coverity

all:
	@echo "available targets: $(TARGETS)"
	@echo ""

cloc:
	@cloc *

coverity:
	@sh script/coverity.sh common_functionality_module_test
