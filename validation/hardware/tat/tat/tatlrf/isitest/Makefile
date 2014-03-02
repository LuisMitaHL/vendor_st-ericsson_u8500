#**********************************************************************
#
# Synopsis: TATLRF testu Makefile- Linux Application Build Rules.
#
#**********************************************************************

include .config.mk
include ../../declare_TAT

DEPENDENCES=tat/tatlmodemsrv \
	modemservices/isi_common_lib \
	modemservices/modem_storage_agent \
	miniape/modem_ape

PACKAGE_NAME:=tatlrf/testapp

PACKAGE_EXEC_NAME=rfisitst

OBJECTS=./isitest.o \
	../isimsg.o \
	../misc.o \
	../isidump.o

HEADER_FILES=$(wildcard ../*.h)

CFLAGS+=-I$(HATS_HEADERS_DIR)/hal_headers \
	-I ../ \
	-D_UNIT_TEST \
	-Wall -Werror

deps: _deps

unpack: .unpacked
.unpacked:
	echo -e "\t-----> $(PACKAGE_NAME) : $@"
#	Do you tar xzvf here or anything
	touch $@

patch: .patched
.patched: .unpacked
	echo -e "\t-----> $(PACKAGE_NAME) : $@"
#	Apply a patch
	touch $@

prepare: .prepared
.prepared: deps .patched

clean:
	echo -e "\t-----> $(PACKAGE_NAME) : $@"
	rm -f $(OBJECTS) $(TARGET_EXE)
	rm -f .compiled .installed .patched .prepared .unpacked .version

#
# create executable
#
compile: .compiled
.compiled: .prepared $(PACKAGE_EXEC_NAME)
	echo -e "\t-----> $(PACKAGE_NAME) : $@"
	touch $@

$(PACKAGE_EXEC_NAME): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(PACKAGE_EXEC_NAME) $^ -lm -ltatmodemlib -lisicommon -lisidriver -lmodem -lphonet_hats -lpthread

$(OBJECTS): .prepared $(HEADER_FILES)
$(OBJECTS): %.o : %.c
	echo -e "\t-----> file(s) $? is (are) new, build $@"
#	$(CC) -c -Wall -g -D_DEBUG -fPIC $(CFLAGS) $< -o $(@)
	$(CC) -c -Wall -g -fPIC $(CFLAGS) $< -o $(@)

install: deps .installed
.installed: .compiled
	echo -e "\t-----> $(PACKAGE_NAME) : $@"
