# Kernel config prerequisits:
# System Type->ST-Ericsson CA9 Dual Core platform :
# ST x500 target platform  --->5500(FairBanks) Simulator platform

PACKAGE_NAME=modemlogrelay-1.0

MALPATH = ../mal/modem_lib/mal/libphonet/include
KERNELPATH = ../../../../../kernel/include
LIBPATH = ../mal/modem_lib/lib/

# Compile this package with small thumb code
ifdef USE_THUMB
CFLAGS := -DT32_DEBUG -g  -mthumb -mthumb-interwork $(CFLAGS)
endif

CFLAGS := $(CFLAGS) -I$(MALPATH) -I$(KERNELPATH)


install: build
	@$(PACKAGE_DIR) /bin 755 0 0
	@$(PACKAGE_FILE) /bin/modemlogrelay $(CURDIR)/modemlogrelay 700 0 0
	@$(PACKAGE_FILE) /bin/mal $(CURDIR)/mal 700 0 0


modemlogrelay: modemlogrelay.o dgramsk.o libmon.o coredump.o xfiledecoder.o memfile.o
	$(CC) $(CFLAGS) modemlogrelay.c -o $@ -lpthread dgramsk.o libmon.o $(LIBPATH)/libmalmon.a $(LIBPATH)/libisicommon.a  $(GCOV_LIB_PATH)/libgcov.a $(LIBPATH)/libphonet.a -lm

mal:mal.o dgramsk.o 
	$(CC) $(CFLAGS) mal.c -o $@ -lpthread dgramsk.o

dgramsk.o: dgramsk.c modemlogrelay.mak
	@echo "dgramsk.o:"
	$(CC) $(CFLAGS) -c dgramsk.c -o $@

libmon.o: libmon.c modemlogrelay.mak
	@echo "libmon.o:"
	$(CC) $(CFLAGS) -c libmon.c -o $@

coredump.o: coredump.c modemlogrelay.mak
	@echo "coredump.o:"
	$(CC) $(CFLAGS) -c coredump.c -o $@

xfiledecoder.o: xfiledecoder.c modemlogrelay.mak
	@echo "xfiledecoder.o:"
	$(CC) $(CFLAGS) -c xfiledecoder.c -o $@

memfile.o: memfile.c modemlogrelay.mak
	@echo "memfile.o:"
	$(CC) $(CFLAGS) -c memfile.c -o $@

build: modemlogrelay mal

clean:
	@rm -rf *.o modemlogrelay mal
