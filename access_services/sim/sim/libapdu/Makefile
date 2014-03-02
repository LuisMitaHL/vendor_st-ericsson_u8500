######################### -*- Mode: Makefile-Gmake -*- ########################
## Copyright (C) 2010, ST-Ericsson
## 
## File name       : Makefile
## Description     : builds libapdu
## 
## Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
## 
###############################################################################
CN_DIR := ../../../call_network/call_network

CC       := gcc
INCDIRS  := -I../include -I../libsimcom -I$(CN_DIR)/libcn/include
CPPFLAGS := $(INCDIRS)
CFLAGS   := -g -Wall -Wextra -std=c99 -pthread -m32 -ftest-coverage -fprofile-arcs -DSAT_ENV_NO_INTERLEAVING_SUPPORT -DHOST_FS_PATH='"$(HOST_FS_PATH)"'
LDFLAGS  := -pthread -rdynamic
LDLIBS   := -lgcov

SRC := apdu.c apdu_utilities.c
OBJ := $(SRC:.c=.o)
HDR := $(SRC:.c=.h)

.PHONY: all clean splint instrument_funcs

all: libapdu.a

libapdu.a: $(OBJ)
	rm -f libapdu.a ; \
	$(AR) crv libapdu.a $(OBJ)


clean:
	rm -f libapdu.a $(OBJ) *~

splint:
	splint $(INCDIRS) $(HDR) $(SRC)

instrument_funcs: CFLAGS += -finstrument-functions -finstrument-functions-exclude-function-list=main
instrument_funcs: all
