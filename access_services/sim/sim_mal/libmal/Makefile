######################### -*- Mode: Makefile-Gmake -*- ########################
## Copyright (C) 2010, ST-Ericsson
## 
## File name       : Makefile
## Description     : build mal modem library, STELP way
## 
## Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
## 
###############################################################################

SIM_DIR := ../../sim
MODEM_DIR := ../../../modem_adaptations/mal/modem_lib/mal

CC := gcc
# -pedantic-errors removed, since we include linux headers somewhere...
CFLAGS   := -g -Wall -Wextra -std=c99 -pthread -m32
INCDIRS  := \
    -I$(MODEM_DIR) \
    -I$(MODEM_DIR)/libmalmce/include \
    -I$(MODEM_DIR)/libmaluicc/include \
    -I$(MODEM_DIR)/mal_util/include \
    -I$(SIM_DIR)/include \
    -I$(SIM_DIR)/libsimcom \
    -I$(SIM_DIR)/libapdu \
    -I$(SIM_DIR)/cat \
    -I$(SIM_DIR)/simd \
    -I$(SIM_DIR)/libsim \
    -I$(SIM_DIR)/uiccd/include

CPPFLAGS := $(INCDIRS)
CFLAGS   += -DLOCAL_COMPILE_TEST
LDFLAGS  := -pthread
LDLIBS   := -lapdu

SRC := sim_mal_modem.c sim_file_path.c
SRC += sim_malmce_rsp_trace.c sim_maluicc_req_trace.c sim_maluicc_rsp_trace.c sim_trace_util.c

OBJ := $(SRC:.c=.o)
HDR := 

.PHONY: all clean splint

all: libmal.a

libmal.a: $(OBJ)
	rm -f libmal.a
	$(AR) crv libmal.a $(OBJ)

clean:
	rm -f libmal.a $(OBJ) *~

splint:
	splint $(INCDIRS) $(HDR) $(SRC)
