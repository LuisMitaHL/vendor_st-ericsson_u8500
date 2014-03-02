
include ../config/config.mk

################################################################################
#                                   LBSCLIENT                                  #
################################################################################

LBSCLIENT_MOD := lbsclient

LBSCLIENT_DIR := srce
LOCAL_EN_AGPS_EE = true
ifeq ($(ENABLE_UP),false)
ifeq ($(ENABLE_SBEE),false)
LOCAL_EN_AGPS_EE = false
endif
endif

ifeq ($(LOCAL_EN_AGPS_EE),false)
LBSCLIENT_SRC := clientutils.c gpsclient.c ../../lbsd/agpscodec/srce/agpscodec.c
else

LBSCLIENT_SRC := clientutils.c gpsclient.c sbeeclient.c HttpHandler.c BearerHandler.c ../../lbsd/agpscodec/srce/agpscodec.c
endif

ifeq ($(LBS_UNIT_TEST_FTR),true)
LOCAL_ENABLE_TLS_STUB = true
else
LOCAL_ENABLE_TLS_STUB = false
endif
ifneq ($(ENABLE_UP),false)
 
ifeq ($(LOCAL_ENABLE_TLS_STUB),true)
LBSCLIENT_SRC += TlsHandler_stub.c
LOCAL_CFLAGS += -DAGPS_UNIT_TEST_FTR
else
LBSCLIENT_SRC += TlsHandler.c
endif
endif

#Currently this falg is not propagated currectly from Hats.
ifeq ($(LOCAL_SET_HATS_BUILD),true)
LBSCLIENT_FLAGS += -DDEBUG_LOG_LEV2 -DARM_LINUX_AGPS_FTR
endif

ifeq ($(ENABLE_UP),false)
LBSCLIENT_INCL := -Iincl -I../lbsd/include -I../lbsd/agpscodec/incl -I../lbsd/libagpsosa/incl -I../include
else
 
LBSCLIENT_INCL := -Iincl -I../lbsd/include -I../lbsd/agpscodec/incl -I../lbsd/libagpsosa/incl -I../include -Iexternal/openssl/include -Isystem/core/include
endif

LBSCLIENT_OBJ := obj
ifeq ($(LOCAL_EN_AGPS_EE),false)
LBSCLIENT_FLAGS := -UAGPS_INJECT_MOBILE_INFO_TEST -UAGPS_AUTO_ACCECPT_NOTIFICATION -UAGPS_INJECT_MOBILE_INFO_TEST \
	-DAGPS_ANDR_ECLAIR_FTR -DAGPS_PID0001_FTR
else

LBSCLIENT_FLAGS += -UAGPS_INJECT_MOBILE_INFO_TEST -UAGPS_AUTO_ACCECPT_NOTIFICATION -DAGPS_INJECT_MOBILE_INFO_TEST \
	-DAGPS_ANDR_ECLAIR_FTR -DAGPS_PID0001_FTR -UAGPS_SBEE_FTR
endif

#LBSCLIENT_FLAGS += -DGPSCLIENT_FORCE_IMSI

LIB := LBSCLIENT

include ../config/build.mk
