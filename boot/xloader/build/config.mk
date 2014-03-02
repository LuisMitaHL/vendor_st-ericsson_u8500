# Don't make changes (even temporary) unless you intend to make them new
# default values. Instead copy this file into test_config.mk and make changes
# in that file instead.

# What subdirs to skip
SKIP_BUILD_TA=1
ifeq ($(STE_PLATFORM),u8500)
BUILD_8500B0=1
endif

ifeq (u9500,$(findstring u9500,$(STE_PLATFORM)))
BUILD_8500B0=1
endif

ifeq (u9540,$(findstring u9540,$(STE_PLATFORM)))
BUILD_9540A0=1
endif

ifeq (l8540,$(findstring l8540,$(STE_PLATFORM)))
BUILD_8540A0=1
endif

ifeq ($(STE_PLATFORM),snowball)
BUILD_SNOWBALL_A02=1
endif

ISSW_COPY_SIPC_KEY=1
ISSW_USE_STATIC_SIPC_KEY=1
ISSW_ACCEPT_UNSIGNED_BINARIES=1
#ISSW_ENABLE_PRINTF=1
#ISSW_PAGER_DEBUG_PRINT=1
#ISSW_DEBUG=1
#ISSW_BOOT_DEBUG=1
#ISSW_WAIT_DEBUG=1
#ISSW_BUILD_VERBOSE=1
#ISSW_USE_MDBG=1
ISSW_WARNS=0
#ISSW_NOWERROR=1

XLOADER_ACCEPT_UNSIGNED_BINARIES=1
#XLOADER_OLD_FORMAT=1
XLOADER_ENABLE_PRINTF=1
#XLOADER_DEBUG=1
#XLOADER_WAIT_DEBUG=1
#XLOADER_TEE_TEST=1
XLOADER_BUILD_VERBOSE=1
#XLOADER_WARNS=3
#XLOADER_NOWERROR=1
XLOADER_ENABLE_LOAD_TEE=1

#UART_OVER_ULPI or UART_OVER_USB, only 1 of these could be enabled.
#Default both are disabled. One of them to be enabled when micro
#USB-switch is used.
#XLOADER_UART_OVER_ULPI=1
#XLOADER_UART_OVER_USB=1

#Enable system reset by remove #, disable system reset by comment the line.
#ENABLE_SYSTEM_RESET=1


#TA_ENABLE_PRINTF=1
TA_DEBUG=1
#TA_WAIT_DEBUG=1
#TA_BUILD_VERBOSE=1
TA_WARNS=0
#TA_NOWERROR=1
#COPS_TA_USE_STATIC_RSA_CHALLENGE=1
#---------------------------------------------------------
# Find the parent directory of this file.
# Requires make version 3.80 or later.
# Get the pathname of this file
this := $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))
# Extract the directory part of that path.
ifeq (/,$(findstring /,$(this)))
# Remove the last slash and everything after it.
parentDir := $(shell x=$(this) && echo $${x%/*})
fileName := $(shell x=$(this) && echo $${x\#*/})
else
parentDir := .
fileName := $(this)
endif
ifeq ($(fileName),config.mk)
-include $(parentDir)/test_config.mk
endif
