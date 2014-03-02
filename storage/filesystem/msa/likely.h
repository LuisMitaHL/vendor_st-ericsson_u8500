#ifndef _LIKELY_H_
#define _LIKELY_H_

/************************************************************************
 *                                                                      *
 *  Copyright (C) 2010 ST-Ericsson                                      *
 *                                                                      *
 *  Author: Joakim AXELSSON <joakim.axelsson AT stericsson.com>         *
 *  Author: Sebastian RASMUSSEN <sebastian.rasmussen AT stericsson.com> *
 *                                                                      *
 ************************************************************************/


/**********************************************
 * Macros
 **********************************************/

#ifdef __GNUC__
	#define likely(x)	__builtin_expect(!!(x), 1)
	#define unlikely(x)	__builtin_expect(!!(x), 0)
#else
	#warning "Not using GCC C compiler, likely()/unlikely() will lose their meaning"
	#define likely(x)   x
	#define unlikely(x) x
#endif

#endif // _LIKELY_H_

