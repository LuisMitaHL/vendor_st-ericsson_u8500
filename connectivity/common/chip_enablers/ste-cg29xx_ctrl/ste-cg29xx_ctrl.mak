PACKAGE_NAME=ste-cg29xx_ctrl-1.0

CSPSA := ../../../../storage/parameter_storage/cspsa
CSPSALIBDIR ?= $(CSPSA)

CFLAGS += -c -Wall -O2
LDFLAGS += -L$(CSPSALIBDIR) -lcspsa
INCLUDES += -I$(CSPSA)

build: ste-cg29xx_ctrl

install: build
	@$(PACKAGE_FILE) /usr/bin/ste-cg29xx_ctrl $(CURDIR)/ste-cg29xx_ctrl 755 0 0

ste-cg29xx_ctrl: ste-cg29xx_ctrl.o
	$(CC) $(LDFLAGS) $^ -o $@

%.o: %.c
	${CC} ${CFLAGS} ${INCLUDES} -c $<

clean:
	@rm -rf ste-cg29xx_ctrl.o ste-cg29xx_ctrl

.PHONY: ste-cg29xx_ctrl clean
