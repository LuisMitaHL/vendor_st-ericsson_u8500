# Makefile for LIB PSCC
#


LIBSTECOM=../connectivity/common/libstecom
MALGPDS = $(HOME)/mal/libmalgpds
MALPIPE = $(HOME)/mal/libmalpipe
MALPHONET = $(HOME)/mal/libphonet

ifndef INSTALL_MOD_PATH
INSTALL_MOD_PATH=/usr/local/lib
endif

ifndef CROSS_COMPILE
CROSS_COMPILE=
endif

AS              = $(CROSS_COMPILE)as
LD              = $(CROSS_COMPILE)ld
CC              = $(CROSS_COMPILE)gcc
LDO             = $(CC)
CPP             = $(CC) -E
AR              = $(CROSS_COMPILE)ar
NM              = $(CROSS_COMPILE)nm
STRIP           = $(CROSS_COMPILE)strip
OBJCOPY         = $(CROSS_COMPILE)objcopy
OBJDUMP         = $(CROSS_COMPILE)objdump

ifdef STEPSCC_USE_MAL
MODEM_OBJS=pscc_bearer_mal.o pscc_qpc_mal.o
MODEM_LIBS=-lmalgpds -lphonet -lmalpipe
MODEM_CFLAGS=-DPSCC_USE_MAL -I$(MALGPDS)/include
MODEM_LDFLAGS=-L$(MALGPDS) -L$(MALPIPE) -L$(MALPHONET)
else
MODEM_OBJS=pscc_bearer_mfl.o pscc_netdev_caif.o pscc_pco.o
MODEM_LIBS=-lmfl_cas -lmfl -lnlcom -lcrypto
MODEM_CFLAGS=-DPSCC_USE_MFL
MODEM_LDFLAGS=
endif

STEPSCC_LIBS=-lstecom -lmpl -lpscc $(MODEM_LIBS)

ifndef STEPSCC_LDFLAGS
STEPSCC_LDFLAGS = -L$(LIBSTECOM) -L../psdata/libmpl -L../psdata/libpscc $(MODEM_LDFLAGS)
endif

ifndef STEPSCC_CFLAGS
STEPSCC_CFLAGS= -Wall -Wextra -g -fPIC -I../psdata/libmpl -I../psdata/libpscc -I$(LIBSTECOM) $(MODEM_CFLAGS) $(CFLAGS) 
endif

PROG=psccd
OBJS = pscc_handler.o psccd.o pscc_object.o pscc_runscript.o $(MODEM_OBJS)

build: psccd

install: 
	cp psccd $(INSTALL_MOD_PATH)/bin/psccd

uninstall: 
	@rm $(INSTALL_MOD_PATH)/bin/psccd

psccd: $(OBJS)
	$(CC) $(STEPSCC_LDFLAGS) -o psccd $(OBJS) $(STEPSCC_LIBS)

%.o: %.c
	$(CC) -c -o $@ $(STEPSCC_CFLAGS) $<

clean:
	rm $(OBJS)
