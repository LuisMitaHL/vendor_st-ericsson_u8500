DEBUG=2
#TARGET=native
WARNS=3

COPS_ROOT=../..
USE_COPS_API=1

COPS_TAPP_EMUL=1

ifdef COPS_TAPP_EMUL
 CPPFLAGS +=-DCOPS_TAPP_EMUL
endif

CPPFLAGS += -I$(COPS_API)
CPPFLAGS += -I./
CPPFLAGS += -I$(COPS_ROOT)/shared

# Testapp
SRCS+= gen_sipc.c
# bass_app.c
LDADD+= -lssl

PROG=gen_sipc

include $(COPS_ROOT)/common.mk

