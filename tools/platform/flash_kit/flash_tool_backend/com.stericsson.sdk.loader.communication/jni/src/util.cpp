/*
 * util.cpp
 *
 *  Created on: Feb 9, 2011
 *      Author: localtester
 */

#include "util.h"
#include "javadefs.h"
#include <cstdlib> //it's Deprecated
#include <ctype.h> //it's Deprecated
#include <iostream>
#include <sstream>
#include <iomanip>
#include "Logger.h"

using namespace std;

char cbuf[DEFAULT_BUFF_LENGTH];
char logBuffer[DEFAULT_BUFF_LENGTH];

void null_fn(){
	throw "Function didn't load properly from dynamical library.";
}

//Deprecated
char* formatMessage(const char * format, ...) {
	va_list args;
	va_start(args, format);
	vsnprintf(&logBuffer[0], DEFAULT_BUFF_LENGTH, format, args);
	va_end(args);
	return &logBuffer[0];
}

void hexFormat(ostringstream &oss, const unsigned char buff[], int sz, int width){
	int y = sz / width;
	int w = sz % width;
	int i;
	int x;
	char c;

	oss << "[" << sz << "=0x" << hex << sz;
	oss << " bytes]\n";

	for (i = 0; i < y; i++) {
		for (x = 0; x < width; x++) {
			oss << hex << setw(2) << setfill('0') << int(buff[i * width + x]) << " ";
		}
		oss << "   ";
		for (x = 0; x < width; x++) {
			c = buff[i * width + x];
			oss << ((isprint(c)) ? c : '.');
		}
		oss << "\n";
	}
	for (x = 0; x < w; x++) {
		oss << hex << setw(2) << setfill('0') << int(buff[i * width + x]) << " ";
	}
	for (; x < width; x++){
		oss << "   ";
	}
	oss << "   ";
	for (x = 0; x < w; x++) {
		c = buff[i * width + x];
		oss << ((isprint(c)) ? c : '.');
	}
}

void debugBuffer(const unsigned char buff[], int sz, int width) {
	ostringstream oss;
	oss << endl;
	hexFormat(oss, buff, sz, width);
	Logger::getInstance()->debug(LP,oss.str());
}

void debugBuffer(const unsigned char buff[], int sz, int width, const std::string &name) {
	ostringstream oss;
	oss << endl << name;
	hexFormat(oss, buff, sz, width);
	Logger::getInstance()->debug(LP,oss.str());
}
