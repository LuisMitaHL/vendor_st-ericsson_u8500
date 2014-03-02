LIB := libthermal

SRCS := thermal.c
OBJS := $(SRCS:%.c=%.o)

CFLAGS += -g3 -fPIC -Wall -I../include

all: $(LIB)

$(LIB): $(OBJS)
	$(CC) $(CFLAGS) -shared -W1,-soname,$(LIB).so -o $(LIB).so $(OBJS)

clean:
	-rm -f $(LIB).so $(OBJS) *~
