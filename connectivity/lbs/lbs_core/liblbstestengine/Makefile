
include ../config/config.mk

################################################################################
#                               LIB LBS TEST ENGINE Make file                  #
################################################################################

LIBLBSTESTENGINE_MOD := liblbstestengine

LIBLBSTESTENGINE_DIR := srce

LIBLBSTESTENGINE_SRC := test_engine_main.c test_engine_process.c test_engine.c test_engine_utils.c


LIBLBSTESTENGINE_INCL := -Iincl

LIBLBSTESTENGINE_OBJ := obj

LIBLBSTESTENGINE_FLAGS := -DAGPS_ANDR_ECLAIR_FTR
ifeq ($(LBS_UBUNTU),true)

LIB := LIBLBSTESTENGINE

else
ifeq ($(UBUNTU_X86),true)

LIB := LIBLBSTESTENGINE
else
MODULE := LIBLBSTESTENGINE
endif
endif

include ../config/build.mk
