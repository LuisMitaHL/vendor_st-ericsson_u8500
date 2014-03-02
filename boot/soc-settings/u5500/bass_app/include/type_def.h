/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
	type_def.h
 */

#ifndef TYPE_DEF_H
#define TYPE_DEF_H


typedef unsigned char bool8; /* Bool8: FALSE, TRUE */
typedef unsigned char byte; /* Byte: 0 - FFH */
typedef unsigned short int word; /* Word: 0 - FFFFH */
typedef unsigned long int dword; /* Dword: 0 - FFFFFFFFH */
typedef unsigned char uint8; /* Uint8 0 ... 255 */
typedef unsigned short int uint16; /* Uint16:0 ... 65535 */
typedef unsigned long int uint32; /* Uint32:0 ... 4294967295 */
typedef unsigned long long uint64; /* Uint64 0.. */
typedef signed char int8; /* Int8: -128 ... +127 */
typedef signed short int int16; /* Int16: -32768 ... +32767*/
typedef signed long int int32; /* Int32: -2147483650 ... */
typedef unsigned long int KEY_TYPE_T; /* Uint32:0 ... 4294967295 */

#if !defined LOCAL
#define LOCAL static
#endif



#endif

