# This makefile goal is to build a small test application for the ModemBufferList class
# If it reaches the end of the main function, the test is ok. 

ModemBufferTest: src/ModemBufferList.cpp inc/ModemBufferList.hpp 
	g++ -D TEST -O0 -g src/ModemBufferList.cpp -o ModemBuffertest -I$(MM_HEADER_BASEDIR)/shared/omxil -Iinc -lc -lpthread

all: ModemBufferTest