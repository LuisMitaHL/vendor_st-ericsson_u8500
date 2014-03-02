######################### -*- Mode: Makefile-Gmake -*- ########################
## Copyright (C) 2010, ST-Ericsson
## 
## File name       : Makefile
## Description     : for libsimcom, common SIM related functionality
## 
## Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
## 
###############################################################################



CC       := gcc
INCDIRS  := -I../include -I../libsim -I./
CPPFLAGS := $(INCDIRS) 
CFLAGS   := -g -Wall -Wextra -std=c99 -pedantic-errors -pthread -m32 -ftest-coverage -fprofile-arcs -DHOST_FS_PATH='"$(HOST_FS_PATH)"'
LDFLAGS  := -pthread
LDLIBS   := -lsim -lgcov

SRC := cat_internal.c sim_internal.c event_stream.c cat_barrier.c msgq.c \
	state_machine.c client_data.c func_trace.c
OBJ := $(SRC:.c=.o)
HDR := $(SRC:.c=.h) uicc_internal.h

.PHONY: all clean splint instrument_funcs

all: libsimcom.a libcat_internal.a

libsimcom.a: $(OBJ)
	rm -f libsimcom.a ; \
	$(AR) crv libsimcom.a $(OBJ)

libcat_internal.a: cat_internal.o
	rm -f libcat_internal.a ; \
	$(AR) crv libcat_internal.a cat_internal.o


clean:
	rm -f libsimcom.a libcat_internal.a $(OBJ) *~

splint:
	splint $(INCDIRS) $(HDR) $(SRC)

instrument_funcs: CFLAGS += -finstrument-functions -finstrument-functions-exclude-function-list=main
instrument_funcs: all
