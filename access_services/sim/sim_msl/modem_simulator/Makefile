######################### -*- Mode: Makefile-Gmake -*- ########################
## Copyright (C) 2010, ST-Ericsson
## 
## File name       : Makefile
## Description     : Make file for sim modem simulator
## 
## Author          : Haiyuan Bu <haiyuan.bu@stericsson.com>
## 
###############################################################################

SIM_DIR := ../../sim

CC := gcc
CFLAGS   := -g -Wall -Wextra -std=c99 -pedantic-errors -pthread -m32 -ftest-coverage -fprofile-arcs -DHOST_FS_PATH='"$(HOST_FS_PATH)"'
INCDIRS  := -I$(SIM_DIR)/libsimcom -I../libmsl
CPPFLAGS := $(INCDIRS)
LIBDIRS  := -L$(SIM_DIR)/libsimcom -L../libmsl -L/lib32 -L/usr/lib32
LDFLAGS  := -pthread -rdynamic -m32 $(LIBDIRS)
LDLIBS   := -lsimcom -lsim_msl -ldl -lrt -lgcov
VPATH    := $(subst -L,,$(LIBDIRS))


SRC := modem_simulator.c
OBJ := $(SRC:.c=.o)
HDR := $(SRC:.c=.h)

.PHONY: all clean splint instrument_funcs

all: modem_simulator

modem_simulator: $(OBJ)
	@$(CC) $(OBJ) $(LDFLAGS) $(LDLIBS) -o modem_simulator

clean:
	rm -f modem_simulator $(OBJ) *~

splint:
	splint $(INCDIRS) $(HDR) $(SRC)

instrument_funcs: CFLAGS += -finstrument-functions -finstrument-functions-exclude-function-list=main
instrument_funcs: all
