PROG := thermald

SRCS := main.c config.c sensors.c actions.c hwmon.c socket.c mitigation_actions.c log.c
OBJS := $(SRCS:%.c=%.o)

INCLUDES := -I../include -I../libthermal

CFLAGS += -Wall -g3

LIBS += -lpthread

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) -o $(PROG) $(OBJS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

clean:
	-rm -rf $(PROG) $(OBJS) actions_table.h *~ *.bak
