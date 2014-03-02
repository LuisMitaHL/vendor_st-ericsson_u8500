include ../config/config.mk

################################################################################
#                               LBS STELP TEST                                 #
################################################################################

LBSSTELPTEST_MOD := lbsstelptest

LBSSTELPTEST_DIR := srce

LBSSTELPTEST_SRC := main.c test_naf.c test_production.c
ifneq ($(UBUNTU_X86),true)
ifneq ($(LBS_UBUNTU),true)
ifeq ($(LBS_UNIT_TEST_FTR),true)
LOCAL_ENABLE_TLS_STUB = true
else
LOCAL_ENABLE_TLS_STUB = false
endif

ifeq ($(LOCAL_ENABLE_TLS_STUB),true)
LBSSTELPTEST_SRC += test_supl.c
LBSSTELPTEST_FLAGS += -DAGPS_UNIT_TEST_FTR
endif

endif
endif

ifeq ($(LBS_UBUNTU),true)
LBSSTELPTEST_INCL := -Iincl -I../liblbstestengine/incl -I../lbsclient/incl -I../include
else
ifeq ($(UBUNTU_X86),true)
LBSSTELPTEST_INCL := -Iincl -I../liblbstestengine/incl -I../lbsclient/incl -I../include -I../../lbs_external/openssl/include
else
LBSSTELPTEST_INCL := -Iincl -I../liblbstestengine/incl -I../lbsclient/incl -I../incl
endif
endif

LBSSTELPTEST_OBJ := obj

LBSSTELPTEST_FLAGS += -DAGPS_ANDR_ECLAIR_FTR
ifeq ($(LBS_UBUNTU),true)

MOD := LBSSTELPTEST

PROG := lbsstelp

PROGOUT := ..

LBS_LIB_PATH += -L../liblbstestengine/lib/$(ARCH) -L../lbsclient/lib/$(ARCH)

LDFLAGS := -lliblbstestengine -llbsclient -lm

else
ifeq ($(UBUNTU_X86),true)

MOD := LBSSTELPTEST
PROG := lbsstelp
PROGOUT := ..
LBS_LIB_PATH += -L../liblbstestengine/lib/$(ARCH) -L../lbsclient/lib/$(ARCH)
LDFLAGS := -lliblbstestengine -llbsclient -lm
else
MODULE := LBSSTELPTEST
endif
endif


include ../config/build.mk
