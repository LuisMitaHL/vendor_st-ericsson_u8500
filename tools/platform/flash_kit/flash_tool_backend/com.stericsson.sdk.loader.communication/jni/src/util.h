/*
 * util.h
 *
 *  Created on: Feb 9, 2011
 *      Author: localtester
 */

#ifndef UTIL_H_
#define UTIL_H_

#include "jni.h"
#include <string>

#define DEFAULT_BUFF_LENGTH 8192
#define VERSION_BUFF_SIZE 100
#define PROTOCOL_BUFF_SIZE 100
#define RESPONSE_BUFF_SIZE 3000

/* Null function which used to handle failed loaded function */
void null_fn();

/* Function for formating string from given format string and variable argument list. */
char* formatMessage(const char * format, ...);

/* Function for print buffer to debug log*/
void debugBuffer(unsigned const char buff[], int sz, int width);
void debugBuffer(const unsigned char buff[], int sz, int width, const std::string &name);

#endif /* UTIL_H_ */
