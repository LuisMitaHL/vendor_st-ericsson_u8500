DEBUG=2
#TARGET=native
WARNS=1
NOWERROR=1

COPS_ROOT=../..
USE_COPS_API=1

# Testapp
CPPFLAGS += -I.
SRCS+= cops_test.c cops_test_framework.c
LDADD+= -lssl
CLEANFILES+=cops.log cops.log2
PROG=cops_test

include $(COPS_ROOT)/common.mk
