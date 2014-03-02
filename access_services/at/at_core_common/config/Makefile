CC := gcc
CFLAGS   := -g -Wall -Wextra -std=c99 -pedantic-errors -pthread
INCDIRS  := -I./include
CPPFLAGS := $(INCDIRS)
LDFLAGS  :=
LDLIBS   :=

SRC :=
OBJ := $(SRC:.c=.o)

.PHONY: all test clean

all: test

test: $(OBJ)

clean:
	rm -f $(OBJ)
