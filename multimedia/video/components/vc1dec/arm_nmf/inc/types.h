/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */



#ifndef TYPES_H
#define TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
extern "C" typedef unsigned int size_t;
extern "C" void *memcpy(void *dest, const void *src, size_t n);
#endif


// definition comes from STD for source file like main
// Otherwise all external definition are here
#ifndef NULL
typedef unsigned int size_t;
void *local_malloc(size_t size);
void local_free(void *ptr);
void *local_realloc(void *ptr, size_t size);
//void *memset(void *s, int c, size_t n);
//void *memcpy(void *dest, const void *src, size_t n);
//double ceil(double x);
#define printf NMF_LOG
#define fatal_error(s) NMF_PANIC(s)
//#define printf decoder_msg

#define NULL (void*) 0
#define EXIT_FAILURE -1
#endif

#ifdef __SYMBIAN32__
    #include <armnmf_dbc.h>
    #define printf NMF_LOG
    #define fatal_error(s) NMF_PANIC(s)
    IMPORT_C void* memset(void* aTrg, int aValue, unsigned int aLength);
    IMPORT_C void* memcpy(void* aTrg, const void* aSrc, unsigned int aLength);
#else
    //#include "string.h"
    #include "stdlib.h"
    #define printf NMF_LOG
    #define fatal_error(s) NMF_PANIC(s)
#endif

#include "settings.h"

typedef unsigned char * t_img_pel;

#ifndef NMF_BUILD
#define METH(a)	a
#endif


#ifndef _COMMON_TYPE_H_
#define _HCL_DEFS_H
typedef unsigned char t_bool;

#ifndef NMF_TYPEDEF_H_
typedef unsigned long long t_uint64;
typedef signed long long t_sint64;

typedef unsigned long   t_uint32;     /**< Unsigned 32-bit integer */
typedef unsigned short  t_uint16;     /**< Unsigned 16-bit integer */
typedef unsigned char   t_uint8;      /**< Unsigned 8-bit integer */
typedef long            t_sint32;     /**< Signed 32-bit integer */
typedef short           t_sint16;     /**< Signed 16-bit integer */
#ifndef HCL_STARTUP
typedef char            t_sint8;      /**< Signed 8-bit integer */
#endif
typedef t_uint32 t_physical_address;
#endif
#endif // NMF_TYPEDEF_H_


#ifndef  _T_ADDRESS_
typedef unsigned long   t_address;  /**< Generic pointer */
#endif

#ifdef HCL_STARTUP
#include "hcl_defs.h"
#endif

//void fatal_error(char *);
//int decoder_msg(const char* format, ...);
//void NMF_PANIC(char *);
//int NMF_LOG(const char* format, ...);

#define VCL_UNDER_PROCESSING     1
#define NONVCL_UNDER_PROCESSING  2

#ifdef NMF_BUILD
#include <vc1dec/arm_nmf/api/nmftype.idt> 
#else
typedef struct
{
    t_uint8 *addr;   /**< current read byte address */
    t_uint8 *start;  /**< address of the first byte of the buffer */
    t_uint8 *end;    /**< address of the last byte of the buffer */
    t_uint16 os;     /**< bit offset inside the current byte */
    t_uint16 inuse;  /**< marker to know if buffer is currenlty under processing by HW or/and by SW */
    
} t_bit_buffer;
#endif

#ifdef __cplusplus
}
#endif

#endif
