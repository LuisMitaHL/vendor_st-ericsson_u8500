# -*- Mode: Makefile -*-
# vim:syntax=make:

# automatically insert the paths to the nmf host components to the include files
ifeq ($(findstring x86,$(MYMMBOARD)),x86) 
MYCOMPATPLAT  := $(MYMMBOARD)
else
MYCOMPATPLAT  := $(MMPROCESSOR)-$(MYMMSYSTEM)-$(MMCOMPILER)-$(MYMMBOARD) \
	$(MMPROCESSOR)-x-$(MMCOMPILER)-x \
        $(MMPROCESSOR)-$(MYMMSYSTEM)-$(MMCOMPILER)-x \
        $(MMPROCESSOR)-x-$(MMCOMPILER)-$(MYMMBOARD)
endif

HOST_NMF_PREFIX = $(foreach platform, $(MYCOMPATPLAT), $(addprefix -I$(MM_AUDIO_NMF_REPO_DIR),$(platform)/))

HOST_NMF_DIRS :=  $(foreach cpt, $(REQUIRED_HOST_NMF_COMPONENTS), $(addsuffix $(subst .,_,$(cpt))/src, $(HOST_NMF_PREFIX)/))

CPPFLAGS += $(HOST_NMF_DIRS)

print:
	@echo "$(COMPATPLAT)"
	@echo "$(COMPAT_PLATFORMS)"
	@echo "$(HOST_NMF_PREFIX)"
	@echo "Dirs: $(HOST_NMF_DIRS)"

include $(MM_MAKEFILES_DIR)/SharedRules.mk
