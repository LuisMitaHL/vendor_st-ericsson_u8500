CC := gcc
CFLAGS := -g -O0 -ftest-coverage -fprofile-arcs
SIM := $(TOP)/vendor/st-ericsson/access_services/sim/sim
INCDIRS := -Itest -Iinclude -Icops/include -Imal/include -Isms/include -Isim/include -I../selector/include  -I../main/include -I../parser/include\
            -I$(SIM)/include -I$(SIM)/cat/libcat
CPPFLAGS := $(INCDIRS) -DEXE_USE_SIM_SERVICE -DWHITE_BOX_TEST
LDFLAGS  := -lc
LDLIBS   := -lgcov

SRC := src/predial_check.c
OBJ := $(SRC:.c=.o)
HDR :=

COV_STUFF := $(SRC:.c=.gcda) $(SRC:.c=.gcno)

.PHONY: all clean test test_ng

all: test

test: libpredial.a

libpredial.a: $(OBJ) $(HDR)
	rm -f libpredial.a ; \
	$(AR) crv libpredial.a $(OBJ)

clean:
	rm -f $(OBJ) $(TEST_OBJ) white_box_test libexe.a $(COV_STUFF)
