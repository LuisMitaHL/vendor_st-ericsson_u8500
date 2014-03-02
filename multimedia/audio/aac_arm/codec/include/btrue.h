/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _btrue_h_
#define _btrue_h_


#ifndef	_audiolibs_common_h_ // to avoid recrusive inclusion of audiolibs_common.h

//#include <stwdsp.h>
#include <stdlib.h> //ESRAM_ALLOC inclusion
#include "audiolibs_types.h"
#include <math.h>
#ifdef BUILD_FIXED
/* Define Float type as integer or floating-point */
typedef int   Float;
#ifndef __flexcc2__
typedef long long dFloat;
#else  // __flexcc2__
typedef long dFloat;
#endif // __flexcc2__
#else // BUILD_FIXED
#ifndef DOUBLE_PRECISION_FLOAT
 typedef float   Float;
 typedef float  dFloat;
#else // DOUBLE_PRECISION_FLOAT
 typedef double   Float;
 typedef double  dFloat;
#endif // DOUBLE_PRECISION_FLOAT
#endif // BUILD_FIXED

//#include "audiolibs_common.h"
 #ifdef __cplusplus
#define CPPCALL "C"
#else
#define CPPCALL
#endif

#ifndef AUDIO_API_IMPORT
	#ifdef __SYMBIAN32__
		#define AUDIO_API_IMPORT IMPORT_C
	#else
		#define AUDIO_API_IMPORT
	#endif
#endif
#ifndef AUDIO_API_EXPORT
	#ifdef __SYMBIAN32__
		#define AUDIO_API_EXPORT EXPORT_C
	#else
		#define AUDIO_API_EXPORT
	#endif
#endif

#endif //_audiolibs_common_h_

#include<typedefs.h>

#ifndef MMDSP

#include <stdio.h>

#define bit_field(pos, size)	((pos & 0x1f) | ((size & 0x1f) << 8))
#define cmove(cond, a, b)	    (a = (cond) ? (b) : (a))


/* definitions for loop counter */
#define MAX_NESTED_LOOP 500 // no limit of nested loop in non mmdsp mode
#if 0
#define loop(a)   _lct++; \
if (_lct>MAX_NESTED_LOOP) \
{ fprintf(stderr,"Too much nested loops\n");exit(1);}\
for(_ct[_lct]=a;_ct[_lct]>0 || (_lct--==-1);_ct[_lct]--)
#else
#define loop(a)   _lct++; \
for(_ct[_lct]=a;_ct[_lct]>0 || (_lct--==-1);_ct[_lct]--)
#endif
extern int             _lct;
extern int	       _ct[1 + MAX_NESTED_LOOP];	
extern int             _lstack[100];
extern int             _lsp;
extern int             _Fcarry;

extern int             wextract(int a, int b);
extern int             wextractu(int a, int b);
extern int             wmerge(int a, int b);
extern int             winsert(int a,int b,int  r2l);
extern void            *mod_add(void *reg_value, int index,
                              void *Max_value, void *Min_value);
//extern Float          *br_add( Float *ad, int index, Float *base);
extern void 		   *br_add(void *ad, int index, void *base);

extern int             wasl(int a, int shift);
extern int             wasr(int a, int shift);
extern int             wlsr(int a, int shift);
extern CPPCALL AUDIO_API_IMPORT int             winterval(int a, int maxval);
extern int             wxor(int a, int b);
extern int             wmaxm(int a, int b);
extern int             wmin(int a, int b);
extern int             wmax(int a, int b);
extern int             wtstcarry(void);
extern WORD56          wX_asl(WORD56 val, int shift);
extern WORD56          winsert_l(WORD56 a, int b);
extern WORD56          winsert_h(WORD56 a, int b);
extern int             wextract_l(WORD56 a);
extern int             wextract_h(WORD56 a);


extern CPPCALL AUDIO_API_IMPORT int             waddsat(int a, int b);
extern unsigned int		winsertu(int a,int b,int  r2l);


#define cmoveifcarry(a, b)	(a = _Fcarry ? (b) : (a))

#ifdef ARM
/* some bit-true functionalities are ported on 32-bit ARM
 * for ease of portability and efficient than x86 implementation.
 * some simple functions are replaceed with Macros and defined 
 * in cortexa9_common.h */
int wextract_arm(int a,int b);

int wextractu_arm(int a,int b);

unsigned int winsertu_arm(int a,int b,int  r2l);

int winsert_arm(int a,int b,int  r2l);

int	 wlsr_arm(int a,int b);

int  wextract_l_arm(WORD56 a);

#endif // ARM

#endif /* MMDSP */

#endif /* _btrue_h_ */
