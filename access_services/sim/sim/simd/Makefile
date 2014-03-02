######################### -*- Mode: Makefile-Gmake -*- ########################
## Copyright (C) 2010, ST-Ericsson
## 
## File name       : Makefile
## Description     : builds simd
## 
## Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
## 
###############################################################################


MODEM_DIR := ../../sim_msl/libmsl

SMS_DIR := /local/sim/vendor/st-ericsson/access_services/sms/sms

CN_DIR := ../../../call_network/call_network

COMMON_DIR := ../../../common/common_functionality

CC := gcc
INCDIRS  := -I../simd -I../cat -I../uiccd/include -I../libsimcom -I../libsim \
	    -I$(MODEM_DIR) -I../libapdu -I../include \
	    -I$(CN_DIR)/libcn/include -I$(SMS_DIR)/include \
	    -I$(COMMON_DIR)/include
CFLAGS   := -g -Wall -Wextra -std=c99 -D_POSIX_C_SOURCE=199309L -pthread -m32 -ftest-coverage -fprofile-arcs -DHOST_FS_PATH='"$(HOST_FS_PATH)"'
CPPFLAGS := $(INCDIRS)
LIBDIRS  := -L../cat -L../uiccd -L../libsimcom -L../libsim \
	    -L$(MODEM_DIR) -L../libapdu -L../libpc -L$(SMS_DIR)
LDFLAGS  := -pthread $(LIBDIRS) -lrt -m32
LDLIBS   := -lsim -lcatd -luiccd -lcat_internal -lsim_msl -lsimcom -lapdu -lpc -lgcov -lm
VPATH    := $(subst -L,,$(LIBDIRS))

SRCH:= catd_reader.c catd_cn_client.c simd_timer.c
SRC := simd.c $(SRCH) sim_log_client.c
OBJ := $(SRC:.c=.o)
HDR := $(SRCH:.c=.h) catd_modem.h sim_log.h

.PHONY: all clean splint instrument_funcs

all: simd

simd: $(OBJ)
	@$(CC) $(OBJ) $(LDFLAGS) $(LDLIBS) -o simd

clean:
	rm -f simd $(OBJ) *~ core

splint:
	splint $(INCDIRS) $(HDR) $(SRC)

instrument_funcs: CFLAGS += -finstrument-functions -finstrument-functions-exclude-function-list=main
instrument_funcs: all
