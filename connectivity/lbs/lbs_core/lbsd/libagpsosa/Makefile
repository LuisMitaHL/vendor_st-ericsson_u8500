
include ../../config/config.mk

################################################################################
#                               LIBAGPS OSA                                    #
################################################################################

LIBAGPSOSA_MOD := agpsosa

LIBAGPSOSA_DIR := srce

LIBAGPSOSA_SRC := agpsosa.c agpsfsm.c

LIBAGPSOSA_INCL := -Iincl

LIBAGPSOSA_OBJ := obj

LIBAGPSOSA_FLAGS := -DAGPS_ANDR_ECLAIR_FTR 

ifeq ($(UBUNTU_X86),true)
LIBAGPSOSA_FLAGS += -DGPS_OVER_UART -DHALGPS_DONOT_USE_RTC
else
LIBAGPSOSA_FLAGS += -DGPS_OVER_HCI
endif
LIB := LIBAGPSOSA

include ../../config/build.mk
