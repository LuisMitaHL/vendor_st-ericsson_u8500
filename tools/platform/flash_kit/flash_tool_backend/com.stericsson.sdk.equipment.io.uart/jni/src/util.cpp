/*
 * util.cpp
 *
 *  Created on: Dec 9, 2009
 *      Author: emicroh
 */
#include <fstream>
#include "util.h"

using namespace std;

TCHAR logBuffer[LOG_BUF_LENGTH];
ofstream of;

void dump(const unsigned char * buff, int len, bool out) {
	if (!of.is_open()) {
		of.open("dump.dat", ios_base::out | ios_base::trunc);
		if (!of)
			throw "Can not open dump file";
	}

	if (out)
		of << "OUT:\n";
	else
		of << "IN:\n";
	int i;

	// remember stream state to restore at the end, required by CoverityPrevent
	char oldFill = of.fill();
	ios_base::fmtflags oldFlags = of.flags();

	for (i = 0; i < len; i++) {
		of.width(2);
		of.fill('0');

		of << hex << (int) (buff[i]) << " ";
		if ((i + 1) % 8 == 0)
			of << endl;
	}
	if ((len % 8) != 0)
		of << endl;

	of.fill(oldFill);
	of.flags(oldFlags);
}


char * getMessage(const char * fstring, ...) {
	va_list args;
	va_start(args, fstring);
	snprintf(logBuffer, LOG_BUF_LENGTH, fstring, args);
	va_end(args);
	return logBuffer;
}
