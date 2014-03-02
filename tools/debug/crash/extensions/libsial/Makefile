#
# Copyright 2001 Silicon Graphics, Inc. All rights reserved.
#
#
# Makefile for LIBSIAL
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# Must be berkeley yacc.  Bison will not cut it
YACC = bison

LDIRT    = lex.sial.c lex.sialpp.c sial.tab.c sial.tab.h sialpp.tab.c \
	sialpp.tab.h y.output mkbaseop baseops.c y.tab.c y.tab.h \
	libsial.so* *.output

LIBDIR	 = /usr/lib
TARGETS  = libsial.a

CFLAGS += -O3 -g -fPIC $(TARGET_CFLAGS)

CFILES   = sial_util.c sial_node.c sial_var.c sial_func.c sial_str.c \
	sial_op.c sial_num.c sial_stat.c sial_builtin.c sial_type.c \
	sial_case.c sial_api.c sial_member.c sial_alloc.c sial_define.c \
	sial_input.c sial_print.c

OFILES   = $(CFILES:.c=.o) sialpp.tab.o sial.tab.o lex.sial.o lex.sialpp.o \
	baseops.o

HFILES   = sial.h sial_api.h

LSOURCES = sial-lsed sialpp-lsed sial.l sialpp.l sial.y sialpp.y mkbaseop.c

all: default

showfiles:
	@echo $(RELDIR)/$(CFILES) $(RELDIR)/$(HFILES) $(RELDIR)/$(LSOURCES)

exports: all
	install $(TARGETS) $(ROOT)$(LIBDIR)

headers:
	install -m 644 $(HFILES) $(ROOT)/usr/include

install: headers exports
	(cd scripts ; $(MAKE) install )

baseops.o: mkbaseop.c
	$(CC) $(CFLAGS) -o mkbaseop mkbaseop.c
	./mkbaseop > baseops.c
	$(CC) $(CFLAGS) -c baseops.c

mkbaseop.c sial_member.o sial_op.o sial_stat.o sial_type.o y.tab.o : sial.tab.h

lex.sial.o: lex.sial.c sial.tab.c sial.h
	$(CC) $(CFLAGS) -c lex.sial.c

lex.sial.c: sial.l
	flex -L -Psial -t sial.l > lex.sial.c

sial.tab.c: sial.tab.h

sialpp.tab.o: sialpp.tab.c
	$(CC) $(CFLAGS) -c sialpp.tab.c

sial.tab.o: sial.tab.c
	$(CC) $(CFLAGS) -c sial.tab.c

sial.tab.h : sial.y
	$(YACC) -psial -v -t -d sial.y

lex.sialpp.o: lex.sialpp.c sialpp.tab.c sial.h
	$(CC) $(CFLAGS) -c lex.sialpp.c

lex.sialpp.c: sialpp.l
	flex -Psialpp -t sialpp.l  > lex.sialpp.c

sialpp.tab.c: sialpp.tab.h sial.tab.h

sialpp.tab.h : sialpp.y sial.tab.h
	$(YACC) -psialpp -v -t -d sialpp.y

default: $(TARGETS)

$(CFILES): $(HFILES) sial.tab.h

$(TARGETS): $(OFILES)
	$(AR) ccurl $(TARGETS) $(OFILES)

clean: 
	-/bin/rm -f *.o $(TARGETS) $(LDIRT)

clobber: clean
