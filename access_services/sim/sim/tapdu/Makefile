######################### -*- Mode: Makefile-Gmake -*- ########################
## Copyright (C) 2010, ST-Ericsson
## 
## File name       : Makefile
## Description     : builds test program for apdu
## 
## Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
## 
###############################################################################
CN_DIR := ../../../call_network/call_network

CC       := gcc
INCDIRS  := -I../include -I../libapdu -I../libsimcom -I$(CN_DIR)/libcn/include	# for trace only (fix this)
CPPFLAGS := $(INCDIRS)
CFLAGS   := -g -Wall -Wextra -std=c99 -pthread -m32 $(INCDIRS) -ftest-coverage -fprofile-arcs
LIBDIRS  := -L../libapdu -L../libsimcom -L/lib32 -L/usr/lib32
LDFLAGS  := -pthread -rdynamic -m32 $(LIBDIRS)
LDLIBS   := -lapdu -ldl -lrt -lgcov -lsimcom # simcom for trace only (fix this)
VPATH    := $(subst -L,,$(LIBDIRS))

SRC := apdu_test.c
OBJ := $(SRC:.c=.o)
HDR := $(SRC:.c=.h)

.PHONY: all clean splint instrument_funcs

all: apdu_test

apdu_test: $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) $(LDLIBS) -o apdu_test

clean:
	rm -f apdu_test $(OBJ) *~

splint:
	splint $(INCDIRS) $(HDR) $(SRC)

instrument_funcs: CFLAGS += -finstrument-functions -finstrument-functions-exclude-function-list=main
instrument_funcs: all
