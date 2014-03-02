/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _DSP_PRINTF_
#define _DSP_PRINTF_
#include <stdarg.h>

static void local_printf(char *format,...)
{
	va_list	ap;
	va_start(ap,format);
    if (! IS_NULL_INTERFACE(oPrintf, printf)) oPrintf.printf(format,ap);
	va_end(ap);
 
}
#define printf	local_printf

#endif
