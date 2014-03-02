######################### -*- Mode: Makefile-Gmake -*- ########################
## Copyright (C) 2010, ST-Ericsson
## 
## File name       : Makefile
## Description     : builds tuicc executable, STELP way
## 
## Author          : Annwesh Mukherjee <annwesh.xa.mukherjee@stericsson.com>
## 
###############################################################################


CC := gcc
CFLAGS   := -g -Wall -Wextra -std=c99 -pthread -m32 -ftest-coverage -fprofile-arcs
INCDIRS  := -I../libsimcom -I../libsim -I../include -I../simd -I../../../common/common_functionality/include
CPPFLAGS := $(INCDIRS)
LIBDIRS  := -L../libsimcom -L../libsim -L/lib32 -L/usr/lib32
LDFLAGS  := -pthread -rdynamic -m32 $(LIBDIRS)
LDLIBS   := -lsim -lsimcom -ldl -lrt -lgcov
VPATH    := $(subst -L,,$(LIBDIRS))

SRC := tuicc.c
OBJ := $(SRC:.c=.o)
HDR := $(SRC:.c=.h)

.PHONY: all clean splint instrument_funcs

all: tuicc

tuicc: $(OBJ)
	@$(CC) $(OBJ) $(LDFLAGS) $(LDLIBS) -o tuicc

clean:
	rm -f tuicc $(OBJ) *~

splint:
	splint $(INCDIRS) $(HDR) $(SRC)

instrument_funcs: CFLAGS += -finstrument-functions -finstrument-functions-exclude-function-list=main
instrument_funcs: all
