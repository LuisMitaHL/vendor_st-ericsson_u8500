#ifndef _TEST_H
#define _TEST_H

/************************************************************************
 *                                                                      *
 *  Copyright (C) 2010 ST-Ericsson                                      *
 *                                                                      *
 *  Author: Joakim AXELSSON <joakim.axelsson AT stericsson.com>         *
 *  Author: Sebastian RASMUSSEN <sebastian.rasmussen AT stericsson.com> *
 *                                                                      *
 ************************************************************************/

#include "log.h"

#include <stdlib.h> //exit()
#include <string.h> //strcmp()

#define CHECK(x) \
	do { \
		if ( !(x) ) { \
			logFATAL(&glog, "Check failed: '%s' %s:%d", \
				#x, __FILE__, __LINE__); \
			abort(); \
		} \
	} while(0)

#define CHECK_FAILED(fmt, args...) \
	do { \
		logFATAL("Check failed: " fmt " %s:%d", ##args, __FILE__, __LINE__); \
		exit(-10); \
	} while (0)

#define CHECK_EQUAL(n, m) \
	do { \
		if ( (n) != (m) ) { \
			CHECK_FAILED("'%s'=%d EQUAL '%s'=%d", #n, (n), #m, (m)); \
		} \
	} while(0)

#define CHECK_EITHER(n, m, p) \
	do { \
		if ( (n) != (m) && (n) != (p) ) { \
			CHECK_FAILED("'%s'=%d IS EITHER '%s'=%d OR '%s'=%d", \
				#n, (n), #m, (m), #p, (p), __FILE__, __LINE__); \
		} \
	} while(0)

#define CHECK_NOT_EQUAL(n, m) \
	do { \
		if ( (n) == (m) ) { \
			CHECK_FAILED("'%s'=%d NOT EQUAL '%s'=%d", \
				#n, (n), #m, (m)); \
		} \
	} while(0)

#define CHECK_BIGGER(n, m) \
	do { \
		if ( (n) <= (m) ) { \
			CHECK_FAILED("'%s'=%d BIGGER '%s'=%d", \
				#n, (n), #m, (m)); \
		} \
	} while(0)

#define CHECK_LESS(n, m) \
	do { \
		if ( (n) >= (m) ) { \
			CHECK_FAILED("'%s'=%d LESS '%s'=%d", \
				#n, (n), #m, (m)); \
		} \
	} while(0)

#define CHECK_BIGGER_OR_EQUAL(n, m) \
	do { \
		if ( (n) < (m) ) { \
			CHECK_FAILED("'%s'=%d BIGGER_OR_EQUAL '%s'=%d", \
				#n, (n), #m, (m)); \
		} \
	} while(0)

#define CHECK_STR_EQUAL(str1, str2) \
	do { \
		if ( strcmp((str1), (str2)) != 0 ) { \
			CHECK_FAILED("%s='%s' EQUAL %s='%s'", \
				#str1, (str1), #str2, (str2)); \
		} \
	} while(0)

#endif // _TEST_H
