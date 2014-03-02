# Don't make changes (even temporary) unless you intend to make them new
# default values. Instead copy this file into test_config.mk and make changes
# in that file instead.

#---------------------------------------------------------
# Find the parent directory of this file.
# Requires make version 3.80 or later.

# Get the pathname of this file
this := $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))
# Extract the directory part of that path.
ifeq (/,$(findstring /,$(this)))
# Remove the last slash and everything after it.
parentDir := $(shell x=$(this) && echo $${x%/*})
fileName := $(shell x=$(this) && echo $${x\#\#*/})
else
parentDir := .
fileName := $(this)
endif

include $(parentDir)/static_config.mk

# Definition of HW unique Chip ID, which ISSW must match against
#ISSW_PUBLIC_ID:=6116AE2F0A69C11B1C654A6C5343346EEE7781C3
#ISSW_PUBLIC_ID+=6116AE2F0A69C11B1C654A6C5343346EEE7781C4
#ISSW_PUBLIC_ID+=6116AE2F0A69C11B1C654A6C5343346EEE7781C5

# Enable this flag to use local signing.
BUILD_USING_LOCAL_SIGNING=1

# What subdirs to skip
#SKIP_BUILD_GP_TAS=1
#SKIP_BUILD_TA=1
SKIP_BUILD_7400A0=1
SKIP_BUILD_7400B0=1
#SKIP_BUILD_9540=1
#SKIP_BUILD_8540APE=1

ISSW_COPY_SIPC_KEY=1
#ISSW_USE_STATIC_SIPC_KEY=1
ISSW_ACCEPT_UNSIGNED_BINARIES=1
#ISSW_ENABLE_PRINTF=1
#ISSW_DISABLE_PRINT_A_=1
#ISSW_DISABLE_PRINT_B_=1
#ISSW_PRINT_C_=1
#ISSW_ENABLE_ASSERT=1
#ISSW_PAGER_DEBUG_PRINT=1
#ISSW_DEBUG=1
#ISSW_BOOT_DEBUG=1
#ISSW_WAIT_DEBUG=1
#ISSW_WAIT_DELAY=5
#ISSW_MODEM_OPEN_FULL=1
#ISSW_BUILD_VERBOSE=1
#ISSW_USE_MDBG=1
#ISSW_MEASURE_TIME=1
#ISSW_TEST_PATCH_CHECK_STACK_DEPTH=1
ISSW_WARNS=3
#ISSW_NOWERROR=1
#ISSW_M7400_CHASSIS=1
#ISSW_PAGE_TO_ESRAM=1
#ISSW_M7400_USB_FIX=1
# Only enable these when testing on other hardware than intended !!!
#ISSW_ENABLE_GENERIC_TOC_TOKEN=1
#ISSW_ENABLE_ENG_MODE=1
#ISSW_ENABLE_ARB=1
#ISSW_ENABLE_STE_DNT_CERT=1
#ISSW_IMAD_ENABLED=1

# This flag is only useful for targets that use ARB
#ISSW_TEST_ARB=1
#ISSW_TEST_STE_DNT_CERT=1
#CPPFLAGS_tee_rpmb.c += -DENABLE_RPMB_DATA_DUMP
#CPPFLAGS_tee_rpmb.c += -DPRINT_C_

#XLOADER_BUILD=1

#TA_ENABLE_PRINTF=1
#TA_DISABLE_PRINT_A_=1
#TA_DISABLE_PRINT_B_=1
#TA_PRINT_C_=1
#TA_DEBUG=1
#TA_WAIT_DEBUG=1
#TA_BUILD_VERBOSE=1
TA_WARNS=3
#TA_NOWERROR=1

#USER_TA_DEBUG=1
#USER_TA_DISABLE_PRINT_A_=1
#USER_TA_DISABLE_PRINT_B_=1
#USER_TA_PRINT_C_=1
#USER_TA_WAIT_DEBUG=1
#USER_TA_BUILD_VERBOSE=1
USER_TA_WARNS=3
#USER_TA_NOWERROR=1

XTEST_DEBUG=1
XTEST_ENABLE_PRINTF=1
#XTEST_WAIT_DEBUG=1
#XTEST_BUILD_VERBOSE=1
XTEST_WARNS=3
#XTEST_NOWERROR=1
#XTEST_BASSAPP_DEBUG=1


#COPS_TA_USE_STATIC_RSA_CHALLENGE=1

SMCL_TA_ACCEPT_UNSIGNED_BINARIES=1
SMCL_TA_COPY_SIPC_KEY=1

ifeq ($(fileName),config.mk)
-include $(parentDir)/test_config.mk
endif
