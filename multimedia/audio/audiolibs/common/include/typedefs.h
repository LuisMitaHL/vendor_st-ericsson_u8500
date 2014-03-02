/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _mmdsp_types_h_
#define _mmdsp_types_h_

typedef unsigned char UWord8;
typedef signed char Word8;

typedef long long WORD56;
typedef long long Word56;

typedef long long WORD40;
typedef long long Word40;

typedef int WORD24;
typedef int Word24;

typedef unsigned int UWORD24;
typedef unsigned int UWord24;

#ifndef __flexcc2__
typedef long long WORD48;
typedef long long Word48;

typedef unsigned long long UWORD48;
typedef unsigned long long UWord48;

/* Warning, this does not reflect the architecture, but defining
   Word32 as a long long does not yield the correct results in speech
   codecs who expect 32 bits. Same for Word16. So here we rely on
   implicit casts when calling the bit-true library functions, who
   expect 32/64 bit integers */
#else // __flexcc2__
typedef long WORD48;
typedef long Word48;

typedef unsigned long UWORD48;
typedef unsigned long UWord48;
#endif // __flexc22__ 

typedef long WORD32;
typedef long Word32;

typedef short WORD16;
typedef short Word16;

/* Define byte type for non-windows platforms*/
typedef char BYTE;
typedef unsigned char UBYTE;
typedef unsigned int   UCHAR;
typedef int            SHORT;
typedef unsigned int   USHORT;

#endif /* _mmdsp_types_h_ */


