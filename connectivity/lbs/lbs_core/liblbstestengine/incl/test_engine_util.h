/*
 * Positioning Manager
 *
 * test_engine_util.h
 *
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */


#ifndef __TESTENGINE_UTIL_H__
#define __TESTENGINE_UTIL_H__

unsigned long TSTENG_StrToUint32(const char *const String_p);
signed long TSTENG_StrToSint32(const char *const String_p);
int TSTENG_StrNZCpy(char *const Dest_p, const char *const Source_p, const int DestLimit);
int TSTENG_CaseInsensitiveStrNCompare(const char *const String1_p, const char *const String2_p, const int MaxCharsToCompare);
int TSTENG_CaseInsensitiveStrCompare(const char *const String1_p, const char *const String2_p);


#endif
