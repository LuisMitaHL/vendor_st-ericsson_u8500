# mop500/href:
#
#	Valid values for TEXT_BASE are:
#
#       Standard configuration - all models
#       0x0560_0000	run from SDRAM
#


sinclude $(OBJTREE)/board/$(BOARDDIR)/config.tmp

ifndef TEXT_BASE
TEXT_BASE = 0x05608000
endif

PLATFORM_CPPFLAGS += -DTEXT_BASE=$(TEXT_BASE)

# Use board specific linker script
LDSCRIPT := $(SRCTREE)/board/st-ericsson/u5500/u-boot.lds
