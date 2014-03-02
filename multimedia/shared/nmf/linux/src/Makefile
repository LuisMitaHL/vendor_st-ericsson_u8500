# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.

default: userall kernelmodule

user%:
	$(MAKE) -C lib $*

BUILD_KERNEL_MODULE=true

ifeq ($(findstring x86,$(PLATFORM)),x86)
  BUILD_KERNEL_MODULE=false
endif
ifeq ($(findstring 5500,$(PLATFORM)),5500)
  BUILD_KERNEL_MODULE=false
endif
ifeq ($(SW_VARIANT),android)
  # In case of SI source tree, NMF CM is in kernel source tree
  BUILD_KERNEL_MODULE=false
endif

kernel%:
ifeq ($(BUILD_KERNEL_MODULE),true)
	$(MAKE) -C driver $*
else
	@echo "No NMF CM driver to build for this platform"
endif


%:
	${MAKE} user$@ kernel$@
