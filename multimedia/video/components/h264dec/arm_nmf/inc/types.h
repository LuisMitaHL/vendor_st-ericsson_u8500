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
#define fatal_error(s) NMF_PANIC(s)
#define NULL (void*) 0
#define EXIT_FAILURE -1
#endif

#define fatal_error(s) NMF_PANIC(s)
#include "nmf_lib.h"
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
#ifdef __ndk8500_ed__
/* H.264 macroblock info structure is define on STn8500ED as CUP H.264 MB_context */
typedef struct
{
   t_sint16 x;
   t_sint16 y;
} t_motion_vector;
		
typedef struct
{
   t_uint8  mb_type;
   t_uint8  CBP;
   t_uint8  QPy;
   t_uint8  QPc;
   t_sint16 nslice;
   t_uint8  conceal;
   t_uint8  error;
   t_uint8  soa;
   t_uint8  sob;
   t_uint8  dis_filter;
   t_uint8  free;
   t_uint8  ref_pic[4];
   t_uint8  I4x4PredMode[16];
   t_uint8  NZCoeff[24];
   t_uint8  sub[4];
   t_uint8  RefIdx[4];
   t_motion_vector MV[4][4];   
} t_macroblock_info;
#else // ndk20 or 8500_a0

#ifdef __ndk8500_a0__ 
/* H.264 macroblock info structure for CUP context */
typedef struct
{
   t_sint16 x;
   t_sint16 y;
} t_motion_vector;
		
typedef struct
{
   t_uint8  mb_type;
   t_uint8  QPy;
   t_uint8  QPcb;
   t_uint8  QPcr;
   t_sint16 nslice;
   t_uint8  user;
   t_uint8  error;
   
   t_uint16 I4x4PredMode[4];
   
   t_uint8  ref_pic[8];
   
   t_uint8  RefIdx[8];
   
   t_uint8  NZCoeff[24];
   
   t_uint8  sub[4];
   t_uint32 shp;
   
   t_motion_vector MV[16][2]; 

} t_macroblock_info;
#else
#if defined __ndk5500_a0__
/* H.264 macroblock info structure for CUP context */
typedef struct
{
    t_sint16 x;
   t_sint16 y;
} t_motion_vector;

typedef struct
{
   t_uint8  sub;      /*subTy*/
   t_uint8  RefIdx; 
   t_uint8  ref_pic;
   t_uint8  mv[13];  /*4 MV x:14bits, y:12bits = 4x26 bits*/
} t_subpart;
			
typedef struct
{
   t_uint8  mb_type;
   t_sint16 nslice;
   t_uint8  QPy;
   t_uint8  QPcb;
   t_uint8  QPcr;
   t_uint8  error;
   t_uint8  ICP;

   t_uint32 intra_dir[2];
   t_uint8 NZCoeff[15]; //???Luma 16x5bits + Chroma 8x5bits
   t_uint8 CBP;

   t_uint8 sao;
   t_uint8 sbo;
   t_uint8 ddf;
   t_uint8 slc_t;
   t_uint8 unused[12];

   t_subpart  subpartL0[4];
   t_subpart  subpartL1[4];

   t_uint8 PxY_bottom[16];
   t_uint8 PxY_right[16];
   t_uint8 PxCb_bottom[8];
   t_uint8 PxCb_right[8];  
   t_uint8 PxCr_bottom[8];
   t_uint8 PxCr_right[8];  
} t_macroblock_info;
#else // ndk20
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
#endif 
#endif 
#endif
typedef t_macroblock_info t_block_info ;
#endif


/**
 * \brief Structure containing all the needed parameters and pointer to handle a bitstream.
 */
/* interface buffer comes from NMF */

#define VCL_UNDER_PROCESSING	1	
#define NONVCL_UNDER_PROCESSING	2

#ifdef NMF_BUILD
#include <arm_codec_type.idt> 
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

#ifdef __ndk8500_ed__
typedef struct 
{
   unsigned qpc2 :6;
   unsigned qpc  :6;
   unsigned sob  :4; 
   unsigned qpl2 :6;
   unsigned qpl  :6;
   unsigned soa  :4;
   t_uint32 reserved :32;
   unsigned bs_v00 :2;
   unsigned bs_v01 :2;
   unsigned bs_v02 :2;
   unsigned bs_v03 :2;
   unsigned bs_v10 :2;
   unsigned bs_v11 :2;
   unsigned bs_v12 :2;
   unsigned bs_v13 :2;
   unsigned bs_v20 :2;
   unsigned bs_v21 :2;
   unsigned bs_v22 :2;
   unsigned bs_v23 :2;
   unsigned bs_v30 :2;
   unsigned bs_v31 :2;
   unsigned bs_v32 :2;
   unsigned bs_v33 :2;
   unsigned bs_h00 :2;
   unsigned bs_h01 :2;
   unsigned bs_h02 :2;
   unsigned bs_h03 :2;
   unsigned bs_h10 :2;
   unsigned bs_h11 :2;
   unsigned bs_h12 :2;
   unsigned bs_h13 :2;
   unsigned bs_h20 :2;
   unsigned bs_h21 :2;
   unsigned bs_h22 :2;
   unsigned bs_h23 :2;
   unsigned bs_h30 :2;
   unsigned bs_h31 :2;
   unsigned bs_h32 :2;
   unsigned bs_h33 :2;
} tps_h4d_param;

#else // ndk20

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

#endif

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


