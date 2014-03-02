PROG := thermaltest

SRCS := main.c test.c thermal.c
OBJS := $(SRCS:%.c=%.o)

INCLUDES := -I../libthermal

CFLAGS += -g3 -Wall -L../libthermal 

LIBS = -lthermal -lpthread

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) -o $(PROG) $(OBJS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

clean:
	-rm -rf $(PROG) $(OBJS) *~
