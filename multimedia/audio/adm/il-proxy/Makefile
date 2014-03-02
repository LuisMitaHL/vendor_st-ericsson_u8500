#Get the path to this makefile.
MAKEDIR:=$(dir $(lastword $(MAKEFILE_LIST)))

#Should point to a build root (the folder in which e.g. mm/audio/ is present).
SYSROOT ?= ../../../../

all : ilsrv libste_il_proxy.so.1.0.1

%.o : %.cpp
	g++ -O0 -fpic -Wall -Wextra -W -Wshadow -Wconversion -Wcast-qual  -ggdb \
	  -I$(SYSROOT)/mm/shared/omxil/ -I../src -I../x86 -I../include -I../src/db -c $< -o $@

%.o : %.c
	gcc -O0 -fpic -Wall -Wextra -W -Wshadow -Wconversion -Wcast-qual  -ggdb -DADM_LOG_STANDALONE \
	-I$(SYSROOT)/mm/shared/omxil/ -c $< -o $@ -I$(SYSROOT)/mm/audio/include -I$(SYSROOT)/mm/shared/omxil/ \
	-I$(SYSROOT)/mm/audio/afm/proxy/include -I$(SYSROOT)/mm/shared/ens/proxy/include/ \
	-I$(SYSROOT)/mm/audio/audio_chipset_apis/ 

ste_adm_omx_log.o : ../src/ste_adm_omx_log.h ../src/ste_adm_omx_log.c
	gcc -O0 -Wall -Wextra -W -Wshadow -Wconversion -Wcast-qual  -ggdb -DADM_LOG_STANDALONE \
	-I$(SYSROOT)/mm/shared/omxil/ -c ../src/ste_adm_omx_log.c -o $@ -I$(SYSROOT)/mm/audio/include -I$(SYSROOT)/mm/shared/omxil/ \
	-I$(SYSROOT)/mm/audio/afm/proxy/include -I$(SYSROOT)/mm/shared/ens/proxy/include/ \
	-I$(SYSROOT)/mm/audio/audio_chipset_apis/ -I../src -I../x86 -I../include -I../src/db 

ste_il_server.o : ste_il_server.cpp ste_il_server.h ../src/ste_adm_omx_log.h

ste_il_proxy.o : ste_il_client.cpp ste_il_server.h

ilsrv : ste_il_server.o ../x86/il_stubs.o ste_adm_omx_log.o
	g++ -lpthread -o ilsrv ste_il_server.o ste_adm_omx_log.o ../x86/il_stubs.o

libste_il_proxy.so.1.0.1 : ste_il_client.o
	g++ -shared -Wl,-soname,libste_il_proxy.so.1 -o libste_il_proxy.so.1.0.1 ste_il_client.o
	ln -sf libste_il_proxy.so.1.0.1 libste_il_proxy.so.1
	ln -sf libste_il_proxy.so.1.0.1 libste_il_proxy.so

