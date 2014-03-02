/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


/**
 * \file types.h
 * \brief Definitions of global custom data types and data structures.
 * \author Filippo Santinello
 * \author Denis Zinato
 *
 * Here are the custom data types for the various integer types
 * and structs for both host and HAMAC sides.
 */

#ifndef TYPES_H
#define TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

// definition comes from STD for source file like main
// Otherwise all external definition are here
#ifndef NULL
typedef unsigned int size_t;
void *local_malloc(size_t size);
void local_free(void *ptr);
void *local_realloc(void *ptr, size_t size);
void *memset(void *s, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
//double ceil(double x);

#ifdef DEBUG_TRACES
#define printf NMF_LOG
#else
static inline void printf(const char *fmt, ...) { }
#endif

#define NULL (void*) 0
#define EXIT_FAILURE -1
#endif

#include "settings.h"



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

#endif



#ifndef  _T_ADDRESS_

typedef unsigned long   t_address;  /**< Generic pointer */
#endif

#ifdef HCL_STARTUP
#include "hcl_defs.h"
#endif


/**
 * \brief Structure containing blocks info for decoding
 * This structure defintiion must be the same as the one
 * in the Hamac DSP part , so 4 field are added 
 * this field are uselless for refcode
 */

#ifdef NO_HAMAC     

typedef struct 
{
    t_sint16 nslice;  /* -1 not decoded */
    t_uint16 non_zero[3];  /* Luma, Cb, Cr */
    t_sint16 type;
    t_uint16 I4x4_type;
    t_sint16 refidx;
    t_sint16 mv[2];
    t_uint16 QP[2];   /* Luma, Chroma */
    t_uint16 concealed;
} t_block_info;

#else
// Different implementation for HAMAC or full SW
// HW one gain size 
typedef struct
{
  t_uint16 non_zero;
  t_uint16 BKType;
  t_sint16 mv[2];
} t_block4x4_info;

typedef struct
{
  t_sint16 nslice;
  t_uint16 concealed;
  t_uint16 QP[2];
  t_uint16 s_info_disable_filter;
  t_sint16 s_info_alpha_c0_offset_div2;
  t_sint16 s_info_beta_offset_div2;
  t_sint16 reserved_1;
  t_block4x4_info block4x4_info[16];
} t_macroblock_info;

typedef t_macroblock_info t_block_info ;
#endif


/**
 * \brief Structure containing all the needed parameters and pointer to handle a bitstream.
 */
/* interface buffer comes from NMF */

#define VCL_UNDER_PROCESSING	1	
#define NONVCL_UNDER_PROCESSING	2

#ifdef NMF_BUILD
#include <h264enc/arm_nmf/api/nmftype.idt> 
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

/**
 * \brief Interface struct used to pass start and end addresses of the bitstream buffer.
 */
typedef struct
{
    t_address start;    /**< address of the first byte of the bitstream buffer */
    t_address end;      /**< address of the last byte of the buffer */
    
} t_interface_buffer;


/**
 * \brief Interface struct used to pass the current byte position and the bit offset of the bitstream buffer.
 */
 
typedef struct
{
    t_address addr;     /**< address of the byte currently read of the bitstream buffer */
    t_uint16 os;      /**< bit offset with respect to the current byte */
    
} t_interface_buffer_pos;


#if (ST_LITTLE_ENDIAN == 0) & (DEBLOCK_HW_MODEL == 1)

/**
 * \brief Structures used for deblocking filter.
 *  Must be used only for Emulation of DBLK 
 *  not for HAMAC code generation 
 */

typedef struct 
{
    unsigned _0 : 2;  unsigned A_l : 6;
    unsigned _1 : 2;  unsigned B_l : 6;
    unsigned _2 : 2;  unsigned A_c : 6;
    unsigned _3 : 2;  unsigned B_c : 6;

} ts_ab_index;



typedef struct 
{
    unsigned _0 : 2;  unsigned h0 : 3; unsigned v0 : 3;
    unsigned _1 : 2;  unsigned h1 : 3; unsigned v1 : 3;
    unsigned _2 : 2;  unsigned h2 : 3; unsigned v2 : 3;
    unsigned _3 : 2;  unsigned h3 : 3; unsigned v3 : 3;

} ts_strength;



typedef struct 
{
    ts_ab_index     index[3];
    t_uint32        loc;
    ts_strength     bs[4];

} tps_h4d_param;

#else
typedef struct 
{
    t_uint32     index[3];
    t_uint32        loc;
    t_uint32     bs[4];

} tps_h4d_param;



#endif


#ifdef __cplusplus
}
#endif

#endif
