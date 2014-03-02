######################### -*- Mode: Makefile-Gmake -*- ########################
## Copyright (C) 2010, ST-Ericsson
## 
## File name       : Makefile
## Description     : builds tcat executable, STELP way
## 
## Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
## 
###############################################################################
CN_DIR := ../../../call_network/call_network

CC := gcc
CFLAGS   := -g -Wall -Wextra -std=c99 -pthread -m32 -ftest-coverage -fprofile-arcs
INCDIRS  := -I../libsimcom -I../libsim -I../libapdu -I../include -I$(CN_DIR)/libcn/include
CPPFLAGS := $(INCDIRS)
LIBDIRS  := -L../libsimcom -L../libsim -L../libapdu -L/lib32 -L/usr/lib32
LDFLAGS  := -pthread -rdynamic -m32 $(LIBDIRS)
LDLIBS   := -lsim -lsimcom -lapdu -ldl -lrt -lgcov
VPATH    := $(subst -L,,$(LIBDIRS))


SRC := tcat.c
OBJ := $(SRC:.c=.o)
HDR := $(SRC:.c=.h)

.PHONY: all clean splint instrument_funcs

all: tcat

tcat: $(OBJ)
	@$(CC) $(OBJ) $(LDFLAGS) $(LDLIBS) -o tcat

clean:
	rm -f tcat $(OBJ) *~

splint:
	splint $(INCDIRS) $(HDR) $(SRC)

instrument_funcs: CFLAGS += -finstrument-functions -finstrument-functions-exclude-function-list=main
instrument_funcs: all
