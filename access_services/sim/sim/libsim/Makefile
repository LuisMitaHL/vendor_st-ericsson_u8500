######################### -*- Mode: Makefile-Gmake -*- ########################
## Copyright (C) 2010, ST-Ericsson
## 
## File name       : Makefile
## Description     : build cat lib STELP Style
## 
## Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
## 
###############################################################################

CC := gcc
CFLAGS   := -g -Wall -Wextra -std=c99 -pedantic-errors -pthread -m32 -ftest-coverage -fprofile-arcs -DHOST_FS_PATH='"$(HOST_FS_PATH)"'
INCDIRS  := -I../libapdu -I../libsimcom -I../include -I../simd
CPPFLAGS := $(INCDIRS)
LDFLAGS  := -L../libapdu -L../libsimcom
LDLIBS   := -lsim -lapdu -pthread -lgcov

SRC := cat.c uicc.c sim.c sync.c
OBJ := $(SRC:.c=.o)
HDR := $(SRC:.c=.h)

.PHONY: all clean splint instrument_funcs

all: libsim.a 

libsim.a: $(OBJ)
	rm -f libsim.a ; \
	$(AR) crv libsim.a $(OBJ)


clean:
	rm -f libsim.a $(OBJ) *~

splint:
	splint $(INCDIRS) $(HDR) $(SRC)

instrument_funcs: CFLAGS += -finstrument-functions -finstrument-functions-exclude-function-list=main
instrument_funcs: all
