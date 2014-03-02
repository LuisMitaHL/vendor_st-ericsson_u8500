######################### -*- Mode: Makefile-Gmake -*- ########################
## Copyright (C) 2010, ST-Ericsson
## 
## File name       : Makefile
## Description     : builds libpc
## 
## Author          : Haiyuan Bu <haiyuan.bu@stericsson.com>
## 
###############################################################################

SMS_DIR := ../../../sms/sms

CN_DIR := ../../../call_network/call_network

COPS_DIR := ../../../../processing/security_framework/cops

CC       := gcc
INCDIRS  := -I../include -I../libsimcom -I../cat -I../simd -I../libapdu -I../uiccd/include \
    -I$(SMS_DIR)/include -I$(COPS_DIR)/cops-api/include -I$(CN_DIR)/libcn/include
CPPFLAGS := $(INCDIRS)
CFLAGS   := -g -Wall -Wextra -pthread -D_POSIX_C_SOURCE=199309L -m32 -ftest-coverage -fprofile-arcs -DHOST_FS_PATH='"$(HOST_FS_PATH)"'
LDFLAGS  := -pthread -rdynamic
LDLIBS   := -lgcov

SRC :=  pc_provide_local_info.c pc_more_time.c pc_polling_off.c pc_poll_interval.c \
	pc_send_dtmf.c pc_send_sms.c pc_send_ss.c pc_send_ussd.c pc_setup_call.c \
	pc_refresh.c pc_setup_event_list.c pc_timer_management.c pc_utilities.c \
	pc_language_notification.c

OBJ := $(SRC:.c=.o)
HDR := $(SRC:.c=.h)

.PHONY: all clean splint instrument_funcs

all: libpc.a

libpc.a: $(OBJ)
	rm -f libpc.a ; \
	$(AR) crv libpc.a $(OBJ)


clean:
	rm -f libpc.a $(OBJ) *~

splint:
	splint $(INCDIRS) $(HDR) $(SRC)

instrument_funcs: CFLAGS += -finstrument-functions -finstrument-functions-exclude-function-list=main
instrument_funcs: all
