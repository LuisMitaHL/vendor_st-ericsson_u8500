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

SMS_DIR := ../../../sms/sms

CN_DIR := ../../../call_network/call_network

COPS_DIR := ../../../../processing/security_framework/cops/cops-api

NETLNK_DIR := ../../../modem_adaptations/mal/modem_lib/mal/libshmnetlnk

COMMON_DIR := ../../../common/common_functionality

CC := gcc
CFLAGS   := -g -Wall -Wextra -pthread -m32 -DHOST_FS_PATH='"$(HOST_FS_PATH)"'
INCDIRS  := -I$(SIM_DIR)/simd \
			-I$(SIM_DIR)/cat \
			-I$(SIM_DIR)/uiccd/include \
			-I$(COPS_DIR)/include \
			-I$(CN_DIR)/libcn/include \
			-I$(SIM_DIR)/libsimcom \
			-I$(SIM_DIR)/include \
			-I$(SIM_DIR)/libsim \
			-I$(SIM_DIR)/libapdu \
			-I$(SMS_DIR)/include \
			-I$(NETLNK_DIR)/include \
			-I$(COPS_DIR)/include \
			-I$(COMMON_DIR)/include

CPPFLAGS := $(INCDIRS)
LDFLAGS  :=  -L$(SIM_DIR)/libsimcom -pthread
LDLIBS   := -lapdu -lsimcom


SRC := sim_stub.c sim_stub_socket.c
OBJ := $(SRC:.c=.o)
HDR := sim_stub.h

.PHONY: all clean splint instrument_funcs

all: libsim_msl.a

libsim_msl.a: $(OBJ)
	rm -f libsim_msl.a ; \
	$(AR) crv libsim_msl.a $(OBJ)

sim_stub.o: sim_stub.c $(HDR)

clean:
	rm -f libsim_msl.a $(OBJ) *~

splint:
	splint $(INCDIRS) $(HDR) $(SRC)

instrument_funcs: CFLAGS += -finstrument-functions -finstrument-functions-exclude-function-list=main
instrument_funcs: all
