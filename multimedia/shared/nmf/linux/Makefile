#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

ifeq ($(strip $(PLATFORM)),)
  $(error Specify which platform to use. Supported platforms are: $(shell grep property component/component.xml | cut -d\" -f 4))
endif

include $(MM_MAKEFILES_DIR)/SharedConfig.mk

all: api install_headers x_all install

ifeq ($(findstring x86,$(PLATFORM)),x86)
  install: install_headers x_all x_install
else
  install: install_headers x_install
endif

install_headers:
	$(MMINSTALL) -d api $(INSTALL_HEADER_DIR)
ifeq ($(findstring x86,$(PLATFORM)),)
  ifeq ($(findstring 5500,$(PLATFORM)),)
	$(MMINSTALL) src/lib/CMUserProxy/dma_proxy.h $(INSTALL_HEADER_DIR)/cm/proxy/api/dma_proxy.h
  endif
endif

TOBECLEAN=$(foreach file, $(shell if [ -d api ] ; then find api/* -depth; fi),$(patsubst api/%,$(INSTALL_HEADER_DIR)/%, $(file)))
uninstall: x_uninstall
#	rm -rf $(addprefix $(INSTALL_HEADER_DIR)/,$(shell cd api ; find . -depth))
ifneq ($(TOBECLEAN),)
	rm -rf $(TOBECLEAN)
endif

clean: x_clean

x_%:
	$(MAKE) -C src $*

## If calling anything else than install and uninstall
## api will copy headers and sources from the 
## osintegration (nmf & los) packages only if existing in tested directories
## The non 'src' packages already contains those sources

include Make.config

$(info NMF_CORE_ROOT=$(NMF_CORE_ROOT))
$(info LOS_ROOT=$(LOS_ROOT))

ifneq ($(NMF_CORE_ROOT),)
  include $(NMF_CORE_ROOT)/linux/exports.mk

  ifeq ($(findstring x86,$(PLATFORM)),)
    ifeq ($(findstring 5500,$(PLATFORM)),)
      include src/driver/Make.config
      include $(NMF_CORE_ROOT)/linux/cm_proxy.mk
      ## Needed files for CM driver are the same for all platforms, just take the u8500_v2 ones
      include $(NMF_CORE_ROOT)/linux/cm_engine_$(MMBOARD).mk
      INSTALLED_PROXYSRC = $(addprefix $(NMF_CORE_ROOT), $(firstword $(CMPROXYSRC) $(CMPROXYCPPSRC)))
      INSTALLED_CMENGINESRC = $(addprefix $(NMF_CORE_ROOT), $(firstword $(CMENGINESRC) $(CMENGINESRC_COPY_NO_BUILD)))

      $(INSTALLED_PROXYSRC):
        ## Copy the sources needed by the CMproxy lib
	  tar -ch -C $(NMF_CORE_ROOT)/linux/src $(CMPROXYSRC) $(CMPROXYCPPSRC) \
          | tar -x -C src/lib/CMUserProxy

      $(INSTALLED_CMENGINESRC):
        ## Copy the sources needed by the CM driver
	  tar -ch -C $(NMF_CORE_ROOT)/linux/src $(CMENGINESRC) $(CMENGINESRC_COPY_NO_BUILD) \
          | tar -x -C src/driver

    endif
  endif
endif

api: $(INSTALLED_PROXYSRC) $(INSTALLED_CMENGINESRC)
  ifneq ($(LOS_ROOT),)
      ## Copy the LOS external headers, LOS sources, iLOS API
      ## if not exsiting to then install them
	tar -ch -C $(LOS_ROOT) api/los api/test | tar -x -C .
      ## Copy the LOS sources
	tar -ch -C $(LOS_ROOT)/src $(LOS_SOURCES) | tar -x -C src/lib/los
      ## Copy the iLOS API
      ##tar -ch -C $(LOS_ROOT)/api ilos | tar -x -C src/lib/los
  endif
  ifneq ($(NMF_CORE_ROOT),)
      ## Copy the CM external headers to then install them
	tar -ch -C $(NMF_CORE_ROOT)/linux/src $(CMAPI) $(EEAPI) $(CMDRVAPI) \
        | tar -x -C api
      ## Copy the internal headers needed by the CM driver and CM user proxy lib
	tar -ch -C $(NMF_CORE_ROOT)/linux/src \
          `cd $(NMF_CORE_ROOT)/linux/src ; find . -path "./cm/proxy" -prune -a -type f -o -path "./ee" -prune -a -type f -o -path "./cm/os" -prune -a -type f -o -name "*.h" -o -name "*.idt"` \
        | tar -x -C src/driver
	tar -ch -C $(NMF_CORE_ROOT)/linux/src ee/api/panic.idt ee/api/trace.idt \
        | tar -x -C src/driver
	tar -ch -C $(NMF_CORE_ROOT)/linux/src \
          `cd $(NMF_CORE_ROOT)/linux/src ; find ./cm/proxy -name "*.h" -o -name "*.hpp" ; find ./cm/os -name "*.h" -o -name "*.hpp"` \
        | tar -x -C src/lib/CMUserProxy
      ## Copy the hostEE sources
	tar -ch -C $(NMF_CORE_ROOT)/linux/src ee \
        | tar -x -C src/lib/hostee
  endif

ifneq ($(NMF_CORE_ROOT),)
  LOS_SOURCES_TO_CLEAN=api $(addprefix src/lib/los/,$(LOS_SOURCES)) src/lib/los/ilos
  CMPROXYSRC_TO_CLEAN=$(addprefix src/lib/CMUserProxy/,$(CMPROXYSRC) $(CMPROXYCPPSRC))
  CMENGINESRC_TO_CLEAN=$(addprefix src/driver/,$(CMENGINESRC) $(CMENGINESRC_COPY_NO_BUILD))
  CMENGINEHDR_TO_CLEAN=$(addprefix src/driver/,$(shell cd $(NMF_CORE_ROOT)/linux/src ; find . -name "*.h" -o -name "*.idt" -o -name "*.hpp"))
  EESRCC_TO_CLEAN=src/lib/hostee/ee
endif

realclean:
	$(MAKE) -C src $@
	rm -rf $(LOS_SOURCES_TO_CLEAN) $(CMPROXYSRC_TO_CLEAN) $(CMENGINESRC_TO_CLEAN) $(CMENGINEHDR_TO_CLEAN) $(EESRCC_TO_CLEAN)
	rm -rf 
	rm -f *~ doc/*~ src/*~ src/common/*~ component/*~
