######################### -*- Mode: Makefile-Gmake -*- ########################
## Copyright (C) 2010, ST-Ericsson
## 
## File name       : Makefile
## Description     : builds catd libary
## 
## Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
## 
###############################################################################

SMS_DIR := ../../../sms/sms

CN_DIR := ../../../call_network/call_network

COMMON_DIR := ../../../common/common_functionality

CC := gcc
CFLAGS   := -g -Wall -Wextra -std=c99 -pthread -m32 -ftest-coverage -fprofile-arcs -DSAT_BIP_SUPPORT -DSAT_ENV_NO_INTERLEAVING_SUPPORT -DHOST_FS_PATH='"$(HOST_FS_PATH)"'
INCDIRS  := -I../simd -I../libsimcom -I../libsim -I../libapdu \
	    -I../include -I../uiccd/include -I../libpc -I$(CN_DIR)/libcn/include -I$(SMS_DIR)/include \
	    -I$(COMMON_DIR)/include
CPPFLAGS := $(INCDIRS)
LDFLAGS  :=
LDLIBS   := -lgcov

SRC := catd.c catd_msg.c catd_tpl.c
OBJ := $(SRC:.c=.o)
HDR := $(SRCH:.c=.h)


.PHONY: all clean splint instrument_funcs

all: libcatd.a

libcatd.a: $(OBJ)
	rm -f libcatd.a
	$(AR) crv libcatd.a $(OBJ)

clean:
	rm -f libcatd.a $(OBJ) *~ core

splint:
	splint $(INCDIRS) $(HDR) $(SRC)

instrument_funcs: CFLAGS += -finstrument-functions -finstrument-functions-exclude-function-list=main
instrument_funcs: all
