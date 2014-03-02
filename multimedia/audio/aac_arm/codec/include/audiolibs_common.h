/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef	_audiolibs_common_h_
#define _audiolibs_common_h_
//#include <stwdsp.h>
#include <stdlib.h> //ESRAM_ALLOC inclusion
#include "audiolibs_types.h"

#define AT_REG(a)

#ifndef _btrue_h_
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
#endif //_btrue_h_

#ifdef __flexcc2__
//--------------------------------------
// AC COMPILER
//--------------------------------------
#define MMDSP_NATIVE_ACUMODE 0x2000 // Force use of 24-bit ACU */

#define wpushloop()
#define wpoploop()
#define cmove(cond, dest, val) ((cond)?((dest)=(val)):((dest)=(dest)))
#define mod_add(a,b, max_adr, min_adr) ((a)+(b)) 										

#define CLR_ACC(a)  ((a)=wX_sub((a),(a)));

#define FORCE_DCUMODE16()  
#define FORCE_DCUMODE24()
#define PRAGMA_FORCE_DCUMODE    _Pragma("force_dcumode")

#define AUDIOLIBS_UPLOAD_MEM __EXTERN
#define WINDOW_MEM __EXTERN
#define SQRT_FRACT __EXTERN
#define MDCT_MEM   __EXTERN
#define AUDIO_CONST_MEM __EXTERN
#define EXTERN __EXTERN
#define EXTERN16 __EXTERN16
#define EXTERN24 __EXTERN24
#ifndef __esram__
#define __ESRAM __EXTERN
#endif

#else // __flexcc2__
//--------------------------------------
// ! AC COMPILER (rvct, gcc)
//--------------------------------------

#define MMDSP_NATIVE_ACUMODE  0x0  // Keep existing configuration in adctl
#define __circ

#ifdef MMDSP
#define CLR_ACC(a)  (a=0,(a)=wX_sub((a),(a))); // removes valgrind warnings

#define FORCE_DCUMODE16()  wmode16()
#define FORCE_DCUMODE24()  wmode24()

#else // MMDSP
#define CLR_ACC(a)  ((a)=0);

#define FORCE_DCUMODE16()
#define FORCE_DCUMODE24()

#endif // MMDSP

#define PRAGMA_FORCE_DCUMODE  

#define at_reg(a)

#define YMEM
#define EXTMEM
#define WAITMEM

#define AUDIOLIBS_UPLOAD_MEM
#define WINDOW_MEM
#define SQRT_FRACT 
#define MDCT_MEM
#define AUDIO_CONST_MEM
#define EXTERN
#define EXTERN16
#define ESRAM

#endif // __flexcc2__



/* standard includes and configuration includes */
#include "stdinc.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define  MAX_BITS          23
#define  MAXVAL            (1L<<MAX_BITS)

#ifndef _btrue_h_ // to avoid recrusive inclusion of btrue.h
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
#endif//_btrue_h_

#ifdef BUILD_FIXED

#ifdef MMDSP
#undef FORMAT_FLOAT_WITH_DOUBLES
#endif // MMDSP


#if defined(FORMAT_FLOAT_WITH_DOUBLES)
	
#define FORMAT_FLOAT(value,normval)      												\
( ( (double)(value) >=(double) 0) ?                                           			\
((( (double)(value) *(double) (normval) + (double)0.5 ) >= (double) (normval) ) ?      	\
   (int)( (normval) - 1) : (int)((double) (value) *(double) (normval) +(double) 0.5)) 	\
:                                                             							\
((((double) (value) *(double) (normval) - (double)0.5) <= -(double)(normval) ) ?        \
    (int)( -(normval) ) :  (int)((double) (value) *(double) (normval) -(double) 0.5)) 	\
)

#else // FORMAT_FLOAT_WITH_DOUBLES

#define FORMAT_FLOAT(value,normval)      												\
( ( (float)(value) >=(float) 0) ?                                           			\
((( (float)(value) *(float) (normval) + (float)0.5 ) >= (float) (normval) ) ?      	\
   (int)( (normval) - 1) : (int)((float) (value) *(float) (normval) +(float) 0.5)) 	\
:                                                             							\
((((float) (value) *(float) (normval) - (float)0.5) <= -(float)(normval) ) ?        \
    (int)( -(normval) ) :  (int)((float) (value) *(float) (normval) -(float) 0.5)) 	\
)

#endif // FORMAT_FLOAT_WITH_DOUBLES

#else // BUILD_FIXED

#define FORMAT_FLOAT(value,normval) ((Float)(value))

#endif // BUILD_FIXED


/** array alignment for NEON **/
#ifdef __arm__

#ifdef __CC_ARM

 /* COMPILATION ISSUE
 *  Problem with --arm_linux flag, __align gets defined to __alignx
 * Compiler can not find definition for __alignx. Need to resolve issue with ARM
 * #define ALIGN_BY_8BYTE __align(8)
 *#define ALIGN_BY_16BYTE __align(16)
 */
#define ALIGN_BY_8BYTE __attribute__ ((aligned(8)))
#define ALIGN_BY_16BYTE __attribute__ ((aligned(16)))
#define GNU_ALIGN_BY_8BYTE 
#define GNU_ALIGN_BY_16BYTE  
#else
#define ALIGN_BY_8BYTE   
#define ALIGN_BY_16BYTE   
#define GNU_ALIGN_BY_8BYTE __attribute__ ((aligned(8)))
#define GNU_ALIGN_BY_16BYTE  __attribute__ ((aligned(16)))
#endif

#else /* __arm__ */

#define ALIGN_BY_8BYTE   
#define ALIGN_BY_16BYTE     
#define GNU_ALIGN_BY_8BYTE 
#define GNU_ALIGN_BY_16BYTE  

#endif /* __arm__ */

/* include types */
#include "typedefs.h"
#include "btrue.h"

/* Make sure HWLOOP is always defined */
#ifndef MMDSP 
#ifdef ARM
#define __lcount(a) __lcount ## a
#define __liter(a) __liter ## a

#define HWLOOP(a) { int __lcount(__LINE__); int __liter(__LINE__);\
for((__liter(__LINE__) = (a)), (__lcount(__LINE__)) = 0; \
  (__lcount(__LINE__)) < (__liter(__LINE__)); (__liter(__LINE__))--) 
#else
#define  HWLOOP(a) { loop(a)                     
#endif
#define END_HWLOOP };
#endif // MMDSP


/* Make sure INLINE is defined */
#ifndef INLINE
#define INLINE
#endif /* INLINE */


#if defined(UNROLL_LOOPS) && defined(__flexcc2__) 
#warning on y est
/* gcc does not like this any longer from version 3.2 on */
#define REPEAT_2(body)  body##body
#define REPEAT_3(body)  body##body##body
#define REPEAT_4(body)  body##body##body##body
#define REPEAT_5(body)  body##body##body##body##body
#define REPEAT_6(body)  body##body##body##body##body##body
#define REPEAT_7(body)  body##body##body##body##body##body##body
#define REPEAT_8(body)  body##body##body##body##body##body##body##body

#else

#define REPEAT_2(body)  HWLOOP ( 2 ) { body } END_HWLOOP;
#define REPEAT_3(body)  HWLOOP ( 3 ) { body } END_HWLOOP;
#define REPEAT_4(body)  HWLOOP ( 4 ) { body } END_HWLOOP; 
#define REPEAT_5(body)  HWLOOP ( 5 ) { body } END_HWLOOP;
#define REPEAT_6(body)  HWLOOP ( 6 ) { body } END_HWLOOP;
#define REPEAT_7(body)  HWLOOP ( 7 ) { body } END_HWLOOP;
#define REPEAT_8(body)  HWLOOP ( 8 ) { body } END_HWLOOP;

#endif /* UNROLL_LOOPS */



/* definition of overflow flags */
#ifndef _REENTRANT
#define ASSERT_FOVERFLOW_SET          assert( Foverflow == 1)
#define ASSERT_FOVERFLOW_CLEAR        assert( Foverflow == 0)
#define ASSERT_FMOVERFLOW_SET         assert( Fmoverflow == 1)
#define ASSERT_FMOVERFLOW_CLEAR		  assert( Fmoverflow == 0)
#else
#define ASSERT_FOVERFLOW_SET          assert( FOVERFLOW == 1) 
#define ASSERT_FOVERFLOW_CLEAR		  assert( FOVERFLOW == 0) 
#define ASSERT_FMOVERFLOW_SET		  assert( FMOVERFLOW == 1)
#define ASSERT_FMOVERFLOW_CLEAR		  assert( FMOVERFLOW == 0)
#endif /* _REENTRANT */

/* values returned by initialization functions */
typedef enum
{
	INIT_OK,
    INIT_WARNING_NO_ESRAM,
	INIT_FAILURE_MALLOC,
	INIT_FAILURE_CONFIGURE,
    INIT_BAD_MEMORY_PRESET,
	INIT_INVALID_VALUE,
	INIT_ERROR = INIT_FAILURE_MALLOC 
} CODEC_INIT_T;


/* location of audiotables */
#define DESC_MEM /* X by default */



/****************************************************************/
/* Value to configure memory preset				*/
/****************************************************************/
/*#ifdef _NMF_MPC_ */
#include "memorybank.idt"
typedef enum 
{
    TCM   = MEM_XTCM,
    YTCM  = MEM_YTCM,
    DDR   = MEM_DDR24,
    DDR16 = MEM_DDR16,
    ESR   = MEM_ESR24,
    ESR16 = MEM_ESR16,
    /* must always be the last element*/
    BANK_COUNT = MEM_BANK_COUNT
} MEMORY_TYPE_T;

/* #else // _NMF_MPC_  */

/* // this enum is used for the pointer of functions  */
/* typedef enum  */
/* { */
/*     TCM, */
/*     DDR, */
/*     DDR16, */
/*     ESR, */
/*     ESR16, */
/*     // must always be the last element */
/*     BANK_COUNT */
/* } MEMORY_TYPE_T; */

/* #endif // _NMF_MPC_ */

typedef enum
{
    MEM_DEFAULT,  	/* same config as default one in 8810 */
    MEM_ALL_DDR,
    MEM_ALL_TCM,
    MEM_MIX_DDR_TCM_1,
    MEM_MIX_DDR_TCM_2,
    MEM_MIX_DDR_TCM_3,
    MEM_MIX_DDR_TCM_4,
    MEM_MIX_DDR_TCM_5,
    MEM_ALL_ESRAM,
    MEM_MIX_ESRAM_DDR,
    MEM_MIX_ESRAM_TCM,
    MEM_MIX_ESRAM_OTHER_1,
    MEM_MIX_ESRAM_OTHER_2,
    MEM_MIX_ESRAM_OTHER_3,
    MEM_MIX_ESRAM_OTHER_4,
    MEM_MIX_ESRAM_OTHER_5,
    ESRAM_BIT = 0x8
} MEMORY_PRESET_T;

/* temporary fixes for afm etc */
#define bs_read_bit_arm       bs_read_bit 
#define bs_copy_bit_arm       bs_copy_bit 
#define bs_fast_move_bit_arm  bs_fast_move_bit

#ifdef USE_24BITS_LIBBITSTREAM
#define bs_byte_align               bs_byte_align_24b
#define bs_copy_bit                 bs_copy_bit_24b
#define bs_fast_move_bit            bs_fast_move_bit_24b
#define bs_fast_move_byte           bs_fast_move_byte_24b
#define bs_fast_move_word           bs_fast_move_word_24b
#define bs_lookup_word_size_bit     bs_lookup_word_size_bit_24b
#define bs_read_bit                 bs_read_bit_24b
#define bs_read_byte                bs_read_byte_24b
#define bs_write_less_than_16_bit   bs_write_less_than_16_bit_24b
#define bs_write_bit                bs_write_bit_24b
#define bs_write_byte               bs_write_byte_24b
#define bs_copy_word                bs_copy_word_24b
#define bs_copy_byte                bs_copy_byte_24b
#define bs_write_word               bs_write_word_24b
#define bs_write_array              bs_write_array_24b
#define bs_read_sample_msb          bs_read_sample_msb_24b
#define bs_read_sample_lsb          bs_read_sample_lsb_24b
#define bs_read_bit_long            bs_read_bit_long_24b
#define bs_read_1bit                bs_read_1bit_24b
#define bs_copy_word_modulo_source  bs_copy_word_modulo_source_24b
#define bs_copy_word_modulo_dest    bs_copy_word_modulo_dest_24b
#define bs_check_byte_aligned       bs_check_byte_aligned_24b
#define bs_byte_align_struct        bs_byte_align_struct_24b
#define bs_copy_samples             bs_copy_samples_24b
#define bs_copy_to_array            bs_copy_to_array_24b
#define bs_read_word                bs_read_word_24b
#define bs_copy_from_array          bs_copy_from_array_24b
#else
#define bs_byte_align               bs_byte_align_32b
#define bs_copy_bit                 bs_copy_bit_32b
#define bs_fast_move_bit            bs_fast_move_bit_32b
#define bs_fast_move_byte           bs_fast_move_byte_32b
#define bs_fast_move_word           bs_fast_move_word_32b
#define bs_lookup_word_size_bit     bs_lookup_word_size_bit_32b
#define bs_read_bit                 bs_read_bit_32b
#define bs_read_byte                bs_read_byte_32b
#define bs_write_less_than_16_bit   bs_write_less_than_16_bit_32b
#define bs_write_bit                bs_write_bit_32b
#define bs_write_byte               bs_write_byte_32b
#define bs_copy_word                bs_copy_word_32b
#define bs_copy_byte                bs_copy_byte_32b
#endif
typedef enum 
{
      AUDIOLIBS_CHANNEL_NONE   = 0,  /**< Unused or empty */
      AUDIOLIBS_CHANNEL_LEFT   = 1,  /**< Left  */
      AUDIOLIBS_CHANNEL_RIGHT  = 2,  /**< Right  */
      AUDIOLIBS_CHANNEL_CENTRE = 3,  /**< Centre */
      AUDIOLIBS_CHANNEL_LS     = 4,  /**< Left Surround */
      AUDIOLIBS_CHANNEL_RS     = 5,  /**< Right Surround  */
      AUDIOLIBS_CHANNEL_LFE1   = 6,  /**< Low Frequency Effects 1 */
      AUDIOLIBS_CHANNEL_Cs     = 7,  /**< Center Surround */
      AUDIOLIBS_CHANNEL_Lsr    = 8,  /**< Left Surround in Rear  */
      AUDIOLIBS_CHANNEL_Rsr    = 9,  /**< Right Surround in Rear  */
      AUDIOLIBS_CHANNEL_Lss    = 10, /**< Left Surround on Side */
      AUDIOLIBS_CHANNEL_Rss    = 11, /**< Right Surround on Side  */
      AUDIOLIBS_CHANNEL_Lc     = 12, /**< Between Left and Centre in front  */
      AUDIOLIBS_CHANNEL_Rc     = 13, /**< Between Right and Centre in front  */
      AUDIOLIBS_CHANNEL_Lh     = 14, /**< Left Height in front */
      AUDIOLIBS_CHANNEL_Ch     = 15, /**< Centre Height in Front  */
      AUDIOLIBS_CHANNEL_Rh     = 16, /**< Right Height in front  */
      AUDIOLIBS_CHANNEL_LFE2   = 17, /**< Low Frequency Effects 2 */
      AUDIOLIBS_CHANNEL_Lw     = 18, /**< Left on side in front */
      AUDIOLIBS_CHANNEL_Rw     = 19, /**< Right on side in front  */
      AUDIOLIBS_CHANNEL_Oh     = 20, /**< Over the listeners Head */
      AUDIOLIBS_CHANNEL_Lhs    = 21, /**< Left Height on Side */
      AUDIOLIBS_CHANNEL_Rhs    = 22, /**< Right Height on Side  */
      AUDIOLIBS_CHANNEL_Chr    = 23, /**< Centre Height in Rear  */
      AUDIOLIBS_CHANNEL_Lhr    = 24, /**< Left Height in Rear */
      AUDIOLIBS_CHANNEL_Rhr    = 25, /**< Right Height in Rear  */
      AUDIOLIBS_CHANNEL_Clf    = 26, /**< Low Center in Front */
      AUDIOLIBS_CHANNEL_Llf    = 27, /**< Low Left in Front */
      AUDIOLIBS_CHANNEL_Rlf    = 28, /**< Low Right in Front */
      AUDIOLIBS_CHANNEL_Lt     = 29,
      AUDIOLIBS_CHANNEL_Rt     = 30,
      AUDIOLIBS_CHANNEL_MAX_CHANNELS /**< This must always be the last entry on the list */
} audiolibs_ChannelMapping ;

#endif				/* _audiolibs_common_h_ */


