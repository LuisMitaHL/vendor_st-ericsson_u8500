PACKAGE_NAME=modem-supervisor
PACKAGE_FILE=/media/disk/frq93151/u8500/ptxdist/lbp/ptxdist/ux500

ifdef USE_THUMB
CFLAGS := -mthumb -mthumb-interwork $(CFLAGS)
endif

CFLAGS := -c -Wall -Os $(STAGING_AREA)/usr/include -D _GNU_SOURCE $(CFLAGS) 
#LDFLAGS := -L$(STAGING_AREA)/usr/lib -lrt $(LDFLAGS)
LDFLAGS :=  $(LDFLAGS)
LDLIBS   := -L. -lbassapp -ltee -lrt
#LD_LIBRARY_PATH=./ 

install: build
	#@$(PACKAGE_FILE)/bin/modem-sup $(CURDIR)/modem-sup 755 0 0
	cp ./libbassapp.so /usr/lib/.
	cp ./libtee.so /usr/lib/.
	cp ./modem-sup /usr/bin/.
	@echo " ******* modem-supervisor installation not done ***** $(PACKAGE_FILE) "

shm_netlnk_debug.o: shm_netlnk_debug.c modem-sup.mak
	@echo "shm_netlnk_debug.o:"
	$(CC) $(CFLAGS) shm_netlnk_debug.c -o $@

fwhandler: fwhandler.c modem-sup.mak
	@echo "fwhandler.o"
	$(CC) $^ $(LDFLAGS) -o $@ $(LIB) -lpthread

mlr_client: mlr_client.o  modem-sup.mak
	@echo "mlr_client.o:"
	$(CC) $^ $(LDFLAGS) -o $@ $(LIB) -lpthread

mlr_server_debug: mlr_server_debug.c modem-sup.mak
	@echo "mlr_server_debug"
	$(CC) $(CFLAGS) mlr_server_debug.c -o $@ $(LIB) -lpthread

modem-sup.o: modem-sup.c modem-sup.mak
	@echo "modem-sup.o"
	$(CC) $(CFLAGS) modem-sup.c -o $@

modem-sup: mlr_client.o modem-sup.o shm_netlnk_debug.o fwhandler.o
	@echo "modem-sup:"
#     $(CC) -v
	$(CC) -o $@ $(LDFLAGS) $(LDLIBS) $^

build: modem-sup mlr_server_debug 

clean:
	@echo " ******* modem-supervisor cleaning ***** "
	@rm -rf  *.o modem-sup

