/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef NMF_LIB_H
#define NMF_LIB_H

#if ((!defined __SYMBIAN32__) && (!defined IMPORT_C))
    #define IMPORT_C
#endif
#if ((!defined __SYMBIAN32__) && (!defined EXPORT_C))
    #define EXPORT_C
#endif

#ifdef __SYMBIAN32__
#include <armnmf_dbc.h>
#else
#include <los/api/los_api.h>
#endif

extern "C" IMPORT_C void NMF_PANIC(const char* fmt, ...);
extern "C" IMPORT_C void NMF_LOG(const char* fmt, ...);
extern "C" IMPORT_C void* memset(void* aTrg, int aValue, unsigned int aLength);
extern "C" IMPORT_C void* memcpy(void* aTrg, const void* aSrc, unsigned int aLength);

#endif
