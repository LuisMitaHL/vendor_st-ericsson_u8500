######################### -*- Mode: Makefile-Gmake -*- ########################
## Copyright (C) 2010, ST-Ericsson
## 
## File name       : Makefile
## Description     : builds test
## 
## Author          : Johan Norberg <johan.xx.norberg@stericsson.com>
## 
###############################################################################

### DIRECTORY DEFINITIONS ###
ACCESS_SERVICES_PATH ?= $(TOP)/vendor/st-ericsson/access_services
MODULE_ROOT := ../..

### SIM ###
SIM_ROOT := $(ACCESS_SERVICES_PATH)/sim/sim

### BACKEND ###
# Set default backend to MAL since MFL is deprecated
CN_SET_FEATURE_BACKEND := MAL

ifeq ($(CN_SET_FEATURE_BACKEND),MAL)
MAL_ROOT := $(ACCESS_SERVICES_PATH)/modem_adaptations/mal/modem_lib
else
ifeq ($(CN_SET_FEATURE_BACKEND),MFL)
MFL_ROOT := $(ACCESS_SERVICES_PATH)/TODO
else
$(info **************************************)
$(info *)
$(info *      NO BACKEND SELECTED!!!)
$(info *)
$(info * $(MAKE) $(MAKECMDGOALS) CN_SET_FEATURE_BACKEND=MAL)
$(info *      or)
$(info * $(MAKE) $(MAKECMDGOALS) CN_SET_FEATURE_BACKEND=MFL)
$(info *)
$(info **************************************)
$(error stop)
endif
endif


### INCLUDE DIRS ###
INCDIRS		:=	-I $(MODULE_ROOT)/internal/test/include \
			-I $(MODULE_ROOT)/internal/cnserver/include \
			-I $(MODULE_ROOT)/internal/main/include \
			-I $(MODULE_ROOT)/libcn/include \
			-I $(ACCESS_SERVICES_PATH)/common/common_functionality/include \
			-I $(ACCESS_SERVICES_PATH)/modem_adaptations/modem-supervisor \
			-I $(SIM_ROOT)/include

ifeq ($(CN_SET_FEATURE_BACKEND),MAL)
INCDIRS	+=	-I $(MODULE_ROOT)/internal/mal/include \
			-I $(MAL_ROOT)/mal/mal_util/include \
			-I $(MAL_ROOT)/mal/libmalcs/include \
			-I $(MAL_ROOT)/mal/libmalgss/include \
			-I $(MAL_ROOT)/mal/libmalmce/include \
			-I $(MAL_ROOT)/mal/libmalmis/include \
			-I $(MAL_ROOT)/mal/libmalnet/include \
			-I $(MAL_ROOT)/mal/libshmnetlnk/include \
			-I $(MAL_ROOT)/mal/libmalrf/include \
			-I $(MAL_ROOT)/mal/libmalnvd/include \
                        -I $(MAL_ROOT)/mal/libmalmte/include \
			-I $(MAL_ROOT)/mal/libmalftd/include

endif

ifeq ($(CN_SET_FEATURE_BACKEND),MFL)
INCDIRS	+=	-I $(MODULE_ROOT)/internal/mfl/include
endif

### COMPILER FLAGS ###
CC		:=	gcc
CFLAGS		:=	-c -Wall -Wextra  -pthread -g -O0 -ftest-coverage -fprofile-arcs $(INCDIRS) -DENABLE_MODULE_TEST -DUSE_ICONV -DENABLE_RAT_BALANCING
LDFLAGS		:=	-lpthread -lgcov -lm -lrt

CFLAGS 		+= -DCN_DEBUG_ENABLED -DENABLE_FTD

ifeq ($(CN_SET_FEATURE_BACKEND),MAL)
CFLAGS		+=	-DUSE_MAL_BACKEND -DUSE_MAL_MCE -DUSE_MAL_GSS -DUSE_MAL_NET -DUSE_MAL_CS -DUSE_MAL_MIS -DUSE_MAL_NVD -DUSE_MAL_RF -DUSE_MAL_FTD
endif

ifeq ($(CN_SET_FEATURE_BACKEND),MFL)
CFLAGS		+=	-DUSE_MFL_BACKEND
endif

### HEADER FILES ###
HDR		:=	$(MODULE_ROOT)/internal/main/include/main.h \
			$(MODULE_ROOT)/internal/main/include/fdmon.h \
			$(MODULE_ROOT)/internal/main/include/sockserv.h \
			$(MODULE_ROOT)/internal/cnserver/include/cnsocket.h \
			$(MODULE_ROOT)/internal/cnserver/include/cnserver.h \
			$(MODULE_ROOT)/internal/cnserver/include/modem_client.h \
			$(MODULE_ROOT)/internal/cnserver/include/message_handler.h \
			$(MODULE_ROOT)/internal/cnserver/include/event_handling.h \
			$(MODULE_ROOT)/internal/cnserver/include/request_handling.h \
			$(MODULE_ROOT)/internal/cnserver/include/cn_utils.h \
			$(MODULE_ROOT)/internal/cnserver/include/cn_ss_command_handling.h \
			$(MODULE_ROOT)/internal/cnserver/include/cn_pdc_ecc_list_handling.h \
			$(MODULE_ROOT)/internal/cnserver/include/cn_event_trigger_level.h \
			$(MODULE_ROOT)/internal/cnserver/include/cn_internal_handlers.h \
			$(MODULE_ROOT)/internal/test/include/testcases.h \
			$(MODULE_ROOT)/internal/test/include/pdc_test_utils.h \
			$(MODULE_ROOT)/libcn/include/cn_message_types.h \
			$(MODULE_ROOT)/libcn/include/cn_log.h \
			$(MODULE_ROOT)/libcn/include/cn_data_types.h \
			$(MODULE_ROOT)/libcn/include/cn_general.h \
			$(MODULE_ROOT)/libcn/include/cn_client.h \
			$(MODULE_ROOT)/libcn/include/cn_macros.h \
			$(MODULE_ROOT)/internal/cnserver/include/cn_pdc.h \
			$(MODULE_ROOT)/internal/cnserver/include/cn_pdc_internal.h \
			$(MODULE_ROOT)/internal/cnserver/include/cn_battery_internal.h \
			$(ACCESS_SERVICES_PATH)/common/common_functionality/include/util_general.h \
			$(ACCESS_SERVICES_PATH)/common/common_functionality/include/util_log.h \
			$(ACCESS_SERVICES_PATH)/common/common_functionality/include/util_security.h

ifeq ($(CN_SET_FEATURE_BACKEND),MAL)
HDR		+=	$(MODULE_ROOT)/internal/mal/include/cn_mal_assertions.h
endif

ifeq ($(CN_SET_FEATURE_BACKEND),MFL)
HDR		+=	$(MODULE_ROOT)/internal/mfl/include/cn_mfl_assertions.h
endif

### SOURCE FILES ###
SRC		:=	$(MODULE_ROOT)/internal/test/src/module_test_main.c \
			$(MODULE_ROOT)/internal/test/src/ss_command_testcases.c \
			$(MODULE_ROOT)/internal/test/src/predial_check_testcases.c \
			$(MODULE_ROOT)/internal/test/src/pdc_msg_translation_testcases.c \
			$(MODULE_ROOT)/internal/test/src/pdc_sat_testcases.c \
			$(MODULE_ROOT)/internal/test/src/pdc_ecc_testcases.c \
			$(MODULE_ROOT)/internal/test/src/pdc_fdn_testcases.c \
			$(MODULE_ROOT)/internal/test/src/pdc_voice_call_testcases.c \
			$(MODULE_ROOT)/internal/test/src/pdc_ss_testcases.c \
			$(MODULE_ROOT)/internal/test/src/pdc_test_utils.c \
			$(MODULE_ROOT)/internal/test/src/plmn_testcases.c \
			$(MODULE_ROOT)/internal/test/src/etl_testcases.c \
			$(MODULE_ROOT)/internal/test/src/misc.c \
			$(MODULE_ROOT)/internal/cnserver/src/cn_ss_command_handling.c \
			$(MODULE_ROOT)/internal/cnserver/src/cn_pdc_ecc_list_handling.c \
			$(MODULE_ROOT)/internal/cnserver/src/cnserver.c \
			$(MODULE_ROOT)/internal/cnserver/src/message_handler.c \
			$(MODULE_ROOT)/internal/cnserver/src/cn_battery_main.c \
			$(MODULE_ROOT)/internal/cnserver/src/cn_battery_client.c \
			$(MODULE_ROOT)/internal/cnserver/src/cn_log_client.c \
			$(MODULE_ROOT)/internal/main/src/fdmon.c \
			$(MODULE_ROOT)/internal/main/src/main.c \
			$(MODULE_ROOT)/internal/main/src/sockserv.c \
			$(MODULE_ROOT)/libcn/src/cn_client.c \
			$(MODULE_ROOT)/libcn/src/cn_log.c \
			$(MODULE_ROOT)/internal/cnserver/src/cn_pdc_main.c \
			$(MODULE_ROOT)/internal/cnserver/src/sim_client.c \
			$(MODULE_ROOT)/internal/cnserver/src/cn_utils.c \
			$(MODULE_ROOT)/internal/cnserver/src/cn_pdc_msg_translation.c \
			$(MODULE_ROOT)/internal/cnserver/src/cn_pdc_ecc_check.c \
			$(MODULE_ROOT)/internal/cnserver/src/cn_pdc_fdn_check.c \
			$(MODULE_ROOT)/internal/cnserver/src/cn_pdc_sat_cc.c \
			$(MODULE_ROOT)/internal/cnserver/src/cn_pdc_utils.c \
			$(MODULE_ROOT)/internal/test/src/sim_stubs.c \
			$(MODULE_ROOT)/internal/test/src/msup_stub.c \
			$(MODULE_ROOT)/internal/cnserver/src/str_convert.c \
			$(MODULE_ROOT)/internal/cnserver/src/plmn_main.c \
			$(MODULE_ROOT)/internal/cnserver/src/plmn_list.c \
			$(MODULE_ROOT)/internal/cnserver/src/plmn_file.c \
			$(MODULE_ROOT)/internal/cnserver/src/plmn_sim.c \
			$(MODULE_ROOT)/internal/cnserver/src/cn_rat_control.c \
			$(MODULE_ROOT)/internal/cnserver/src/cn_message_strings.c \
			$(MODULE_ROOT)/internal/cnserver/src/cn_event_trigger_level.c \
			$(ACCESS_SERVICES_PATH)/common/common_functionality/log/src/util_log.c \
			$(ACCESS_SERVICES_PATH)/common/common_functionality/security/src/util_file.c \
			$(ACCESS_SERVICES_PATH)/common/common_functionality/security/src/util_user.c

ifeq ($(CN_SET_FEATURE_BACKEND),MAL)
SRC		+=	$(MODULE_ROOT)/internal/test/src/mal_testcases.c \
			$(MODULE_ROOT)/internal/test/src/mal_stubs.c \
			$(MODULE_ROOT)/internal/mal/src/request_handlers.c \
			$(MODULE_ROOT)/internal/mal/src/event_handlers.c \
			$(MODULE_ROOT)/internal/mal/src/mal_client.c \
			$(MODULE_ROOT)/internal/mal/src/utils.c \
			$(MODULE_ROOT)/internal/mal/src/dial_dtmf_handler.c
endif

ifeq ($(CN_SET_FEATURE_BACKEND),MFL)
SRC		+=	$(MODULE_ROOT)/internal/test/src/mfl_testcases.c \
			$(MODULE_ROOT)/internal/test/src/mfl_stubs.c \
			$(MODULE_ROOT)/internal/mfl/src/request_handlers.c \
			$(MODULE_ROOT)/internal/mfl/src/event_handlers.c \
			$(MODULE_ROOT)/internal/mfl/src/mfl_client.c
endif


### GCOV FILES ###
COV := $(SRC:.c=.gcda) $(SRC:.c=.gcno)

### OBJECT FILES ###
OBJ := $(SRC:.c=.o)

### BUILD TARGETS ###
all: main

main: $(OBJ) $(SRC) $(HDR)
	@mkdir -p /tmp/socket
	$(CC) $(OBJ) $(LDFLAGS) -o test

cov: clean # Initial Code coverage setup, please improve!
	@for f in $(HDR) $(SRC); do cp -f $$f .; done
	@for f in *.c; do $(CC) -c $(CFLAGS) -g -O0 -ftest-coverage -fprofile-arcs $$f; done
	@$(CC) *.o $(LDFLAGS) -lgcov -o cov_test
	@./cov_test
	@rm -f module_test_main* *stubs* *testcases* cn_battery* # Remove files that shouldn't be measured
	@rm -f utils* # 
	@gcov *.gcda
	@lcov -d . -b . -c -o cov.info
	@genhtml -o coverage_results cov.info
	@rm -f *.c *.h *.o *.gcda *.gcno *.cov cov_test *.info
	@firefox $(MODULE_ROOT)/internal/test/coverage_results/internal/test/index.html &


### EXECUTION TARGETS ###
.PHONY: test
test: all
	@./test 2>&1 | tee test.txt
	@echo
	@echo "internal/test/test.txt created with the execution log"
	@echo

.PHONY: valgrind
valgrind: all
	@valgrind --leak-check=full --track-origins=yes --show-reachable=yes ./test 2>&1 | tee valgrind.txt && ./analyze_valgrind.py
	@echo
	@echo "internal/test/valgrind.txt' created with the execution log"
	@echo

.PHONY:
debug_print:
	@ echo
	@ echo "Enabling all prints (bit field value 15) by specifying log level file in /tmp/telephony_log_levels"
	@cp $(MODULE_ROOT)/internal/test/misc/telephony_log_levels /tmp/telephony_log_levels
	@ echo

clean:
	@rm -fv /tmp/enl
	@rm -fv $(OBJ) $(COV)
	@rm -rfv coverage_results
	@rm -fv *.txt
	@rm -fv /tmp/telephony_log_levels

