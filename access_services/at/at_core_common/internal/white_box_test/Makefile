CC := gcc
CFLAGS := -g -O0 -ftest-coverage -fprofile-arcs

ACCESS_SERVICES_PATH := $(TOP)/vendor/st-ericsson/access_services

AUDIO := $(TOP)/vendor/st-ericsson/multimedia/audio/adm
CN  := $(ACCESS_SERVICES_PATH)/call_network/call_network
SMS := $(ACCESS_SERVICES_PATH)/sms/sms
SIM := $(ACCESS_SERVICES_PATH)/sim/sim
EXTERNAL := $(TOP)/external
EXTENSION := $(ACCESS_SERVICES_PATH)/at/at_core_extension
CONNECTIVITY_PATH := $(TOP)/vendor/st-ericsson/connectivity
COPS := $(TOP)/vendor/st-ericsson/processing/security_framework/cops
CSPSA := $(TOP)/vendor/st-ericsson/storage/parameter_storage/cspsa
COMMON_FUNCTIONALITY := $(TOP)/vendor/st-ericsson/access_services/common/common_functionality

INCDIRS := -I./include -I../../include $(EXTENSIONINCFLAGSS) -I../../config/include -I../parser/include -I../predial/include -I../selector/include -I../main/include -I../anchor/include \
		-I../executor/include -I../executor/audio/include -I../executor/pscc/include -I../executor/sms/include -I../executor/sim/include -I../executor/cops/include\
		-I$(AUDIO)/include\
		-I$(COPS)/cops-api/include\
		-I$(SMS)/include\
		-I$(SIM)/include\
		-I$(CONNECTIVITY_PATH)/common/libstecom -I$(CONNECTIVITY_PATH)/psdata/psdata/libmpl -I$(CONNECTIVITY_PATH)/psdata/psdata_stepscc/libpscc -I$(CONNECTIVITY_PATH)/psdata/psdata_sterc/libsterc \
		-I$(CSPSA) \
		-I$(COMMON_FUNCTIONALITY)/include

CN_INCDIRS  :=$(INCDIRS) -I../executor/cn/include -I$(CN)/libcn/include

COMPILEFLAGS := -DEXE_USE_AUDIO_SERVICE -DEXE_USE_COPS_SERVICE -DEXE_USE_SMS_SERVICE -DEXE_USE_SIM_SERVICE -DEXE_USE_PSCC_SERVICE -DEXE_USE_CSPSA_SERVICE -DWHITE_BOX_TEST

LDFLAGS  := -lc -lm -ldl
LDLIBS   := -lparser -lexe -lgcov -lpredial -lmain -ldl
LDDIRS   := -L../executor -L../parser -L../predial -L../main

SRC :=	src/whitebox.c src/wb_test.c src/wb_test_audio.c src/wb_test_net.c src/wb_test_pscc.c \
	src/wb_test_sim.c src/wb_test_sec.c src/wb_test_sms.c src/wb_test_ss.c src/wb_test_control.c\
	src/wb_stub_audio.c src/wb_stub_main.c src/wb_stub_selector.c src/wb_stub_cops.c \
	src/wb_stub_pscc.c src/wb_stub_sms.c src/wb_stub_sim.c src/wb_stub_exe_start.c \
	src/wb_test_sat.c src/wb_stub_librt.c\
	src/wb_stub_cspsa.c src/wb_test_storage.c \
	$(ACCESS_SERVICES_PATH)/common/common_functionality/log/src/util_log.c

CN_SRC  = $(SRC) src/wb_stub_cn.c src/wb_test_cn.c
HDR := include/whitebox.h include/wb_test.h

ifeq ($(ATC_ENABLE_FEATURE_ATC_CUSTOMER_EXTENSIONS),true)
include $(ACCESS_SERVICES_PATH)/at/at_core_extension/internal/white_box_test/inc_Makefile
endif


CN_OBJ := $(CN_SRC:.c=.o)
COV_STUFF := $(CN_SRC:.c=.gcda) $(CN_SRC:.c=.gcno)

.PHONY: all clean test test_ng debug_print

test: CPPFLAGS := $(CN_INCDIRS) $(COMPILEFLAGS) -DEXE_USE_CN_SERVICE
test: $(CN_OBJ) $(HDR)
	@echo "------------------- Whitebox for CallNetwork"
	@echo $(CPPFLAGS)
	@echo "------------------- "
	$(CC) -o white_box_test $(CN_OBJ) $(LDLIBS) $(LDDIRS) $(LDFLAGS)
	./white_box_test

debug_print:
			@ echo
			@ echo "Enabling all prints (bit field value 31) by specifying log level file in /tmp/telephony_log_levels"
			@cp telephony_log_levels /tmp/telephony_log_levels
			@ echo
clean:
	rm -f $(CN_OBJ) $(TEST_OBJ) white_box_test $(COV_STUFF)
	@rm -fv /tmp/telephony_log_levels
