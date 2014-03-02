DEBUG = 2
#TARGET=native
WARNS = 3

COPS_ROOT = ..

COPS_SOCKET_PATH ?= ./cops.socket

CPPFLAGS += -DCOPS_SOCKET_PATH=$(COPS_SOCKET_PATH) \
		-Iinclude \
		-I. \
		-I../shared \
		-I../shared/ta

CLEANFILES += $(COPS_SOCKET_PATH)

CPPFLAGS += -DCOPS_TAPP_EMUL

ifeq ($(COPS_DUMMY_DUMMY_TEST),true)
CPPFLAGS += -DDUMMY_WITHOUT_IMEI_SUPPORT=1 \
		-I. \

SRCS += \
	cops_api_client_dummy.c \


else
# The lib
VPATH += ../shared/ta
SRCS += \
	cops_api_client.c \
	cops_api_connection.c \
	cops_api_server.c \
	../shared/cops_socket.c \
	../shared/cops_fd.c \
	../shared/cops_error.c \
	../shared/cops_shared_util.c \
	../shared/cops_sipc.c \
	../shared/cops_log.c \
	cops_sipc_message.c \
	cops_tapp_sipc.c
endif

LIB = cops

IGNORE_DEFAULT_INSTALL=1

include $(COPS_ROOT)/common.mk

# Overriding the common install directive.
.PHONY: install
install: $(LIBNAME)
	$(INSTALL) -d $(DESTDIR)$(libdir)
	$(INSTALL) -d $(DESTDIR)$(includedir)
	$(INSTALL_DATA) $(LIBNAME) $(DESTDIR)$(libdir)/$(LIBNAME)
	$(INSTALL_DATA) include/cops.h $(DESTDIR)$(includedir)/cops.h
	$(INSTALL_DATA) include/cops_types.h $(DESTDIR)$(includedir)/cops_types.h
