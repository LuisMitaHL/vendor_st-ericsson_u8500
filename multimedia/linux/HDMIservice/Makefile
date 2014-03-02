# Makefile for HDMIservice
#
# Copyright (C) ST-Ericsson AB 2011.
#

PACKAGE_NAME=hdmi_service

CFLAGS += -c -Wall -O2 -fPIC
LDFLAGS += -L./ -lpthread -shared
LDFLAGS_2 = -L./
INCLUDES += -I./include
HDMILIBS = hdmiservice.so

build: copyheader hdmiservice.so hdmistart

install: clean build
	@$(PACKAGE_FILE) /usr/lib/hdmiservice.so $(CURDIR)/hdmiservice.so 755 0 0
	@$(PACKAGE_FILE) /usr/bin/hdmistart $(CURDIR)/hdmistart 755 0 0

%.o: src/%.c
	${CC} ${CFLAGS} ${INCLUDES} -c $<

hdmiservice.so: cec.o edid.o hdcp.o hdmi_service_api.o hdmi_service.o kevent.o \
	setres.o socket.o
	$(CC) $(LDFLAGS) $^ -o $@

hdmistart: hdmi_service_start.o
	$(CC) $(LDFLAGS_2) $^ -o $@ $(HDMILIBS)

copyheader:
	@mkdir -p ./include/linux
	@cp $(KERNELDIR)/include/linux/compdev.h ./include/linux

clean:
	@rm -rf cec.o edid.o hdcp.o hdmi_service_api.o hdmi_service.o kevent.o \
	setres.o socket.o hdmiservice.so hdmi_service_start.o hdmistart include/linux

.PHONY: hdmiservice.so clean
