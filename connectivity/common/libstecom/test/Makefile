# Makefile for libstecom test

#ifndef CC
CC=gcc
#endif

ifndef TEST_CFLAGS
TEST_CFLAGS=-O2 -Wall -Werror -g
endif

ifndef LDO
LDO=$(CC)
endif

ALL=test-cli test-srv

TEST_CLI_OBJS = test-cli.o
TEST_SRV_OBJS = test-srv.o

LIB_OBJS = ../subscribe.o ../stecom.o

TEST_LIBS = 

ifdef RUN_TEST
#TEST_CFLAGS += -fprofile-arcs -ftest-coverage
TEST_LIBS += -lgcov
endif

ifdef NETLINK_DIR
TEST_LIBS += -L$(NETLINK_DIR)/lib
endif

-include ../.config

ifdef CONFIG_NL80211_TESTMODE
CFLAGS += -DCONFIG_NL80211_TESTMODE
LIB_OBJS += ../nl80211testmode.o
TEST_LIBS += -lnl
endif

-include $(OBJS:%.o=%.d)

all: $(ALL)

test-cli: $(TEST_CLI_OBJS)
	$(LDO) $(TEST_LDFLAGS) -o test-cli $(TEST_CLI_OBJS) $(LIB_OBJS) $(TEST_LIBS)

test-srv: $(TEST_SRV_OBJS)
	$(LDO) $(TEST_LDFLAGS) -o test-srv $(TEST_SRV_OBJS) $(LIB_OBJS) $(TEST_LIBS)

%.o: %.c
	$(CC) -MD -c -o $@ $(TEST_CFLAGS) $<

clean:
	rm -f *~ *.o *.d $(ALL)

