#/************************************************************************
# *                                                                      *
# *  Copyright (C) 2011 ST-Ericsson SA                                   *
# *                                                                      *
# *  This software is released under the terms of the BSD-style          *
# *  license accompanying TOC.                                           *
# *                                                                      *
# *  Author: Mikael LARSSON <mikael.xt.larsson@stericsson.com>           *
# *                                                                      *
# ************************************************************************/

ifndef CC
	CC = gcc
endif
ifndef AR
	AR = ar
endif

CTAGS = -ctags

CFLAGS = -Wall -Werror -Wextra -static
ARFLAGS = -cqs

CMDL		= tocparser
CMDL_COBJS	= toc_parser_cmdline.c
CMDL_SRCS	:= $(CMDL_COBJS:.o=.c)
CMDL_OBJS	:= $(CMDL_SRCS:.c=.o)

LIB	= libtocparser.a
COBJS	= toc_parser.o
SRCS	:= $(COBJS:.o=.c)
OBJS	:= $(SRCS:.c=.o)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: all
all:	tags $(LIB) $(CMDL)

.PHONY: clean
clean:
	rm -f $(LIB) $(OBJS) $(CMDL_OBJS) $(CMDL) tags

tags: $(SRCS)
	$(CTAGS) *.[ch]

$(LIB): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

$(CMDL): $(CMDL_OBJS) $(LIB)
	$(CC) $^ -o $@

.PHONY: check
check:
	-cppcheck -a -s -v --unused-functions *.c

