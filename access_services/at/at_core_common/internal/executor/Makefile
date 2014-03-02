CC := gcc
CFLAGS := -g -O0 -ftest-coverage -fprofile-arcs -D_GNU_SOURCE -DENABLE_FTD

AUDIO := $(TOP)/vendor/st-ericsson/multimedia/audio/adm
CN  := $(TOP)/vendor/st-ericsson/access_services/call_network/call_network
SMS := $(TOP)/vendor/st-ericsson/access_services/sms/sms
SIM := $(TOP)/vendor/st-ericsson/access_services/sim/sim
COPS := $(TOP)/vendor/st-ericsson/processing/security_framework/cops
CSPSA := $(TOP)/vendor/st-ericsson/storage/parameter_storage/cspsa
COMMON_FUNCTIONALITY := $(TOP)/vendor/st-ericsson/access_services/common/common_functionality

CONNECTIVITY_PATH := $(TOP)/vendor/st-ericsson/connectivity

INCDIRS := -Itest -Iinclude -Iaudio/include -Icops/include -Istorage/include -Ipscc/include -Isms/include -Isim/include -I../selector/include  -I../main/include -I../parser/include -I../predial/include\
           -I../anchor/include\
           -I../../config/include\
           -I$(AUDIO)/include\
           -I$(SMS)/include -I$(SMS)/internal/porting/include\
           -I$(SIM)/include -I$(SIM)/cat/libcat\
           -I$(COPS)/cops-api/include\
           -I$(CONNECTIVITY_PATH)/common/libstecom -I$(CONNECTIVITY_PATH)/psdata/psdata/libmpl -I$(CONNECTIVITY_PATH)/psdata/psdata_stepscc/libpscc -I$(CONNECTIVITY_PATH)/psdata/psdata_sterc/libsterc\
           -I$(CSPSA) \
           -I$(COMMON_FUNCTIONALITY)/include

CN_INC_DIR  :=	$(INCDIRS) -Icn/include -I$(CN)/libcn/include

COMPILEFLAGS := -DEXE_USE_AUDIO_SERVICE -DEXE_USE_COPS_SERVICE -DEXE_USE_SMS_SERVICE -DEXE_USE_SIM_SERVICE -DEXE_USE_PSCC_SERVICE -DEXE_USE_CSPSA_SERVICE -DWHITE_BOX_TEST

LDFLAGS  := -lc
LDLIBS   := -lgcov

SRC := src/exe.c src/exe_request_record.c src/exe_requests.c src/exe_util.c src/exe_last_fail_cause.c\
       audio/src/exe_audio_requests.c \
       cops/src/exe_cops_client.c cops/src/exe_cops_requests.c\
       sms/src/exe_sms_requests.c sms/src/exe_sms_requests_cbs.c sms/src/exe_sms_client.c sms/src/exe_sms_utils.c\
       sim/src/exe_sim_requests.c sim/src/exe_sim_client.c\
       pscc/src/exe_pscc_client.c pscc/src/exe_pscc_iadb.c pscc/src/exe_pscc_requests.c\
       pscc/src/exe_pscc_requests_data_call_list.c pscc/src/exe_pscc_requests_deactivate_data_call.c\
       pscc/src/exe_pscc_requests_modify_data_call.c pscc/src/exe_pscc_events.c\
       pscc/src/exe_pscc_requests_send_data_uplink_call.c pscc/src/exe_pscc_requests_nap.c\
       pscc/src/exe_pscc_requests_dun.c\
       pscc/src/exe_pscc_requests_setup_data_call.c pscc/src/exe_pscc_util.c\
       storage/src/exe_cspsa_requests.c

CN_SRC  = $(SRC) cn/src/exe_cn_client.c cn/src/exe_cn_requests.c cn/src/exe_cn_events.c cn/src/exe_cn_utils.c
TEST_SRC := test/exe_test.c test/exe_test_mal.c test/exe_test_stub_mal.c test/exe_test_stub_sms.c test/exe_test_sms.c test/exe_test_sim.c

ifeq ($(ATC_ENABLE_FEATURE_ATC_CUSTOMER_EXTENSIONS),true)
include $(TOP)/vendor/st-ericsson/access_services/at/at_core_extension/internal/executor/inc_Makefile
endif

CN_OBJ := $(CN_SRC:.c=.o)

COV_STUFF := $(CN_SRC:.c=.gcda) $(CN_SRC:.c=.gcno)

.PHONY: all clean test_pld test

all: test

test: CPPFLAGS := $(CN_INC_DIR) $(COMPILEFLAGS) -DEXE_USE_CN_SERVICE
test: $(CN_OBJ)
	@echo "------------------- Exe for CallNetwork"
	@echo $(CPPFLAGS)
	@echo "------------------- "
	rm -f libexe.a ; \
	$(AR) crv libexe.a $(CN_OBJ)

clean:
	rm -f $(CN_OBJ) $(TEST_OBJ) white_box_test libexe.a $(COV_STUFF)
