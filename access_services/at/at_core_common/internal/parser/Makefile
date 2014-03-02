CC := gcc
SIM :=   $(TOP)/vendor/st-ericsson/access_services/sim/sim
COMMON_FUNCTIONALITY := $(TOP)/vendor/st-ericsson/access_services/common/common_functionality
CFLAGS   := -g -O0  -ftest-coverage -fprofile-arcs
INCDIRS  := -I./include -I../../include -I../../config/include -I../selector/include -I../main/include -I../anchor/include -I../executor/include -I../selector/include -I$(SIM)/include -I$(SIM)/cat/libcat \
        -I$(COMMON_FUNCTIONALITY)/include
LDFLAGS  := -lc
LDLIBS   :=

SRC := src/atc_hand_me_control.c src/atc_hand_at_control.c src/atc_parser.c src/c_type.c src/atc_command_table.c src/atc_exe_glue.c\
       src/atc_hand_audio.c src/atc_hand_flags.c src/atc_hand_net.c src/atc_hand_sms.c src/atc_hand_clock.c\
       src/atc_hand_security.c src/atc_hand_sim.c src/atc_hand_call.c src/atc_parser_util.c src/atc_hand_debug.c\
       src/atc_hand_ss.c src/atc_hand_ps.c src/atc_string.c\
       src/atc_hand_sat.c\
       src/atc_hand_storage.c


HDR := include/atc_command_list.h include/atc_common.h include/atc_handlers.h include/c_type.h\
       include/atc_command_table.h include/atc_debug.h include/atc_parser.h

ifeq ($(ATC_ENABLE_FEATURE_ATC_CUSTOMER_EXTENSIONS),true)
include $(TOP)/vendor/st-ericsson/access_services/at/at_core_extension/internal/parser/inc_Makefile
endif

CFLAGS += $(INCDIRS) -DENABLE_READABILITY -DWHITE_BOX_TEST -DENABLE_FTD
TEST_HDR :=
OBJ := $(SRC:.c=.o)
COV_STUFF := $(SRC:.c=.gcda) $(SRC:.c=.gcno)

.PHONY: all clean test

all: test

test: libparser.a

libparser.a: $(OBJ) $(HDR)
	rm -f libparser.a ; \
	$(AR) crv libparser.a $(OBJ)

clean:
	rm -f $(OBJ) $(TEST_OBJ) white_box_test libparser.a $(COV_STUFF)
