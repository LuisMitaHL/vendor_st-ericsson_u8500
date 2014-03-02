/*
 * util.cpp
 *
 *  Created on: Jun 15, 2009
 *      Author: emicroh
 */

#include "util.h"

int default_cbuf_len = DEFAULT_BUF_LENGTH;
char *cbuf = NULL;

void setCharBufferLength(int len) {
	if ((cbuf == NULL) || (len != default_cbuf_len)) {
		delete cbuf;
		default_cbuf_len = len;
		cbuf = new char[default_cbuf_len];
	}
}

char * vgetCharString(const char *fString, ...) {
	va_list args;
	va_start(args, fString);
	char * res = vgetCharString(fString, args);
	va_end(args);
	return res;
}

char * vgetCharString(const char *fString, va_list args) {
	if (default_cbuf_len != DEFAULT_BUF_LENGTH) setCharBufferLength(DEFAULT_BUF_LENGTH);
	vsprintf(cbuf, fString, args);
	return cbuf;
}

char * vgetCharString(unsigned int bufLength, const char *fString, va_list args) {
	setCharBufferLength(bufLength);
	vsprintf(cbuf, fString, args);
	return cbuf;
}
