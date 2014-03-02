CC	:= gcc
CFLAGS	:= -g -O0  -ftest-coverage -fprofile-arcs -DENABLE_FTD
LDFLAGS	:= -lc
LDLIBS	:=

COMMON_FUNCTIONALITY := $(TOP)/vendor/st-ericsson/access_services/common/common_functionality
INCDIRS	:=	-I./include			\
		-I../../include			\
		-I../../config/include		\
		-I../selector/include		\
		-I../main/include		\
		-I../anchor/include		\
		-I../executor/include		\
		-I../parser/include		\
		-I../selector/include\
		-I$(COMMON_FUNCTIONALITY)/include

SRC	:=	src/atc_context.c	\
		src/atc_log_client.c

HDR	:=	include/atc_connection.h	\
		include/atc_context.h		\
		include/atc_log.h		\
		include/atc.h

CFLAGS	+= $(INCDIRS) -DWHITE_BOX_TEST
TEST_HDR :=
OBJ	:= $(SRC:.c=.o)
COV_STUFF := $(SRC:.c=.gcda) $(SRC:.c=.gcno)

.PHONY: all clean test

all: test

test: libmain.a

libmain.a: $(OBJ) $(HDR)
	rm -f libmain.a ; \
	$(AR) crv libmain.a $(OBJ)

clean:
	rm -f $(OBJ) $(TEST_OBJ) white_box_test libmain.a $(COV_STUFF)
