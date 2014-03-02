# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.

include $(MM_MAKEFILES_DIR)/SharedConfig.mk

CPPFLAGS += -I$(MMROOT)/linux/trace/api

all: main

clean:
	rm *~ *.o main

%.o: %.c
	$(CC) $(CPPFLAGS) -c $< -o $@

main: main.o
	$(LD_EXE) $(FIXED_LDEXEFLAGS) $< -o $@ -L$(MM_LIB_DIR) -Wl,-Bstatic -ltrace -Wl,-Bdynamic
