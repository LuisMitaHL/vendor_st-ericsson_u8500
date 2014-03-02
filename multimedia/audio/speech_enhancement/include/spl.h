/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/***********************************************************************
 * COPYRIGHT (C)   ERICSSON AB 2009
 *
 * The copyright to the document(s) herein is the property of
 * Ericsson AB, Sweden.
 *
 * The document(s) may be used and/or copied only with the
 * written permission from Ericsson AB
 * or in accordance with the terms and conditions stipulated
 * in the agreement or contract under which the document(s)
 * have been supplied.
 ***********************************************************************
 * DESCRIPTION
 *
 *
 *
 ***********************************************************************/

#ifndef INCLUSION_GUARD_SPL_H
#define INCLUSION_GUARD_SPL_H

#ifdef __cplusplus
extern "C" {
#endif


/***********************************************************************
 *
 * INCLUDE FILES
 *
 ***********************************************************************/
#define SPL_SW_LIB_ARM_NEON

#ifndef SPL_SW_LIB_ARM_NEON
#include <string.h>
#endif

#include "spl_debug.h"

/***********************************************************************
 *
 * CONSTANTS AND GERNERAL TYPES
 *
 ***********************************************************************/
/* General fixed point variables */
#ifndef _MSC_VER
#include <stdint.h>
#else
#include <intsafe.h>

#define int8_t char
#define int16_t short
#define int32_t int
#define int64_t signed long long
#define uint8_t  unsigned char
#define uint16_t unsigned short
#define uint32_t unsigned int
#define uint64_t unsigned long long
#endif

/***********************************************************************
 *
 * ENUM TYPES
 *
 ***********************************************************************/

typedef enum {
  SPL_FFT_IS_COMPLEX = 0,  /* Input is complex valued                      */
  SPL_FFT_IS_REAL          /* Input is real valued,                        */
                           /* stored as [Re(0) Re(1) ... Re(N/2-1) R(N/2)  */
                           /*                  Im(1) ... Im(N/2-1)]        */
} SPL_FFT_Data_t;

typedef enum {
  SPL_FFT_NO_NORMALIZATION = 0,    /* No input data normalization */
  SPL_FFT_NORMALIZE                /* Normalize input data        */
} SPL_FFT_Normalize_t;

typedef enum {
  SPL_NO_CONJUGATE = 0,
  SPL_CONJUGATE
} SPL_Conjugate_Data_t;  

typedef enum {
  SPL_MIN_LIMIT = 0,
  SPL_MAX_LIMIT
} SPL_Limit_t;  

typedef enum {
  SPL_LESS_THAN = 0,
  SPL_EQUAL_TO,
  SPL_GREATER_THAN
} SPL_Compare_t;  

typedef enum {
  SPL_HALF_LINEAR_PHASE_FILTER = 0,
  SPL_FULL_LINEAR_PHASE_FILTER,
  SPL_GENERAL_FIR_FILTER
} SPL_Filter_Type_t;

typedef enum {
  SPL_LOG2_GENERAL = 0,
  SPL_LOG2_EVEN,
  SPL_LOG10
} SPL_Log_Type_t;

typedef enum {
  SPL_EXP2 = 0,
  SPL_EXP10
} SPL_Exp_Type_t;

typedef enum {
  SPL_FALSE = 0,
  SPL_TRUE
} SPL_Boolean_t;


/***********************************************************************
 *
 * TYPE DECLARATIONS
 *
 ***********************************************************************/
#ifdef SPL_SW_LIB_ARM_NEON
#define SPL_FLOATING_POINT_DATA_TYPE
#define SPL_Float_t float
#endif

#ifdef SPL_SW_LIB_X86
#define SPL_FLOATING_POINT_DATA_TYPE
#define SPL_Float_t double
#endif

#ifdef SPL_SW_LIB_FLOAT
#define SPL_REFERENCE_FLOAT
#define SPL_RefFloat_t double 
#define SPL_Float_t double
#endif



#ifdef SPL_FLOATING_POINT_DATA_TYPE

  typedef SPL_Float_t SPL_int16QData_t;
  typedef SPL_Float_t SPL_int32QData_t;

  typedef struct {
    int16_t      Q; 
    SPL_Float_t  v;
  } SPL_sfloatQ_t;

  typedef struct {
    int16_t      Q; 
    int16_t      L; 
    SPL_Float_t  *v_p;
  } SPL_vfloatQ_t;

  #define SPL_sint16Q_t SPL_sfloatQ_t 
  #define SPL_sint32Q_t SPL_sfloatQ_t 
  #define SPL_vint16Q_t SPL_vfloatQ_t 
  #define SPL_vint32Q_t SPL_vfloatQ_t

#else

  #ifndef SPL_REFERENCE_FLOAT
  /* Single precision varaibles (48 dB dynamic range) */ 
  #define SPL_int16QData_t  int16_t  
  /* Double precision variables (96 dB dynamic range) */
  #define SPL_int32QData_t  int32_t  
  #else
  /* Single precision varaibles (48 dB dynamic range) */ 
  #define SPL_int16QData_t  SPL_RefFloat_t
  /* Double precision variables (96 dB dynamic range) */
  #define SPL_int32QData_t  SPL_RefFloat_t
  #endif

/* 16 bit mantissa */
  typedef struct {
    int16_t            Q; 
    SPL_int16QData_t   v;
  } SPL_sint16Q_t;

  typedef struct {
    int16_t            Q; 
    int16_t            L;    
    SPL_int16QData_t* v_p;
  } SPL_vint16Q_t;

  /* 32 bit mantissa */
  typedef struct {
    int16_t             Q; 
    SPL_int32QData_t    v;
  } SPL_sint32Q_t;

  typedef struct {
    int16_t           Q; 
    int16_t           L;    
    SPL_int32QData_t  *v_p;
  } SPL_vint32Q_t;
#endif /* SPL_FLOATING_POINT_DATA_TYPE */


/***********************************************************************
 * 
 * CPP MACROS FOR SPL VINT DECLARATIONS
 *
 ***********************************************************************/

#define SPL_INIT_VINT(VARIABLE, DATA, Q_VALUE) \
do {\
  (VARIABLE).L   = sizeof(DATA)/sizeof(DATA[0]);\
  (VARIABLE).Q   = Q_VALUE;\
  (VARIABLE).v_p = DATA;\
} while(0)


/***********************************************************************
 *
 * Definition of other types 
 *
 ***********************************************************************/
  
typedef struct {
  int16_t* data_p; 
  int      circularIndex;
  int      bufferLength;
} SPL_CircularBuffer_t;

#ifdef SPL_FLOATING_POINT_DATA_TYPE
typedef struct {
    SPL_Float_t*  data_p;
    int                circularIndex;
    int                bufferLength;
  } SPL_CircularBuffer_vfloatQ_t;
  #define  SPL_CircularBuffer_vint16Q_t SPL_CircularBuffer_vfloatQ_t
  #define  SPL_CircularBuffer_vint32Q_t SPL_CircularBuffer_vfloatQ_t
#else
  typedef struct {
    SPL_int16QData_t*  data_p;
    int                circularIndex;
    int                bufferLength;
  } SPL_CircularBuffer_vint16Q_t;
  typedef struct {
  SPL_int32QData_t*  data_p;
  int                circularIndex;
  int                bufferLength;
} SPL_CircularBuffer_vint32Q_t;
#endif

#ifndef _MSC_VER
typedef struct {
  int16_t A_p[2] __attribute__((aligned(4)));
  int16_t B_p[3] __attribute__((aligned(4)));
} SPL_BiQuadCoeffs_t;

typedef struct {
  SPL_BiQuadCoeffs_t  Coeffs;

  int16_t             x_old[2] __attribute__((aligned(4)));
  int32_t             y_old[2] __attribute__((aligned(4)));
} SPL_BiQuad_t;

typedef struct {
  SPL_vint16Q_t A;
  SPL_vint16Q_t B;
  SPL_int16QData_t AData_p[2] __attribute__((aligned(4)));
  SPL_int16QData_t BData_p[3] __attribute__((aligned(4)));
} SPL_BiQuadQCoeffs_t;
typedef struct {
  SPL_BiQuadQCoeffs_t  Coeffs;

  SPL_vint16Q_t       xOld;
  SPL_vint32Q_t       yOld;

  SPL_int16QData_t    xOldData_p[2] __attribute__((aligned(4)));
  SPL_int32QData_t    yOldData_p[2] __attribute__((aligned(4)));
} SPL_BiQuadQ_t;

#else

typedef struct {
  int16_t A_p[2];
  int16_t B_p[3];
} SPL_BiQuadCoeffs_t;

typedef struct {
  SPL_BiQuadCoeffs_t  Coeffs;

  int16_t             x_old[2];
  int32_t             y_old[2];
} SPL_BiQuad_t;

typedef struct {
  SPL_vint16Q_t A;
  SPL_vint16Q_t B;
  SPL_int16QData_t AData_p[2];
  SPL_int16QData_t BData_p[3];
} SPL_BiQuadQCoeffs_t;
typedef struct {
  SPL_BiQuadQCoeffs_t  Coeffs;
  
  SPL_vint16Q_t       xOld;
  SPL_vint32Q_t       yOld;
  
  SPL_int16QData_t    xOldData_p[2];
  SPL_int32QData_t    yOldData_p[2];
} SPL_BiQuadQ_t;
#endif



  
/***********************************************************************
 *
 * Data windows including window power
 *
 ***********************************************************************/
// Half Hamming window of even lengths and corresponding window power
extern const int16_t SPL_HammingWindow160_p[80];
extern const int16_t SPL_HammingWindow320_p[160];
extern const int16_t SPL_HammingWindow640_p[320];
extern const int16_t SPL_HammingWindow960_p[480];
extern const SPL_vint16Q_t SPL_RaisingHammingWindow160;
extern const SPL_vint16Q_t SPL_FallingHammingWindow160;
extern const SPL_vint16Q_t SPL_RaisingHammingWindow320;
extern const SPL_vint16Q_t SPL_FallingHammingWindow320;
extern const SPL_vint16Q_t SPL_RaisingHammingWindow640;
extern const SPL_vint16Q_t SPL_FallingHammingWindow640;
extern const SPL_vint16Q_t SPL_RaisingHammingWindow960;
extern const SPL_vint16Q_t SPL_FallingHammingWindow960;

#define SPL_HAMMING_WINDOW_POWER_160 25884          /* Q16 */
#define SPL_HAMMING_WINDOW_POWER_320 25964          /* Q16 */
#define SPL_HAMMING_WINDOW_POWER_640 26004          /* Q16 */
#define SPL_HAMMING_WINDOW_POWER_960 26017          /* Q16 */

// Odd order half Kaiser windows with reversed coefficients, including 1 at end
extern const int16_t SPL_KaiserWindow63_p[32];
extern const int16_t SPL_KaiserWindow127_p[64];

// Odd order half Hanning windows including 0 and 1 at ends
extern const SPL_vint16Q_t SPL_RaisingHanningWindow15;
extern const SPL_vint16Q_t SPL_FallingHanningWindow15;
extern const SPL_vint16Q_t SPL_RaisingHanningWindow31;
extern const SPL_vint16Q_t SPL_FallingHanningWindow31;
extern const SPL_vint16Q_t SPL_RaisingHanningWindow63;
extern const SPL_vint16Q_t SPL_FallingHanningWindow63;
extern const SPL_vint16Q_t SPL_RaisingHanningWindow95;
extern const SPL_vint16Q_t SPL_FallingHanningWindow95;
  

/***********************************************************************
 *
 * RETURN TYPE - VOID OR ERROR CODE HANDLING FOR SPL IN DEBUG
 *
 ***********************************************************************/

/* Error code */ 
typedef enum {
  SPL_NO_ERROR = 0,
  SPL_UNINITIALIZED_PARAMETER,
  SPL_UNINITIALIZED_DATA,
  SPL_CONFLICTING_VECTOR_LENGTHS,
  SPL_CONFLICTING_Q_VALUES,
  SPL_VECTOR_INDEX_OUT_OF_RANGE,
  SPL_DIVISION_BY_ZERO,
  SPL_CONFLICTING_VECTOR_INDEX,
  SPL_INPUT_VECTOR_NOT_MODULO_TWO,
  SPL_INPUT_VECTOR_NOT_MODULO_FOUR,
  SPL_INPUT_VECTOR_NOT_MODULO_EIGHT,
  SPL_INVALID_FFT_LENGTH,
  SPL_INVALID_FILTER_LENGTH,
  SPL_NON_ZERO_Q_VALUE_FOR_FLOAT,
  SPL_NON_SUPPORTED_EXP_TYPE,
  SPL_NON_SUPPORTED_LOG_TYPE,
  SPL_UNKNOWN_ERROR
} SPL_Return_t;

#define SPL_NUMBER_OF_ERROR_CODES 17

/***********************************************************************
 *
 * CONSTANTS FOR INTERNAL VARIABLES
 *
 ***********************************************************************/
#if (SPL_MAX_SAMPLE_RATE == 8)
  #define SPL_MAX_VECTOR_LENGTH             256 
  #define SPL_MAX_VECTOR_LENGTH_BY_TWO      128 
#else
  #if (SPL_MAX_SAMPLE_RATE == 16)
    #define SPL_MAX_VECTOR_LENGTH             512
    #define SPL_MAX_VECTOR_LENGTH_BY_TWO      256
  #else
    #if (SPL_MAX_SAMPLE_RATE == 32)
      #define SPL_MAX_VECTOR_LENGTH            1024
      #define SPL_MAX_VECTOR_LENGTH_BY_TWO      512
    #else
      #define SPL_MAX_VECTOR_LENGTH            1536
      #define SPL_MAX_VECTOR_LENGTH_BY_TWO      768
    #endif
  #endif
#endif
#define SPL_MAX_FLOAT                    1e30
#define SPL_MAX_AR_ORDER                   12
#define SPL_MAX_FIR_FILTER_LENGTH         256
#define SPL_MAX_FIR_FILTER_LENGTH_BY_TWO  128


/***********************************************************************
 *
 * CONSTANTS FOR CODE USING SPL
 *
 ***********************************************************************/
/* Generic constants */
#define SPL_ZERO_VALUE_Q (int16_t) 150
#define SPL_MIN_Q_VALUE (int16_t) -150
#define SPL_EPS 1E-8f


/* Constants that are independent of float or fix */
#define SPL_CONSTANT_ZERO_V   0
#define SPL_CONSTANT_ZERO_Q   0

#define SPL_CONSTANT_ONE_V    1
#define SPL_CONSTANT_ONE_Q    0

#define SPL_CONSTANT_TWO_V    2
#define SPL_CONSTANT_TWO_Q    0

#define SPL_CONSTANT_FOUR_V    4
#define SPL_CONSTANT_FOUR_Q    0

#define SPL_CONSTANT_EIGHT_V    8
#define SPL_CONSTANT_EIGHT_Q    0

#define SPL_CONSTANT_TEN_V    10
#define SPL_CONSTANT_TEN_Q    0


/* Constants that are float or fix dependent */
#ifdef SPL_FLOATING_POINT_DATA_TYPE
#define SPL_FLOATING_POINT_CONSTANTS
#endif

#ifdef SPL_FLOATING_POINT_CONSTANTS

#define SPL_CONSTANT_DBOVL_TO_DB_V  90.30859375 /* 23119 @ Q8 */
#define SPL_CONSTANT_DBOVL_TO_DB_Q  0

#define SPL_ONE_BY_TWO_V        0.5
#define SPL_ONE_BY_FOUR_V       0.25
#define SPL_ONE_BY_EIGHT_V      0.125
#define SPL_ONE_BY_SIXTEEN_V    0.0625
#define SPL_ONE_BY_THIRTYTWO_V  0.03125
#define SPL_ONE_BY_SIXTYFOUR_V  0.015625

#define SPL_ONE_BY_TWO_Q        0
#define SPL_ONE_BY_FOUR_Q       0
#define SPL_ONE_BY_EIGHT_Q      0
#define SPL_ONE_BY_SIXTEEN_Q    0
#define SPL_ONE_BY_THIRTYTWO_Q  0
#define SPL_ONE_BY_SIXTYFOUR_Q  0

#define SPL_TWO_POWER_MINUS_SEVEN_V     0.007812500000000
#define SPL_TWO_POWER_MINUS_EIGHT_V     0.003906250000000
#define SPL_TWO_POWER_MINUS_NINE_V      0.001953125000000
#define SPL_TWO_POWER_MINUS_TEN_V       9.765625000000000e-004
#define SPL_TWO_POWER_MINUS_ELEVEN_V    4.882812500000000e-004
#define SPL_TWO_POWER_MINUS_TWELVE_V    2.441406250000000e-004
#define SPL_TWO_POWER_MINUS_THIRTEEN_V  1.220703125000000e-004
#define SPL_TWO_POWER_MINUS_FOURTEEN_V  6.103515625000000e-005
#define SPL_TWO_POWER_MINUS_FIFTEEN_V   3.051757812500000e-005
#define SPL_TWO_POWER_MINUS_SIXTEEN_V   1.525878906250000e-005
#define SPL_TWO_POWER_MINUS_THIRTYTWO_V 2.328306436538696e-010

#define SPL_TWO_POWER_MINUS_SEVEN_Q     0
#define SPL_TWO_POWER_MINUS_EIGHT_Q     0
#define SPL_TWO_POWER_MINUS_NINE_Q      0
#define SPL_TWO_POWER_MINUS_TEN_Q       0
#define SPL_TWO_POWER_MINUS_ELEVEN_Q    0
#define SPL_TWO_POWER_MINUS_TWELVE_Q    0
#define SPL_TWO_POWER_MINUS_THIRTEEN_Q  0
#define SPL_TWO_POWER_MINUS_FOURTEEN_Q  0
#define SPL_TWO_POWER_MINUS_FIFTEEN_Q   0
#define SPL_TWO_POWER_MINUS_SIXTEEN_Q   0
#define SPL_TWO_POWER_MINUS_THIRTYTWO_Q 0

#define SPL_TWO_POWER_FIFTEEN_V  32768
#define SPL_TWO_POWER_FIFTEEN_Q  0

#define SPL_THREE_BY_FOUR_V     0.75
#define SPL_THREE_BY_FOUR_Q     0

#define SPL_ONE_BY_TWENTY_V     0.049999237060547
#define SPL_ONE_BY_TWENTY_Q     0

#define SPL_SQRT_ONE_BY_TWO_V   0.707092285156250
#define SPL_SQRT_ONE_BY_TWO_Q   0

#define SPL_ZERO_POINT_ZERO_THREE_DB_V       1.007080078125000
#define SPL_ZERO_POINT_ZERO_THREE_DB_Q       0

#define SPL_ZERO_POINT_ONE_DB_V         1.023315429687500
#define SPL_ZERO_POINT_ONE_DB_Q         0

#define SPL_ONE_BY_EIGHT_DB_V           1.029174804687500
#define SPL_ONE_BY_EIGHT_DB_Q           0

#define SPL_ZERO_POINT_TWO_DB_V         1.004638671875000
#define SPL_ZERO_POINT_TWO_DB_Q         0

#define SPL_ZERO_POINT_FIVE_DB_V        1.122009277343750
#define SPL_ZERO_POINT_FIVE_DB_Q        0

#define SPL_ONE_POINT_FIVE_DB_V         1.412536621093750
#define SPL_ONE_POINT_FIVE_DB_Q         0

#define SPL_THREE_DB_V                  2
#define SPL_THREE_DB_Q                  0

#define SPL_FOUR_POINT_FIVE_DB_V        2.818359375000000
#define SPL_FOUR_POINT_FIVE_DB_Q        0

#define SPL_SIX_DB_V                    4
#define SPL_SIX_DB_Q                    0

#define SPL_MINUS_ZERO_POINT_ZERO_THREE_DB_V       0.992553710937500
#define SPL_MINUS_ZERO_POINT_ZERO_THREE_DB_Q       0

#define SPL_MINUS_ZERO_POINT_FIVE_DB_V  0.891235351562500
#define SPL_MINUS_ZERO_POINT_FIVE_DB_Q  0

#define SPL_MINUS_ONE_POINT_FIVE_DB_V   0.707946777343750
#define SPL_MINUS_ONE_POINT_FIVE_DB_Q   0

#define SPL_MINUS_THREE_DB_V            0.501190185546875
#define SPL_MINUS_THREE_DB_Q            0

#define SPL_MINUS_FIVE_DB_V             0.316223144531250
#define SPL_MINUS_FIVE_DB_Q             0

#define SPL_MINUS_SIX_DB_V              0.25
#define SPL_MINUS_SIX_DB_Q              0

#define SPL_MINUS_TWENTY_DB_V           0.010000228881836
#define SPL_MINUS_TWENTY_DB_Q           0

#define SPL_MINUS_ZERO_POINT_ZEROFIVE_DB_V  0.988555908203125
#define SPL_MINUS_ZERO_POINT_ZEROFIVE_DB_Q  0

#define SPL_ONE_POINT_FIVE_V            1.5
#define SPL_ONE_POINT_FIVE_Q            0

#define SPL_ZERO_POINT_ONE_V            0.100097656250000
#define SPL_ZERO_POINT_ONE_Q            0

#define SPL_ZERO_POINT_FIFTEEN_V        0.149902343750000
#define SPL_ZERO_POINT_FIFTEEN_Q        0

#else

#define SPL_CONSTANT_DBOVL_TO_DB_V    23119
#define SPL_CONSTANT_DBOVL_TO_DB_Q    8

#define SPL_ONE_BY_TWO_V        1
#define SPL_ONE_BY_FOUR_V       1
#define SPL_ONE_BY_EIGHT_V      1
#define SPL_ONE_BY_SIXTEEN_V    1
#define SPL_ONE_BY_THIRTYTWO_V  1
#define SPL_ONE_BY_SIXTYFOUR_V  1

#define SPL_ONE_BY_TWO_Q        1
#define SPL_ONE_BY_FOUR_Q       2
#define SPL_ONE_BY_EIGHT_Q      3
#define SPL_ONE_BY_SIXTEEN_Q    4
#define SPL_ONE_BY_THIRTYTWO_Q  5
#define SPL_ONE_BY_SIXTYFOUR_Q  6

#define SPL_TWO_POWER_MINUS_SEVEN_V     1
#define SPL_TWO_POWER_MINUS_EIGHT_V     1
#define SPL_TWO_POWER_MINUS_NINE_V      1
#define SPL_TWO_POWER_MINUS_TEN_V       1
#define SPL_TWO_POWER_MINUS_ELEVEN_V    1
#define SPL_TWO_POWER_MINUS_TWELVE_V    1
#define SPL_TWO_POWER_MINUS_THIRTEEN_V  1
#define SPL_TWO_POWER_MINUS_FOURTEEN_V  1
#define SPL_TWO_POWER_MINUS_FIFTEEN_V   1
#define SPL_TWO_POWER_MINUS_SIXTEEN_V   1
#define SPL_TWO_POWER_MINUS_THIRTYTWO_V 1

#define SPL_TWO_POWER_MINUS_SEVEN_Q      7
#define SPL_TWO_POWER_MINUS_EIGHT_Q      8
#define SPL_TWO_POWER_MINUS_NINE_Q       9
#define SPL_TWO_POWER_MINUS_TEN_Q       10
#define SPL_TWO_POWER_MINUS_ELEVEN_Q    11
#define SPL_TWO_POWER_MINUS_TWELVE_Q    12
#define SPL_TWO_POWER_MINUS_THIRTEEN_Q  13
#define SPL_TWO_POWER_MINUS_FOURTEEN_Q  14
#define SPL_TWO_POWER_MINUS_FIFTEEN_Q   15
#define SPL_TWO_POWER_MINUS_SIXTEEN_Q   16
#define SPL_TWO_POWER_MINUS_THIRTYTWO_Q 32

#define SPL_TWO_POWER_FIFTEEN_V  1
#define SPL_TWO_POWER_FIFTEEN_Q  -15

#define SPL_THREE_BY_FOUR_V     3
#define SPL_THREE_BY_FOUR_Q     2

#define SPL_ONE_BY_TWENTY_V     26214
#define SPL_ONE_BY_TWENTY_Q     19

#define SPL_SQRT_ONE_BY_TWO_V   23170
#define SPL_SQRT_ONE_BY_TWO_Q   15

#define SPL_ZERO_POINT_ONE_DB_V         16766
#define SPL_ZERO_POINT_ONE_DB_Q         14 

#define SPL_ONE_BY_EIGHT_DB_V           16862
#define SPL_ONE_BY_EIGHT_DB_Q           14

#define SPL_ZERO_POINT_TWO_DB_V         16460
#define SPL_ZERO_POINT_TWO_DB_Q         14

#define SPL_ZERO_POINT_ZERO_THREE_DB_V       16500
#define SPL_ZERO_POINT_ZERO_THREE_DB_Q       14

#define SPL_ZERO_POINT_FIVE_DB_V        18383
#define SPL_ZERO_POINT_FIVE_DB_Q        14

#define SPL_ONE_POINT_FIVE_DB_V         23143
#define SPL_ONE_POINT_FIVE_DB_Q         14

#define SPL_THREE_DB_V                  2
#define SPL_THREE_DB_Q                  0 

#define SPL_FOUR_POINT_FIVE_DB_V        11544
#define SPL_FOUR_POINT_FIVE_DB_Q        12

#define SPL_SIX_DB_V                    4
#define SPL_SIX_DB_Q                    0

#define SPL_MINUS_ZERO_POINT_ZERO_THREE_DB_V       32524
#define SPL_MINUS_ZERO_POINT_ZERO_THREE_DB_Q       15

#define SPL_MINUS_ZERO_POINT_FIVE_DB_V  29204
#define SPL_MINUS_ZERO_POINT_FIVE_DB_Q  15

#define SPL_MINUS_ZERO_POINT_ZEROFIVE_DB_V  32393
#define SPL_MINUS_ZERO_POINT_ZEROFIVE_DB_Q  15

#define SPL_MINUS_ONE_POINT_FIVE_DB_V   23198
#define SPL_MINUS_ONE_POINT_FIVE_DB_Q   15

#define SPL_MINUS_THREE_DB_V            16423
#define SPL_MINUS_THREE_DB_Q            15

#define SPL_MINUS_FIVE_DB_V             5181
#define SPL_MINUS_FIVE_DB_Q             14

#define SPL_MINUS_SIX_DB_V              1
#define SPL_MINUS_SIX_DB_Q              2

#define SPL_MINUS_TWENTY_DB_V           20972
#define SPL_MINUS_TWENTY_DB_Q           21

#define SPL_ONE_POINT_FIVE_V            3
#define SPL_ONE_POINT_FIVE_Q            1

#define SPL_ZERO_POINT_ONE_V            410
#define SPL_ZERO_POINT_ONE_Q            12

#define SPL_ZERO_POINT_FIFTEEN_V        614
#define SPL_ZERO_POINT_FIFTEEN_Q        12

#endif


/***********************************************************************
 *
 * PUBLIC FUNCTIONS
 *
 ***********************************************************************/

/***********************************************************************
 *
 * Defines
 *
 ***********************************************************************/

#define SPL_MIN(X, Y) ((X) >= (Y) ? (Y) : (X))
#define SPL_MAX(X, Y) ((X) >= (Y) ? (X) : (Y))

#define SPL_Copy(X, Y, Z)  memcpy((X), (Y), (Z))
#define SPL_Set(X, Y, Z)  memset((X), (Y), (Z))

#define SPL_INCREMENT_CIRCULAR_INDEX(X, Y, Z) \
  (X) = (X) + (Y); if ((X) >= (Z)) { (X) = (X) - (Z); };

#define SPL_DECREMENT_CIRCULAR_INDEX(X, Y, Z) \
  (X) = (X) - (Y); if ((X) < 0) { (X) = (X) + (Z); };

#define SPL_IS_TRUE(X) \
  ((X) == SPL_TRUE)

#define SPL_IS_FALSE(X) \
  ((X) == SPL_FALSE)

#define SPL_NEGATE(X) (SPL_TRUE == (X) ? SPL_FALSE : SPL_TRUE)

/***********************************************************************
 *
 * Functions declared with defines to avoid duplication of function calls
 *
 * Common functions for fix and float SPL
 *
 ***********************************************************************/
// Copy of sint16Q and sint32Q
#define SPL_Copy_sint16Q(X, Y)\
do {\
  (X).v = (Y).v; \
  (X).Q = (Y).Q; \
} while (0)

#define SPL_Copy_sint32Q(X, Y)\
do {\
  (X).v = (Y).v; \
  (X).Q = (Y).Q; \
} while (0)

#define SPL_Copy_sint32Q_sint16Q(X, Y)\
do {\
  (X).v = (Y).v; \
  (X).Q = (Y).Q; \
} while (0)

// Set sint16Q and sint32Q
#define SPL_Set_sint16Q(X, value, Qvalue)\
do {\
  (X).v = (SPL_int16QData_t) (value); \
  (X).Q = (Qvalue); \
} while (0)

#define SPL_Set_sint32Q(X, value, Qvalue)\
do {\
  (X).v = (SPL_int32QData_t) (value); \
  (X).Q = (Qvalue); \
} while (0)

#define SPL_Index_vint16Q(X, Y, start, length)\
do {\
  SPL_INDEX_VINT_PARAMETER_VERIFICATION(X, Y, start, length); \
  (X).v_p = &((Y).v_p[start]); \
  (X).L = length; \
  (X).Q = (Y).Q; \
} while (0)

#define SPL_Init_BiQuadQ(X) \
do {\
  SPL_INIT_VINT((X).Coeffs.A, (X).Coeffs.AData_p, 0); \
  SPL_INIT_VINT((X).Coeffs.B, (X).Coeffs.BData_p, 0); \
  SPL_INIT_VINT((X).xOld, (X).xOldData_p, 0); \
  SPL_INIT_VINT((X).yOld, (X).yOldData_p, 0); \
} while (0)

/***********************************************************************
 *
 * Functions declared with defines to avoid duplication of function calls
 *
 * Specific functions for fix and float SPL
 *
 ***********************************************************************/
#ifdef SPL_REFERENCE_FLOAT
#define SPL_MAP_BASIC_FUNCTIONS
#endif

#ifdef SPL_FLOATING_POINT_DATA_TYPE
#define SPL_MAP_BASIC_FUNCTIONS
#endif

void SPL_Negate_sint16Q(SPL_sint16Q_t  X);
void SPL_Negate_sint32Q(SPL_sint32Q_t  X);
void SPL_Abs_sint16Q(SPL_vint16Q_t  X);
void SPL_Abs_sint32Q(SPL_vint32Q_t  X);


#ifdef SPL_MAP_BASIC_FUNCTIONS
// Negation
#define SPL_Negate_sint16Q(X)\
do {\
  (X).v = -(X).v; \
} while (0)

#define SPL_Negate_sint32Q(X)\
do {\
  (X).v = -(X).v; \
} while (0)

// Absolute values
#define SPL_Abs_sint16Q(X)\
do {\
  (X).v = fabs((X).v); \
} while (0)

#define SPL_Abs_sint32Q(X)\
do {\
  (X).v = fabs((X).v); \
} while (0)
#else
// Not declared for fixed point yet
#endif


/***********************************************************************
 *
 * Low level fixed point instructions based on ITU-T STL 
 *
 ***********************************************************************/

int16_t SPL_add (int16_t var1, int16_t var2);

int16_t SPL_sub (int16_t var1, int16_t var2);

int16_t SPL_abs_s (int16_t var1);

int16_t SPL_shl(int16_t var1, int16_t var2);

int16_t SPL_shr (int16_t var1, int16_t var2);

int16_t SPL_mult (int16_t var1, int16_t var2);

int32_t SPL_L_mult (int16_t var1, int16_t var2);

int16_t SPL_negate (int16_t var1);

int16_t SPL_extract_h (int32_t L_var1);

int16_t SPL_extract_l (int32_t L_var1);

int16_t SPL_round (int32_t L_var1);

int32_t SPL_L_mac (int32_t L_var3, int16_t var1, int16_t var2);

int32_t SPL_L_msu (int32_t L_var3, int16_t var1, int16_t var2);

int32_t SPL_L_macNs (int32_t L_var3, int16_t var1, int16_t var2);

int32_t SPL_L_msuNs (int32_t L_var3, int16_t var1, int16_t var2);

int32_t SPL_L_add (int32_t L_var1, int32_t L_var2);

int32_t SPL_L_sub (int32_t L_var1, int32_t L_var2);

int32_t SPL_L_add_c (int32_t L_var1, int32_t L_var2);

int32_t SPL_L_sub_c (int32_t L_var1, int32_t L_var2);

int32_t SPL_L_negate (int32_t L_var1);

int16_t SPL_mult_r (int16_t var1, int16_t var2);

int32_t SPL_L_shl (int32_t L_var1, int16_t var2);

int64_t SPL_LL_shl (int64_t L_var1, int16_t var2);

int32_t SPL_L_shr (int32_t L_var1, int16_t var2);

int64_t SPL_LL_shr (int64_t L_var1, int16_t var2);

int16_t SPL_shr_r (int16_t var1, int16_t var2);

int16_t SPL_mac_r (int32_t L_var3, int16_t var1, int16_t var2);

int16_t SPL_msu_r (int32_t L_var3, int16_t var1, int16_t var2);

int32_t SPL_L_deposit_h (int16_t var1);

int32_t SPL_L_deposit_l (int16_t var1);

int32_t SPL_L_shr_r (int32_t L_var1, int16_t var2);

int64_t SPL_LL_shr_r (int64_t L_var1, int16_t var2);

int32_t SPL_L_abs (int32_t L_var1);

int32_t SPL_L_sat (int32_t L_var1);

int16_t SPL_norm_s (int16_t var1);

int16_t SPL_div_s (int16_t var1, int16_t var2);

int16_t SPL_norm_l (int32_t L_var1);

int16_t SPL_saturate (int32_t L_var1);

int32_t SPL_L_saturate (int64_t var1);

/***********************************************************************
 *
 * FUNCTION HEADERS
 *
 ***********************************************************************/
#ifdef SPL_VERIFY_CALLS
#define SPL_RETURN_TYPE SPL_Return_t
#else
#define SPL_RETURN_TYPE void
#endif


/***********************************************************************
 *
 * Fixed Q
 *
 ***********************************************************************/
SPL_RETURN_TYPE SPL_Add_vint16(      int16_t*         const Sum_p,
                            const int16_t*         const X_p,
                            const int16_t*         const Y_p,
                            const int16_t                L);

SPL_RETURN_TYPE SPL_Add_vint16_int16(      int16_t*  const Sum_p,
                                  const int16_t*  const X_p,
                                  const int16_t         term,
                                  const int16_t         L);

SPL_RETURN_TYPE SPL_And_vint16(      int16_t*  const flag_p,
                            const int16_t*  const x_p,
                            const int16_t*  const y_p,
                            const int16_t        L);
                            
SPL_RETURN_TYPE SPL_BlockSum_vint16(      int16_t*  const x_Summed_p,
                                 const int16_t*  const x_p,
                                 const int16_t        length,
                                 const int16_t        bandwidth);
                     
SPL_RETURN_TYPE SPL_Find_vint16(      int16_t*       const Flag_p,
                             const int16_t*       const X_p,
                             const int16_t              Value,
                             const SPL_Compare_t        comparison,
                             const int16_t              L);

SPL_RETURN_TYPE SPL_Find_vint32(      int16_t*       const Flag_p,
                             const int32_t*       const X_p,
                             const int32_t              Value,
                             const SPL_Compare_t        comparison,
                             const int16_t              L);

SPL_RETURN_TYPE SPL_Limit_vint16(      int16_t*     const X_limit_p,
                              const int16_t*     const X_p,
                              const int16_t            Limit,
                              const int16_t            L,
                              const SPL_Limit_t        comparison);

SPL_RETURN_TYPE SPL_Log_int16(      int16_t*       const out_p,
                           const int16_t              in,
                           const SPL_Log_Type_t       logType);

SPL_RETURN_TYPE SPL_Mask_vint16(      int16_t*  const x_masked_p,
                             const int16_t*  const x_p,
                             const int16_t*  const mask_p,
                             const int16_t         L);

SPL_RETURN_TYPE SPL_Mask_vint32(      int32_t*  const x_masked_p,
                             const int32_t*  const x_p,
                             const int16_t*  const mask_p,
                             const int16_t         L);

SPL_RETURN_TYPE SPL_Max_vint16(      int16_t*  const Max_p,
                                  int16_t*  const index_p,
                            const int16_t*  const x_p,
                            const int16_t         L);

SPL_RETURN_TYPE SPL_MaxAbs_vint16(      int16_t*  const Max_p,
                                     int16_t*  const index_p,
                               const int16_t*  const x_p,
                               const int16_t         L);

SPL_RETURN_TYPE SPL_Mult_vint16(      int16_t*  const Product_p,
                             const int16_t*  const X_p,
                             const int16_t*  const Y_p,
                             const int16_t         L);

SPL_RETURN_TYPE SPL_Mult_vint16_int16(      int16_t*       const Product_p,
                                   const int16_t*       const X_p,
                                   const int16_t              factor,
                                   const int16_t              length);

SPL_RETURN_TYPE SPL_Norm_vint16(      int16_t*         const norm_p,
                             const int16_t*         const X_p,
                             const int16_t                L);

SPL_RETURN_TYPE SPL_Norm_vint32(      int16_t*        const norm_p,
                             const int32_t*        const X_p,
                             const int16_t               L);

SPL_RETURN_TYPE SPL_Not_vint16(      int16_t*  const flag_p,
                            const int16_t*  const x_p,
                            const int16_t         L);

                             
SPL_RETURN_TYPE SPL_Or_vint16(      int16_t*  const flag_p,
                           const int16_t*  const x_p,
                           const int16_t*  const y_p,
                           const int16_t         L);

SPL_RETURN_TYPE SPL_Resample_vint16(      int16_t*  const X_resampled_p,
                                 const int16_t*  const X_p,
                                 const int16_t         resampleFactor,
                                 const int16_t         L);

SPL_RETURN_TYPE SPL_Set_vint16(      int16_t*  const data_p,
                            const int16_t        value,
                            const int16_t        L);

SPL_RETURN_TYPE SPL_ShiftLeft_vint16(      int16_t*        const Out_p,
                                  const int16_t*        const In_p,
                                  const int16_t               shiftValue,
                                  const int16_t               L);

SPL_RETURN_TYPE SPL_ShiftLeft_vint32(      int32_t*        const Out_p,
                                  const int32_t*        const In_p,
                                  const int16_t               shiftValue,
                                  const int16_t               L);

SPL_RETURN_TYPE SPL_ShiftRight_vint16(      int16_t*        const Out_p,
                                   const int16_t*        const In_p,
                                   const int16_t               shiftValue,
                                   const int16_t               L);

SPL_RETURN_TYPE SPL_ShiftRight_vint32(      int32_t*        const Out_p,
                                   const int32_t*        const In_p,
                                   const int16_t               shiftValue,
                                   const int16_t               L);

SPL_RETURN_TYPE SPL_ShiftRightRound_vint16(      int16_t*        const Out_p,
                                        const int16_t*        const In_p,
                                        const int16_t               shiftValue,
                                        const int16_t               L);

SPL_RETURN_TYPE SPL_ShiftRightRound_vint32(      int32_t*        const Out_p,
                                        const int32_t*        const In_p,
                                        const int16_t               shiftValue,
                                        const int16_t               L);

SPL_RETURN_TYPE SPL_Sub_int16_vint16(      int16_t*         const Diff_p,
                                  const int16_t*         const X_p,
                                  const int16_t                term,
                                  const int16_t                L);

SPL_RETURN_TYPE SPL_Sub_vint16(      int16_t*        const Diff_p,
                            const int16_t*        const X_p,
                            const int16_t*        const Y_p,
                            const int16_t               L);

SPL_RETURN_TYPE SPL_Sub_vint16_int16(      int16_t*       const Diff_p,
                                  const int16_t*       const X_p, 
                                  const int16_t              term,
                                  const int16_t              L);

SPL_RETURN_TYPE SPL_Sum_vint16(      int32_t*  const y_p,
                            const int16_t*  const x_p,
                            const int16_t         L);

SPL_RETURN_TYPE SPL_Vmax_vint16(      int16_t*  const Max_p,
                             const int16_t*  const X_p,
                             const int16_t*  const Y_p,
                             const int16_t         L);


SPL_RETURN_TYPE SPL_Vmin_vint16(      int16_t*  const Min_p,
                             const int16_t*  const X_p,
                             const int16_t*  const Y_p,
                             const int16_t         L);


/***********************************************************************
 *
 * Floating Q
 *
 ***********************************************************************/
#ifdef SPL_FLOATING_POINT_DATA_TYPE
  /* Declaration of floating point mantissa functions */
  SPL_RETURN_TYPE SPL_AbsSquare_Cx_vfloatQ(      SPL_vfloatQ_t*  const X_Squared_p,
                                        const SPL_vfloatQ_t*  const X_p);

  SPL_RETURN_TYPE SPL_Add_sfloatQ(      SPL_sfloatQ_t*  const sum_p,
                               const SPL_sfloatQ_t*  const x_p,
                               const SPL_sfloatQ_t*  const y_p);

  SPL_RETURN_TYPE SPL_Add_vfloatQ(      SPL_vfloatQ_t*  const Sum_p,
                               const SPL_vfloatQ_t*  const X_p,
                               const SPL_vfloatQ_t*  const Y_p);

  SPL_RETURN_TYPE SPL_Add_vfloatQ_sfloatQ(      SPL_vfloatQ_t*  const Sum_p,
                                       const SPL_vfloatQ_t*  const X_p,
                                       const SPL_sfloatQ_t*  const Term_p);

  SPL_RETURN_TYPE SPL_Average_sfloatQ(      SPL_sfloatQ_t*  const Average_p,
                                   const SPL_sfloatQ_t*  const X_p,
                                   const SPL_sfloatQ_t*  const Y_p,
                                   const SPL_sfloatQ_t*  const Rho_p);

  SPL_RETURN_TYPE SPL_Average_vfloatQ(      SPL_vfloatQ_t*  const Average_p,
                                   const SPL_vfloatQ_t*  const X_p,
                                   const SPL_vfloatQ_t*  const Y_p,
                                   const SPL_vfloatQ_t*  const Rho_p);

  SPL_RETURN_TYPE SPL_AverageElements_vfloatQ(      SPL_vfloatQ_t*  const X_Averaged_p,
                                           const SPL_vfloatQ_t*  const X_p,
                                           const int16_t               start,
                                           const int16_t               length,
                                           const int16_t*              bandwidthNumBits_p);

  SPL_RETURN_TYPE SPL_BlockMax_vfloatQ(      SPL_vfloatQ_t*  const Max_p,
                                          int16_t*        const index_p,
                                    const SPL_vfloatQ_t*  const X_p,
                                    const int16_t               bandwidth);

  SPL_RETURN_TYPE SPL_BlockSum_vfloatQ(      SPL_vfloatQ_t*  const X_Summed_p,
                                    const SPL_vfloatQ_t*  const X_p,
                                    const int16_t               start,
                                    const int16_t               length,
                                    const int16_t               bandwidth);

  SPL_RETURN_TYPE SPL_Compare_sfloatQ(      SPL_Boolean_t*   const Flag_p,
                                   const SPL_sfloatQ_t*   const X_p,
                                   const SPL_sfloatQ_t*   const Y_p,
                                   const SPL_Compare_t          comparison);

  SPL_RETURN_TYPE SPL_Compare_vfloatQ(      int16_t*        const Flag_p,
                                   const SPL_vfloatQ_t*  const X_p,
                                   const SPL_vfloatQ_t*  const Y_p,
                                   const SPL_Compare_t         comparison);

  SPL_RETURN_TYPE SPL_ConditionalAverage_vfloatQ(      SPL_vfloatQ_t*  const Average_p,
                                              const SPL_vfloatQ_t*  const Y_p,
                                              const int16_t               rho,
                                              const int16_t*        const mask_p);

  SPL_RETURN_TYPE SPL_ConditionalCopy_vfloatQ(      SPL_vfloatQ_t*  const X_p,
                                           const SPL_vfloatQ_t*  const Y_p,
                                           const int16_t*        const mask_p);

  SPL_RETURN_TYPE SPL_Convert_sfloatQ_16bits(      SPL_sfloatQ_t* const Y_p,
                                         const SPL_sfloatQ_t* const X_p);

  SPL_RETURN_TYPE SPL_Convert_sfloatQ_vfloatQ_16bits(      SPL_vfloatQ_t* const Y_p,
                                           const SPL_sfloatQ_t* const X_p,
                                           const int16_t              length);

  SPL_RETURN_TYPE SPL_Convert_vfloatQ_16bits(      SPL_vfloatQ_t* const Y_p,
                                         const SPL_vfloatQ_t* const X_p);

  SPL_RETURN_TYPE SPL_Copy_vfloatQ(      SPL_vfloatQ_t*  const OutData_p,
                                const SPL_vfloatQ_t*  const InData_p);

  SPL_RETURN_TYPE SPL_Copy_vfloatQ_vint16(      SPL_vfloatQ_t*  const OutData_p,
                                       const int16_t*        const inData_p,
                                       const int16_t               start,
                                       const int16_t               length);

  SPL_RETURN_TYPE SPL_CountLeadingSignedBits_int16_sfloatQ(
                                       const SPL_sfloatQ_t*      const Flt_p,
                                             int16_t*            const norm_p);
    
  SPL_RETURN_TYPE SPL_CountLeadingSignedBits_int32_sfloatQ(
                                       const SPL_sfloatQ_t*      const Flt_p,
                                             int16_t*            const norm_p);

  SPL_RETURN_TYPE SPL_CountLeadingSignedBits_int16_vfloatQ(
                                       const SPL_vfloatQ_t*  const Flt_p,
                                             int16_t*        const norm_p);
    
  SPL_RETURN_TYPE SPL_CountLeadingSignedBits_int32_vfloatQ(
                                       const SPL_vfloatQ_t*  const Flt_p,
                                             int16_t*        const norm_p);

  SPL_RETURN_TYPE SPL_Div_sfloatQ(      SPL_sfloatQ_t*        Quotient_p,
                               const SPL_sfloatQ_t*  const Num_p,
                               const SPL_sfloatQ_t*  const Denom_p);

  SPL_RETURN_TYPE SPL_Div16Q_sfloatQ_sfloatQ(      SPL_sfloatQ_t*  const Quotient_p,
                                          const SPL_sfloatQ_t*  const Num_p, 
                                          const SPL_sfloatQ_t*  const Denom_p); 

  SPL_RETURN_TYPE SPL_Div_vfloatQ(      SPL_vfloatQ_t*  Quotient_p,
                               const SPL_vfloatQ_t*  Num_p, 
                               const SPL_vfloatQ_t*  Denom_p);

  SPL_RETURN_TYPE SPL_Div_vfloatQ_vfloatQ(      SPL_vfloatQ_t*  Quotient_p,
                                       const SPL_vfloatQ_t*  Num_p, 
                                       const SPL_vfloatQ_t*  Denom_p);

  SPL_RETURN_TYPE SPL_Div16Q_vfloatQ_vfloatQ(      SPL_vfloatQ_t*  Quotient_p,
                                          const SPL_vfloatQ_t*  Num_p, 
                                          const SPL_vfloatQ_t*  Denom_p); 

  SPL_RETURN_TYPE SPL_Exp_sfloatQ_int16(       SPL_sfloatQ_t* const Out_p,
                                      const int16_t              in,
                                      const SPL_Exp_Type_t       expType);

  SPL_RETURN_TYPE SPL_Exp_sfloatQ(      SPL_sfloatQ_t* const Out_p,
                               const SPL_sfloatQ_t* const in_p,
                               const SPL_Exp_Type_t       expType);
                      
  SPL_RETURN_TYPE SPL_Find_vfloatQ(      int16_t*        const Flag_p,
                                const SPL_vfloatQ_t*  const X_p,
                                const SPL_sfloatQ_t*  const Value_p,
                                const SPL_Compare_t         comparison);

  SPL_RETURN_TYPE SPL_Get_int16_sfloatQ(        int16_t*        const data_p,
                                     const SPL_sfloatQ_t*  const Data_p,
                                     const int16_t               Q);

  SPL_RETURN_TYPE SPL_Get_int32_sfloatQ(      int32_t*        const data_p,
                                     const SPL_sfloatQ_t*  const Data_p,
                                     const int16_t               Q);

  SPL_RETURN_TYPE SPL_Get_vint16_vfloatQ(      int16_t*        const data_p,
                                      const SPL_vfloatQ_t*  const Data_p,
                                      const int16_t               Q);

  SPL_RETURN_TYPE SPL_Insert_vfloatQ(      SPL_vfloatQ_t*  const OutVector_p,
                                  const SPL_vfloatQ_t*  const InVector_p,
                                  const SPL_sfloatQ_t*  const Value_p,
                                  const int16_t               index);

  SPL_RETURN_TYPE SPL_IsZeroVector_vfloatQ(      SPL_Boolean_t*  const IsZeroVector_p,
                                        const SPL_vfloatQ_t*  const X_p);
    
  SPL_RETURN_TYPE SPL_Limit_vfloatQ(      SPL_vfloatQ_t*  const X_limit_p,
                                 const SPL_vfloatQ_t*  const X_p,
                                 const SPL_sfloatQ_t*  const Limit_p,
                                 const SPL_Limit_t           comparison);

  SPL_RETURN_TYPE SPL_LimitNonNegative_vfloatQ(      SPL_vfloatQ_t*  const X_limit_p,
                                 const SPL_sfloatQ_t*  const Limit_p,
                                 const SPL_Limit_t           comparison);
  SPL_RETURN_TYPE SPL_Log_sfloatQ_int16(      SPL_sfloatQ_t* const Out_p,
                                     const int16_t              in,
                                     const SPL_Log_Type_t       logType);

  SPL_RETURN_TYPE SPL_Log_sfloatQ(      SPL_sfloatQ_t* const Out_p,
                               const SPL_sfloatQ_t* const In_p,
                               const SPL_Log_Type_t       logType);
                             
  SPL_RETURN_TYPE SPL_Log_vfloatQ(      SPL_vfloatQ_t* const Out_p,
                                  const SPL_vfloatQ_t* const In_p,
                                  const SPL_Log_Type_t       logType);

  SPL_RETURN_TYPE SPL_Mac_Cx_vfloatQ(      SPL_vfloatQ_t*        const Acc_p,
                                  const SPL_vfloatQ_t*        const X_p,
                                  const SPL_vfloatQ_t*        const Y_p,
                                  const SPL_Conjugate_Data_t        conjugate_Y);

  SPL_RETURN_TYPE SPL_Mask_vfloatQ(      SPL_vfloatQ_t*  const X_masked_p,
                                const SPL_vfloatQ_t*  const X_p,
                                const int16_t*        const mask_p);
                               
  SPL_RETURN_TYPE SPL_Max_sfloatQ(      SPL_sfloatQ_t*  const sum_p,
                               const SPL_sfloatQ_t*  const x_p,
                               const SPL_sfloatQ_t*  const y_p);

  SPL_RETURN_TYPE SPL_Max_vfloatQ(      SPL_sfloatQ_t*   const Max_p,
                                     int16_t*         const index_p,
                               const SPL_vfloatQ_t*   const X_p);

  SPL_RETURN_TYPE SPL_Max_Index_vfloatQ(      SPL_sfloatQ_t*   const Max_p,
                                     int16_t*         const index_p,
                               const SPL_vfloatQ_t*   const X_p);

  SPL_RETURN_TYPE SPL_Min_sfloatQ(      SPL_sfloatQ_t*  const Min_p,
                               const SPL_sfloatQ_t*  const X_p,
                               const SPL_sfloatQ_t*  const Y_p);

  SPL_RETURN_TYPE SPL_Min_vfloatQ(      SPL_sfloatQ_t*  const Min_p,
                                     int16_t*        const index_p,
                               const SPL_vfloatQ_t*  const X_p);

  SPL_RETURN_TYPE SPL_Mult_Cx_vfloatQ(      SPL_vfloatQ_t*        const Product_p,
                                   const SPL_vfloatQ_t*        const X_p,
                                   const SPL_vfloatQ_t*        const Y_p,
                                   const SPL_Conjugate_Data_t        conjugate_Y);

  SPL_RETURN_TYPE SPL_Mult_Cx_vfloatQ_R_vfloatQ(      SPL_vfloatQ_t*  const Product_p,
                                             const SPL_vfloatQ_t*  const X_p,
                                             const SPL_vfloatQ_t*  const Y_p,
                                             const int16_t               bandwidth_Y);

  SPL_RETURN_TYPE SPL_Mult_sfloatQ_int16(      SPL_sfloatQ_t*  const product_p,
                                      const SPL_sfloatQ_t*  const x_p,
                                      const int16_t               y);

  SPL_RETURN_TYPE SPL_Mult_sfloatQ(      SPL_sfloatQ_t*  const product_p,
                                const SPL_sfloatQ_t*  const x_p,
                                const SPL_sfloatQ_t*  const y_p);

  SPL_RETURN_TYPE SPL_Mult_vint16_sfloatQ(      int16_t*        const Product_p,
                                       const int16_t*        const X_p,
                                       const int16_t              length,
                                       const SPL_sfloatQ_t* const Factor_p);

  SPL_RETURN_TYPE SPL_Mult_vfloatQ(      SPL_vfloatQ_t*  const Product_p,
                                const SPL_vfloatQ_t*  const X_p,
                                const SPL_vfloatQ_t*  const Y_p);
                              
  SPL_RETURN_TYPE SPL_Mult_vfloatQ_sfloatQ(      SPL_vfloatQ_t*  const Product_p,
                                        const SPL_vfloatQ_t*  const X_p,
                                        const SPL_sfloatQ_t*  const Factor_p);
   
  SPL_RETURN_TYPE SPL_Mult_vfloatQ_vint16(      SPL_vfloatQ_t*  const Product_p,
                                       const SPL_vfloatQ_t*  const X_p,
                                       const int16_t*        const Y_p,
                                       const int16_t               length);
   
  SPL_RETURN_TYPE SPL_Mult_vfloatQ_vfloatQ(      SPL_vfloatQ_t*  const Product_p,
                                        const SPL_vfloatQ_t*  const X_p,
                                        const SPL_vfloatQ_t*  const Y_p);
                                              
  SPL_RETURN_TYPE SPL_Resample_vfloatQ(      SPL_vfloatQ_t* const X_resampled_p,
                                    const SPL_vfloatQ_t* const X_p,
                                    const int16_t              resampleFactor);

  SPL_RETURN_TYPE SPL_ScalarProduct_sfloatQ_vint16(      
                                            SPL_sfloatQ_t*  const ScalarProduct_p,
                                      const int16_t*        const x_p,
                                      const int16_t*        const y_p,
                                      const int16_t               length);
  SPL_RETURN_TYPE SPL_ScalarProduct_vfloatQ(      SPL_sfloatQ_t*  const ScalarProduct_p,
                                         const SPL_vfloatQ_t*  const X_p,
                                         const SPL_vfloatQ_t*  const Y_p);

  SPL_RETURN_TYPE SPL_Set_vfloatQ(      SPL_vfloatQ_t*     const Data_p,
                               const SPL_Float_t        value);

  SPL_RETURN_TYPE SPL_Set_vfloatQ_sfloatQ(      SPL_vfloatQ_t*  const Data_p,
                                       const SPL_sfloatQ_t*  const value_p);
    
  SPL_RETURN_TYPE SPL_SetQ_int16_sfloatQ(       SPL_sfloatQ_t*  const x_p,
                                      const int16_t                Q);

  SPL_RETURN_TYPE SPL_SetQ_int32_sfloatQ(       SPL_sfloatQ_t*  const x_p,
                                      const int16_t                Q);

  SPL_RETURN_TYPE SPL_SetQ_int16_vfloatQ(      SPL_vfloatQ_t*  const X_p,
                                      const int16_t               Q);

  SPL_RETURN_TYPE SPL_SetQ_int32_vfloatQ(      SPL_vfloatQ_t*  const X_p,
                                      const int16_t               Q);

  SPL_RETURN_TYPE SPL_Sqrt_sfloatQ(      SPL_sfloatQ_t*  X_Root_p,
                                const SPL_sfloatQ_t*  X_p);

  SPL_RETURN_TYPE SPL_Sqrt_vfloatQ(      SPL_vfloatQ_t*  X_Root_p,
                                const SPL_vfloatQ_t*  X_p);

  SPL_RETURN_TYPE SPL_SquaredSum_vint16(      SPL_sfloatQ_t*  const SquaredSum_p,
                                     const int16_t*        const x_p,
                                     const int16_t               length);

  SPL_RETURN_TYPE SPL_SquaredSum_vfloatQ(      SPL_sfloatQ_t*  const SquaredSum_p,
                                      const SPL_vfloatQ_t*  const X_p,
                                      const int16_t               start,
                                      const int16_t               length);

  SPL_RETURN_TYPE SPL_Sub_vint16_vfloatQ(      int16_t*        const Diff_p,
                                      const int16_t*        const X_p, 
                                      const SPL_vfloatQ_t*  const Y_p);
   
  SPL_RETURN_TYPE SPL_Sub_vfloatQ(      SPL_vfloatQ_t*  const Diff_p,
                               const SPL_vfloatQ_t*  const X_p,
                               const SPL_vfloatQ_t*  const Y_p);

  SPL_RETURN_TYPE SPL_Sub_vfloatQ_sfloatQ(      SPL_vfloatQ_t*  const Sum_p,
                                       const SPL_vfloatQ_t*  const X_p,
                                       const SPL_sfloatQ_t*  const Term_p);

  SPL_RETURN_TYPE SPL_Sub_vfloatQ_vint16(      SPL_vfloatQ_t*  const Diff_p,
                                      const SPL_vfloatQ_t*  const X_p,
                                      const int16_t*        const Y_p);

  SPL_RETURN_TYPE SPL_Sub_sfloatQ(      SPL_sfloatQ_t*  const sum_p,
                               const SPL_sfloatQ_t*  const x_p,
                               const SPL_sfloatQ_t*  const y_p);

  SPL_RETURN_TYPE SPL_Sum_vfloatQ(      SPL_sfloatQ_t*  const X_Summed_p,
                               const SPL_vfloatQ_t*  const X_p,
                               const int16_t               start,
                               const int16_t               length);

  SPL_RETURN_TYPE SPL_SumElements_vfloatQ(      SPL_vfloatQ_t*  const X_Summed_p,
                                       const SPL_vfloatQ_t*  const X_p,
                                       const int16_t               start,
                                       const int16_t               length,
                                       const int16_t*              bandwidth_p);

  SPL_RETURN_TYPE SPL_Vmax_vfloatQ(      SPL_vfloatQ_t*  const Max_p,
                                const SPL_vfloatQ_t*  const X_p,
                                const SPL_vfloatQ_t*  const Y_p);

  SPL_RETURN_TYPE SPL_Vmin_vfloatQ(      SPL_vfloatQ_t*  const Min_p,
                                const SPL_vfloatQ_t*  const X_p,
                                const SPL_vfloatQ_t*  const Y_p);

#ifndef SPL_VERIFY_CALLS
  /* This is done in spl_verified_calls.h if defined */
  /* Map the fixed point mantissa functions to the corresponding floating point */
  #define SPL_AbsSquare_Cx_vint16Q   SPL_AbsSquare_Cx_vfloatQ
  #define SPL_AbsSquare_Cx_vint32Q   SPL_AbsSquare_Cx_vfloatQ

  #define SPL_Add_sint16Q            SPL_Add_sfloatQ
  #define SPL_Add_sint32Q            SPL_Add_sfloatQ

  #define SPL_Add_vint16Q            SPL_Add_vfloatQ
  #define SPL_Add_vint32Q            SPL_Add_vfloatQ

  #define SPL_Add_vint16Q_sint16Q    SPL_Add_vfloatQ_sfloatQ
  #define SPL_Add_vint32Q_sint16Q    SPL_Add_vfloatQ_sfloatQ
  #define SPL_Add_vint32Q_sint32Q    SPL_Add_vfloatQ_sfloatQ

  #define SPL_Average_sint16Q        SPL_Average_sfloatQ

  #define SPL_Average_vint16Q        SPL_Average_vfloatQ
  #define SPL_Average_vint32Q        SPL_Average_vfloatQ

  #define SPL_AverageElements_vint32Q    SPL_AverageElements_vfloatQ

  #define SPL_BlockMax_vint32Q       SPL_BlockMax_vfloatQ

  #define SPL_BlockSum_vint16Q       SPL_BlockSum_vfloatQ
  #define SPL_BlockSum_vint32Q       SPL_BlockSum_vfloatQ

  #define SPL_Compare_sint16Q        SPL_Compare_sfloatQ
  #define SPL_Compare_sint32Q        SPL_Compare_sfloatQ

  #define SPL_Compare_vint16Q        SPL_Compare_vfloatQ
  #define SPL_Compare_vint32Q        SPL_Compare_vfloatQ

  #define SPL_ConditionalAverage_vint16Q SPL_ConditionalAverage_vfloatQ

  #define SPL_ConditionalCopy_vint16Q    SPL_ConditionalCopy_vfloatQ
  #define SPL_ConditionalCopy_vint32Q    SPL_ConditionalCopy_vfloatQ

  #define SPL_Convert_sint32Q_sint16Q    SPL_Convert_sfloatQ_16bits

  #define SPL_Convert_sint32Q_vint16Q    SPL_Convert_sfloatQ_vfloatQ_16bits

  #define SPL_Convert_vint32Q_vint16Q    SPL_Convert_vfloatQ_16bits

  #define SPL_Copy_vint16Q           SPL_Copy_vfloatQ
  #define SPL_Copy_vint32Q           SPL_Copy_vfloatQ

  #define SPL_Copy_vint16Q_vint16    SPL_Copy_vfloatQ_vint16
  #define SPL_Copy_vint32Q_vint16    SPL_Copy_vfloatQ_vint16

  #define SPL_CountLeadingSignedBits_sint16Q   SPL_CountLeadingSignedBits_int16_sfloatQ

  #define SPL_CountLeadingSignedBits_vint16Q   SPL_CountLeadingSignedBits_int16_vfloatQ
 
  #define SPL_CountLeadingSignedBits_sint32Q   SPL_CountLeadingSignedBits_int32_sfloatQ

  #define SPL_CountLeadingSignedBits_vint32Q   SPL_CountLeadingSignedBits_int32_vfloatQ

  #define SPL_Div_sint16Q              SPL_Div_sfloatQ
  #define SPL_Div16Q_sint32Q_sint32Q   SPL_Div_sfloatQ

  #define SPL_Div_vint16Q              SPL_Div_vfloatQ
  #define SPL_Div_vint16Q_vint32Q      SPL_Div_vfloatQ
  #define SPL_Div16Q_vint32Q_vint32Q   SPL_Div_vfloatQ

  #define SPL_Exp_sint16Q_int16      SPL_Exp_sfloatQ_int16

  #define SPL_Exp_sint16Q_sint16Q    SPL_Exp_sfloatQ

  #define SPL_Find_vint16Q           SPL_Find_vfloatQ
  #define SPL_Find_vint32Q           SPL_Find_vfloatQ

  #define SPL_Insert_vint16Q         SPL_Insert_vfloatQ
  #define SPL_Insert_vint32Q         SPL_Insert_vfloatQ

  #define SPL_IsZeroVector_vint16Q   SPL_IsZeroVector_vfloatQ
  #define SPL_IsZeroVector_vint32Q   SPL_IsZeroVector_vfloatQ

  #define SPL_Get_int16              SPL_Get_int16_sfloatQ

  #define SPL_Get_int32              SPL_Get_int32_sfloatQ

  #define SPL_Get_vint16             SPL_Get_vint16_vfloatQ

  #define SPL_LimitNonNegative_vint16Q  SPL_LimitNonNegative_vfloatQ
  #define SPL_LimitNonNegative_vint32Q  SPL_LimitNonNegative_vfloatQ
  #define SPL_Limit_vint16Q          SPL_Limit_vfloatQ
  #define SPL_Limit_vint32Q          SPL_Limit_vfloatQ

  #define SPL_Log_sint16Q_sint16Q    SPL_Log_sfloatQ

  #define SPL_Log_sint16Q_int16      SPL_Log_sfloatQ_int16

  #define SPL_Log_vint16Q_vint32Q    SPL_Log_vfloatQ

  #define SPL_Mac_Cx_vint16Q         SPL_Mac_Cx_vfloatQ

  #define SPL_Mask_vint16Q           SPL_Mask_vfloatQ
  #define SPL_Mask_vint32Q           SPL_Mask_vfloatQ

  #define SPL_Max_sint16Q            SPL_Max_sfloatQ
  #define SPL_Max_sint32Q            SPL_Max_sfloatQ

  #define SPL_Max_vint16Q            SPL_Max_vfloatQ
  #define SPL_Max_vint32Q            SPL_Max_vfloatQ

  #define SPL_Max_Index_vint16Q      SPL_Max_Index_vfloatQ
  #define SPL_Max_Index_vint32Q      SPL_Max_Index_vfloatQ

  #define SPL_Min_sint16Q            SPL_Min_sfloatQ
  #define SPL_Min_sint32Q            SPL_Min_sfloatQ

  #define SPL_Min_vint16Q            SPL_Min_vfloatQ
  #define SPL_Min_vint32Q            SPL_Min_vfloatQ

  #define SPL_Mult_Cx_vint16Q        SPL_Mult_Cx_vfloatQ

  #define SPL_Mult_Cx_vint16Q_R_vint16Q      SPL_Mult_Cx_vfloatQ_R_vfloatQ

  #define SPL_Mult_sint16Q_int16     SPL_Mult_sfloatQ_int16
  #define SPL_Mult_sint32Q_int16     SPL_Mult_sfloatQ_int16

  #define SPL_Mult_sint16Q_sint16Q   SPL_Mult_sfloatQ
  #define SPL_Mult_sint32Q_sint16Q   SPL_Mult_sfloatQ

  #define SPL_Mult_vint16_sint16Q    SPL_Mult_vint16_sfloatQ

  #define SPL_Mult_vint32Q           SPL_Mult_vfloatQ
  #define SPL_Mult_vint16Q           SPL_Mult_vfloatQ

  #define SPL_Mult_vint16Q_sint16Q   SPL_Mult_vfloatQ_sfloatQ
  #define SPL_Mult_vint32Q_sint16Q   SPL_Mult_vfloatQ_sfloatQ

  #define SPL_Mult_vint16Q_vint16    SPL_Mult_vfloatQ_vint16
  #define SPL_Mult_vint32Q_vint16    SPL_Mult_vfloatQ_vint16

  #define SPL_Mult_vint16Q_vint32Q   SPL_Mult_vfloatQ
  #define SPL_Mult_vint32Q_vint16Q   SPL_Mult_vfloatQ

  #define SPL_Resample_vint16Q       SPL_Resample_vfloatQ

  #define SPL_ScalarProduct_vint16   SPL_ScalarProduct_sfloatQ_vint16
  #define SPL_ScalarProduct_vint16Q  SPL_ScalarProduct_vfloatQ

  #define SPL_Set_vint16Q            SPL_Set_vfloatQ
  #define SPL_Set_vint32Q            SPL_Set_vfloatQ

  #define SPL_Set_vint16Q_sint16Q    SPL_Set_vfloatQ_sfloatQ
  #define SPL_Set_vint32Q_sint32Q    SPL_Set_vfloatQ_sfloatQ

  #define SPL_SetQ_sint16Q           SPL_SetQ_int16_sfloatQ

  #define SPL_SetQ_sint32Q           SPL_SetQ_int32_sfloatQ

  #define SPL_SetQ_vint16Q           SPL_SetQ_int16_vfloatQ

  #define SPL_SetQ_vint32Q           SPL_SetQ_int32_vfloatQ

  #define SPL_Sqrt_vint16Q           SPL_Sqrt_vfloatQ
  #define SPL_Sqrt_vint32Q           SPL_Sqrt_vfloatQ

  #define SPL_Sqrt_sint16Q           SPL_Sqrt_sfloatQ

  //#define SPL_SquaredSum_vint16      SPL_SquaredSum_vint16

  #define SPL_SquaredSum_vint16Q     SPL_SquaredSum_vfloatQ

  #define SPL_Sub_vint16_vint16Q     SPL_Sub_vint16_vfloatQ

  #define SPL_Sub_vint16Q            SPL_Sub_vfloatQ
  #define SPL_Sub_vint32Q            SPL_Sub_vfloatQ

  #define SPL_Sub_vint16Q_sint16Q    SPL_Sub_vfloatQ_sfloatQ

  #define SPL_Sub_vint16Q_vint16     SPL_Sub_vfloatQ_vint16

  #define SPL_Sub_sint16Q            SPL_Sub_sfloatQ
  #define SPL_Sub_sint32Q            SPL_Sub_sfloatQ

  #define SPL_Sum_vint16Q            SPL_Sum_vfloatQ
  #define SPL_Sum_vint32Q            SPL_Sum_vfloatQ

  #define SPL_SumElements_vint32Q    SPL_SumElements_vfloatQ

  #define SPL_Vmax_vint16Q           SPL_Vmax_vfloatQ
  #define SPL_Vmax_vint32Q           SPL_Vmax_vfloatQ

  #define SPL_Vmin_vint16Q           SPL_Vmin_vfloatQ
  #define SPL_Vmin_vint32Q           SPL_Vmin_vfloatQ
#endif //SPL_VERIFY_CALLS
#else

  SPL_RETURN_TYPE SPL_AbsSquare_Cx_vint16Q(      SPL_vint32Q_t*  const X_Squared_p,
                                        const SPL_vint16Q_t*  const X_p);

  SPL_RETURN_TYPE SPL_AbsSquare_Cx_vint32Q(      SPL_vint32Q_t*  const X_Squared_p,
                                        const SPL_vint32Q_t*  const X_p);

  SPL_RETURN_TYPE SPL_Add_sint16Q(      SPL_sint16Q_t*  const sum_p,
                               const SPL_sint16Q_t*  const x_p,
                               const SPL_sint16Q_t*  const y_p);

  SPL_RETURN_TYPE SPL_Add_sint32Q(      SPL_sint32Q_t*  const sum_p,
                               const SPL_sint32Q_t*  const x_p,
                               const SPL_sint32Q_t*  const y_p);

  SPL_RETURN_TYPE SPL_Add_vint16Q(      SPL_vint16Q_t*  const Sum_p,
                               const SPL_vint16Q_t*  const X_p,
                               const SPL_vint16Q_t*  const Y_p);

  SPL_RETURN_TYPE SPL_Add_vint32Q(      SPL_vint32Q_t*  const Sum_p,
                               const SPL_vint32Q_t*  const X_p,
                               const SPL_vint32Q_t*  const Y_p);

  SPL_RETURN_TYPE SPL_Add_vint16Q_sint16Q(      SPL_vint16Q_t*  const Sum_p,
                                       const SPL_vint16Q_t*  const X_p,
                                       const SPL_sint16Q_t*  const Term_p);

  SPL_RETURN_TYPE SPL_Add_vint32Q_sint16Q(      SPL_vint32Q_t*  const Sum_p,
                                       const SPL_vint32Q_t*  const X_p,
                                       const SPL_sint16Q_t*  const Term_p);

  SPL_RETURN_TYPE SPL_Add_vint32Q_sint32Q(      SPL_vint32Q_t*  const Sum_p,
                                       const SPL_vint32Q_t*  const X_p,
                                       const SPL_sint32Q_t*  const Term_p);
                            
  SPL_RETURN_TYPE SPL_Average_sint16Q(      SPL_sint16Q_t*  const Average_p,
                                   const SPL_sint16Q_t*  const X_p,
                                   const SPL_sint16Q_t*  const Y_p,
                                   const SPL_sint16Q_t*  const Rho_p);


  SPL_RETURN_TYPE SPL_Average_vint16Q(      SPL_vint16Q_t*  const Average_p,
                                   const SPL_vint16Q_t*  const X_p,
                                   const SPL_vint16Q_t*  const Y_p,
                                   const SPL_vint16Q_t*  const Rho_p);


  SPL_RETURN_TYPE SPL_Average_vint32Q(      SPL_vint32Q_t*  const Average_p,
                                   const SPL_vint32Q_t*  const X_p,
                                   const SPL_vint32Q_t*  const Y_p,
                                   const SPL_vint16Q_t*  const Rho_p);

  SPL_RETURN_TYPE SPL_AverageElements_vint32Q(      SPL_vint32Q_t*  const X_Averaged_p,
                                           const SPL_vint32Q_t*  const X_p,
                                           const int16_t               start,
                                           const int16_t               length,
                                           const int16_t*              bandwidthNumBits_p);


  SPL_RETURN_TYPE SPL_BlockMax_vint32Q(      SPL_vint32Q_t*  const Max_p,
                                            int16_t*        const index_p,
                                      const SPL_vint32Q_t*  const X_p,
                                      const int16_t               bandwidth);

  SPL_RETURN_TYPE SPL_BlockSum_vint16Q(      SPL_vint16Q_t*  const X_Summed_p,
                                    const SPL_vint16Q_t*  const X_p,
                                    const int16_t               start,
                                    const int16_t               length,
                                    const int16_t               bandwidth);

  SPL_RETURN_TYPE SPL_BlockSum_vint32Q(      SPL_vint32Q_t*  const X_Summed_p,
                                    const SPL_vint32Q_t*  const X_p,
                                    const int16_t               start,
                                    const int16_t               length,
                                    const int16_t               bandwidth);

  SPL_RETURN_TYPE SPL_Compare_sint16Q(      SPL_Boolean_t*   const Flag_p,
                                   const SPL_sint16Q_t*   const X_p,
                                   const SPL_sint16Q_t*   const Y_p,
                                   const SPL_Compare_t          comparison);

  SPL_RETURN_TYPE SPL_Compare_sint32Q(      SPL_Boolean_t*   const Flag_p,
                                   const SPL_sint32Q_t*   const X_p,
                                   const SPL_sint32Q_t*   const Y_p,
                                   const SPL_Compare_t          comparison);

  SPL_RETURN_TYPE SPL_Compare_vint16Q(      int16_t*         const Flag_p,
                                   const SPL_vint16Q_t*   const X_p,
                                   const SPL_vint16Q_t*   const Y_p,
                                   const SPL_Compare_t          comparison);

  SPL_RETURN_TYPE SPL_Compare_vint32Q(      int16_t*        const Flag_p,
                                   const SPL_vint32Q_t*  const X_p,
                                   const SPL_vint32Q_t*  const Y_p,
                                   const SPL_Compare_t         comparison);

  SPL_RETURN_TYPE SPL_ConditionalAverage_vint16Q(      SPL_vint16Q_t*  const Average_p,
                                              const SPL_vint16Q_t*  const Y_p,
                                              const int16_t               rho,
                                              const int16_t*        const mask_p);

  SPL_RETURN_TYPE SPL_ConditionalCopy_vint16Q(      SPL_vint16Q_t*  const X_p,
                                           const SPL_vint16Q_t*  const Y_p,
                                           const int16_t*        const mask_p);

  SPL_RETURN_TYPE SPL_ConditionalCopy_vint32Q(      SPL_vint32Q_t*  const X_p,
                                           const SPL_vint32Q_t*  const Y_p,
                                           const int16_t*        const mask_p);

  SPL_RETURN_TYPE SPL_Convert_sint32Q_sint16Q(      SPL_sint16Q_t* const Y_p,
                                           const SPL_sint32Q_t* const X_p);

  SPL_RETURN_TYPE SPL_Convert_sint32Q_vint16Q(      SPL_vint16Q_t* const Y_p,
                                           const SPL_sint32Q_t* const X_p,
                                           const int16_t              length);

  SPL_RETURN_TYPE SPL_Convert_vint32Q_vint16Q(      SPL_vint16Q_t* const Y_p,
                                           const SPL_vint32Q_t* const X_p);

  SPL_RETURN_TYPE SPL_Copy_vint16Q(      SPL_vint16Q_t*  const OutData_p,
                                const SPL_vint16Q_t*  const InData_p);

  SPL_RETURN_TYPE SPL_Copy_vint16Q_vint16(      SPL_vint16Q_t*  const OutData_p,
                                       const int16_t*        const inData_p,
                                       const int16_t               start,
                                       const int16_t               length);

  SPL_RETURN_TYPE SPL_Copy_vint32Q(      SPL_vint32Q_t*  const OutData_p,
                                const SPL_vint32Q_t*  const InData_p);

  SPL_RETURN_TYPE SPL_Copy_vint32Q_vint16(      SPL_vint32Q_t*  const OutData_p,
                                       const int16_t*        const inData_p,
                                       const int16_t               start,
                                       const int16_t               length);

  SPL_RETURN_TYPE SPL_CountLeadingSignedBits_sint16Q(
                                       const SPL_sint16Q_t*       const Flt_p,
                                             int16_t*             const norm_p);

  SPL_RETURN_TYPE SPL_CountLeadingSignedBits_sint32Q(
                                       const SPL_sint32Q_t*      const Flt_p,
                                             int16_t*            const norm_p);
    
  SPL_RETURN_TYPE SPL_CountLeadingSignedBits_vint16Q(
                                       const SPL_vint16Q_t*      const Flt_p,
                                             int16_t*        const norm_p);
    
  SPL_RETURN_TYPE SPL_CountLeadingSignedBits_vint32Q(
                                       const SPL_vint32Q_t*      const Flt_p,
                                             int16_t*            const norm_p);

  SPL_RETURN_TYPE SPL_Div_sint16Q(      SPL_sint16Q_t*        Quotient_p,
                               const SPL_sint16Q_t*  const Num_p,
                               const SPL_sint16Q_t*  const Denom_p);

  SPL_RETURN_TYPE SPL_Div16Q_sint32Q_sint32Q(      SPL_sint16Q_t*  const Quotient_p,
                                          const SPL_sint32Q_t*  const Num_p, 
                                          const SPL_sint32Q_t*  const Denom_p); 

  SPL_RETURN_TYPE SPL_Div_vint16Q(      SPL_vint16Q_t*  Quotient_p,
                               const SPL_vint16Q_t*  Num_p, 
                               const SPL_vint16Q_t*  Denom_p);

  SPL_RETURN_TYPE SPL_Div_vint16Q_vint32Q(      SPL_vint16Q_t*  Quotient_p,
                                       const SPL_vint16Q_t*  Num_p, 
                                       const SPL_vint32Q_t*  Denom_p);

  SPL_RETURN_TYPE SPL_Div16Q_vint32Q_vint32Q(      SPL_vint16Q_t*  Quotient_p,
                                          const SPL_vint32Q_t*  Num_p, 
                                          const SPL_vint32Q_t*  Denom_p); 

  SPL_RETURN_TYPE SPL_Exp_sint16Q_int16(       SPL_sint16Q_t* const Out_p,
                                      const int16_t              in,
                                      const SPL_Exp_Type_t       expType);

  SPL_RETURN_TYPE SPL_Exp_sint16Q_sint16Q(      SPL_sint16Q_t* const Out_p,
                                       const SPL_sint16Q_t* const in_p,
                                       const SPL_Exp_Type_t       expType);
                      
  SPL_RETURN_TYPE SPL_Find_vint16Q(      int16_t*        const Flag_p,
                                const SPL_vint16Q_t*  const X_p,
                                const SPL_sint16Q_t*  const Value_p,
                                const SPL_Compare_t         comparison);

  SPL_RETURN_TYPE SPL_Find_vint32Q(      int16_t*        const Flag_p,
                                const SPL_vint32Q_t*  const X_p,
                                const SPL_sint32Q_t*  const Value_p,
                                const SPL_Compare_t         comparison);

  SPL_RETURN_TYPE SPL_Insert_vint16Q(      SPL_vint16Q_t*  const OutVector_p,
                                  const SPL_vint16Q_t*  const InVector_p,
                                  const SPL_sint16Q_t*  const Value_p,
                                  const int16_t               index);

  SPL_RETURN_TYPE SPL_Insert_vint32Q(      SPL_vint32Q_t*  const OutVector_p,
                                  const SPL_vint32Q_t*  const InVector_p,
                                  const SPL_sint32Q_t*  const Value_p,
                                  const int16_t               index);

  SPL_RETURN_TYPE SPL_IsZeroVector_vint16Q(      SPL_Boolean_t*  const IsZeroVector_p,
                                        const SPL_vint16Q_t*  const X_p);
    
  SPL_RETURN_TYPE SPL_IsZeroVector_vint32Q(      SPL_Boolean_t*  const IsZeroVector_p,
                                        const SPL_vint32Q_t*  const X_p);

  SPL_RETURN_TYPE SPL_Get_int16(        int16_t*        const data_p,
                               const SPL_sint16Q_t*  const Data_p,
                               const int16_t               Q);

  SPL_RETURN_TYPE SPL_Get_int32(      int32_t*        const data_p,
                             const SPL_sint32Q_t*  const Data_p,
                             const int16_t               Q);

  SPL_RETURN_TYPE SPL_Get_vint16(      int16_t*        const data_p,
                              const SPL_vint16Q_t*  const Data_p,
                              const int16_t               Q);

  SPL_RETURN_TYPE SPL_Limit_vint16Q(      SPL_vint16Q_t*  const X_limit_p,
                                 const SPL_vint16Q_t*  const X_p,
                                 const SPL_sint16Q_t*  const Limit_p,
                                 const SPL_Limit_t           comparison);

  SPL_RETURN_TYPE SPL_Limit_vint32Q(      SPL_vint32Q_t*  const X_limit_p,
                                 const SPL_vint32Q_t*  const X_p,
                                 const SPL_sint32Q_t*  const Limit_p,
                                 const SPL_Limit_t           comparison);

  SPL_Return_t SPL_LimitNonNegative_vint16Q(      SPL_vint16Q_t*  const X_p,
                                            const SPL_sint16Q_t*  const Limit_p,
                                            const SPL_Limit_t           comparison);
  SPL_Return_t SPL_LimitNonNegative_vint32Q(      SPL_vint32Q_t*  const X_p,
                                            const SPL_sint32Q_t*  const Limit_p,
                                            const SPL_Limit_t           comparison);
  SPL_RETURN_TYPE SPL_Log_sint16Q_sint16Q(      SPL_sint16Q_t* const Out_p,
                                       const SPL_sint16Q_t* const In_p,
                                       const SPL_Log_Type_t       logType);

  SPL_RETURN_TYPE SPL_Log_sint16Q_int16(      SPL_sint16Q_t* const Out_p,
                                     const int16_t              in,
                                     const SPL_Log_Type_t       logType);
                             
  SPL_RETURN_TYPE SPL_Log_vint16Q_vint32Q(      SPL_vint16Q_t* const Out_p,
                                     const SPL_vint32Q_t* const In_p,
                                     const SPL_Log_Type_t       logType);

  SPL_RETURN_TYPE SPL_Mac_Cx_vint16Q(      SPL_vint16Q_t*        const Acc_p,
                                  const SPL_vint16Q_t*        const X_p,
                                  const SPL_vint16Q_t*        const Y_p,
                                  const SPL_Conjugate_Data_t        conjugate_Y);

  SPL_RETURN_TYPE SPL_Mask_vint16Q(      SPL_vint16Q_t*   const X_masked_p,
                                const SPL_vint16Q_t*   const X_p,
                                const int16_t*         const mask_p);

  SPL_RETURN_TYPE SPL_Mask_vint32Q(      SPL_vint32Q_t*  const X_masked_p,
                                const SPL_vint32Q_t*  const X_p,
                                const int16_t*        const mask_p);

  SPL_RETURN_TYPE SPL_Max_sint16Q(      SPL_sint16Q_t*  const Max_p,
                               const SPL_sint16Q_t*  const X_p,
                               const SPL_sint16Q_t*  const Y_p);

  SPL_RETURN_TYPE SPL_Max_sint32Q(      SPL_sint32Q_t*  const sum_p,
                               const SPL_sint32Q_t*  const x_p,
                               const SPL_sint32Q_t*  const y_p);

  SPL_RETURN_TYPE SPL_Max_vint16Q(      SPL_sint16Q_t*   const Max_p,
                                     int16_t*         const index_p,
                               const SPL_vint16Q_t*   const X_p);

  SPL_RETURN_TYPE SPL_Max_vint32Q(      SPL_sint32Q_t*   const Max_p,
                                     int16_t*         const index_p,
                               const SPL_vint32Q_t*   const X_p);

  SPL_RETURN_TYPE SPL_Min_sint16Q(      SPL_sint16Q_t*  const Min_p,
                               const SPL_sint16Q_t*  const X_p,
                               const SPL_sint16Q_t*  const Y_p);

  SPL_RETURN_TYPE SPL_Min_sint32Q(      SPL_sint32Q_t*  const Max_p,
                               const SPL_sint32Q_t*  const X_p,
                               const SPL_sint32Q_t*  const Y_p);

  SPL_RETURN_TYPE SPL_Min_vint16Q(      SPL_sint16Q_t*  const Min_p,
                                     int16_t*        const index_p,
                               const SPL_vint16Q_t*  const X_p);

  SPL_RETURN_TYPE SPL_Min_vint32Q(      SPL_sint32Q_t*   const Min_p,
                                     int16_t*         const index_p,
                               const SPL_vint32Q_t*   const X_p);

  SPL_RETURN_TYPE SPL_Mult_Cx_vint16Q(      SPL_vint32Q_t*        const Product_p,
                                   const SPL_vint16Q_t*        const X_p,
                                   const SPL_vint16Q_t*        const Y_p,
                                   const SPL_Conjugate_Data_t        conjugate_Y);

  SPL_RETURN_TYPE SPL_Mult_Cx_vint16Q_R_vint16Q(      SPL_vint16Q_t*  const Product_p,
                                             const SPL_vint16Q_t*  const X_p,
                                             const SPL_vint16Q_t*  const Y_p,
                                             const int16_t               bandwidth_Y);

  SPL_RETURN_TYPE SPL_Mult_sint16Q_int16(      SPL_sint16Q_t*  const product_p,
                                      const SPL_sint16Q_t*  const x_p,
                                      const int16_t               y);

  SPL_RETURN_TYPE SPL_Mult_sint16Q_sint16Q(      SPL_sint16Q_t*  const product_p,
                                        const SPL_sint16Q_t*  const x_p,
                                        const SPL_sint16Q_t*  const y_p);

  SPL_RETURN_TYPE SPL_Mult_sint32Q_int16(      SPL_sint32Q_t*  const product_p,
                                      const SPL_sint32Q_t*  const x_p,
                                      const int16_t               y);

  SPL_RETURN_TYPE SPL_Mult_sint32Q_sint16Q(      SPL_sint32Q_t*  const product_p,
                                        const SPL_sint32Q_t*  const x_p,
                                        const SPL_sint16Q_t*  const y_p);

  SPL_RETURN_TYPE SPL_Mult_vint16_sint16Q(      int16_t*        const Product_p,
                                       const int16_t*        const X_p,
                                       const int16_t              length,
                                       const SPL_sint16Q_t* const Factor_p);

  SPL_RETURN_TYPE SPL_Mult_vint32Q(      SPL_vint32Q_t*  const Product_p,
                                const SPL_vint32Q_t*  const X_p,
                                const SPL_vint32Q_t*  const Y_p);

  SPL_RETURN_TYPE SPL_Mult_vint16Q(      SPL_vint16Q_t*  const Product_p,
                                const SPL_vint16Q_t*  const X_p,
                                const SPL_vint16Q_t*  const Y_p);
                               
  SPL_RETURN_TYPE SPL_Mult_vint16Q_sint16Q(      SPL_vint16Q_t*  const Product_p,
                                        const SPL_vint16Q_t*  const X_p,
                                        const SPL_sint16Q_t*  const Factor_p);
   
  SPL_RETURN_TYPE SPL_Mult_vint16Q_vint16(      SPL_vint16Q_t*  const Product_p,
                                       const SPL_vint16Q_t*  const X_p,
                                       const int16_t*        const Y_p,
                                       const int16_t               length);
   
  SPL_RETURN_TYPE SPL_Mult_vint16Q_vint32Q(      SPL_vint16Q_t*  const Product_p,
                                        const SPL_vint16Q_t*  const X_p,
                                        const SPL_vint32Q_t*  const Y_p);
                                              
  SPL_RETURN_TYPE SPL_Mult_vint32Q_sint16Q(      SPL_vint32Q_t*  const Product_p,
                                        const SPL_vint32Q_t*  const X_p,
                                        const SPL_sint16Q_t*  const Factor_p);

  SPL_RETURN_TYPE SPL_Mult_vint32Q_sint32Q(      SPL_vint32Q_t*  const Product_p,
                                        const SPL_vint32Q_t*  const X_p,
                                        const SPL_sint32Q_t*  const Factor_p);

  SPL_RETURN_TYPE SPL_Mult_vint32Q_vint16(      SPL_vint32Q_t*  const Product_p,
                                       const SPL_vint32Q_t*  const X_p,
                                       const int16_t*        const Y_p,
                                       const int16_t               length);

  SPL_RETURN_TYPE SPL_Mult_vint32Q_vint16Q(      SPL_vint32Q_t*  const Product_p,
                                        const SPL_vint32Q_t*  const X_p,
                                        const SPL_vint16Q_t*  const Y_p);

  SPL_RETURN_TYPE SPL_Resample_vint16Q(      SPL_vint16Q_t* const X_resampled_p,
                                    const SPL_vint16Q_t* const X_p,
                                    const int16_t              resampleFactor);

  SPL_RETURN_TYPE SPL_ScalarProduct_vint16(      SPL_sint16Q_t*  const ScalarProduct_p,
                                        const int16_t*        const x_p,
                                        const int16_t*        const y_p,
                                        const int16_t               length);

  SPL_Return_t SPL_ScalarProduct_vint16Q(      SPL_sint16Q_t*  const ScalarProduct_p,
                                         const SPL_vint16Q_t*  const X_p,
                                         const SPL_vint16Q_t*  const Y_p);
  SPL_RETURN_TYPE SPL_Set_vint16Q(      SPL_vint16Q_t*     const Data_p,
                               const SPL_int16QData_t         value);

  SPL_RETURN_TYPE SPL_Set_vint16Q_sint16Q(      SPL_vint16Q_t*  const Data_p,
                                       const SPL_sint16Q_t*  const value_p);
    
  SPL_RETURN_TYPE SPL_Set_vint32Q(      SPL_vint32Q_t*     const Data_p,
                               const SPL_int32QData_t         value);

  SPL_RETURN_TYPE SPL_Set_vint32Q_sint32Q(      SPL_vint32Q_t*  const Data_p,
                                       const SPL_sint32Q_t*  const value_p);

  SPL_RETURN_TYPE SPL_SetQ_sint16Q(      SPL_sint16Q_t*  const x_p,
                                const int16_t               Q);

  SPL_RETURN_TYPE SPL_SetQ_sint32Q(      SPL_sint32Q_t*  const X_p,
                                const int16_t               Q);
    
  SPL_RETURN_TYPE SPL_SetQ_vint16Q(      SPL_vint16Q_t*  const X_p,
                                const int16_t               Q);

  SPL_RETURN_TYPE SPL_SetQ_vint32Q(      SPL_vint32Q_t*  const X_p,
                                const int16_t               Q);

  SPL_RETURN_TYPE SPL_Sqrt_sint16Q(      SPL_sint16Q_t*  X_Root_p,
                                const SPL_sint16Q_t*  X_p);

  SPL_RETURN_TYPE SPL_Sqrt_vint16Q(      SPL_vint16Q_t*  X_Root_p,
                                const SPL_vint16Q_t*  X_p);

  SPL_RETURN_TYPE SPL_Sqrt_vint32Q(      SPL_vint16Q_t*  X_Root_p,
                                const SPL_vint32Q_t*  X_p);

  SPL_RETURN_TYPE SPL_SquaredSum_vint16(      SPL_sint16Q_t*  const SquaredSum_p,
                                     const int16_t*        const x_p,
                                     const int16_t               length);

  SPL_RETURN_TYPE SPL_SquaredSum_vint16Q(      SPL_sint16Q_t*  const SquaredSum_p,
                                      const SPL_vint16Q_t*  const X_p,
                                      const int16_t               start,
                                      const int16_t               length);

  SPL_RETURN_TYPE SPL_Sub_vint16_vint16Q(      int16_t*        const Diff_p,
                                      const int16_t*        const X_p, 
                                      const SPL_vint16Q_t*  const Y_p);
   
  SPL_RETURN_TYPE SPL_Sub_vint16Q(      SPL_vint16Q_t*  const Diff_p,
                               const SPL_vint16Q_t*  const X_p,
                               const SPL_vint16Q_t*  const Y_p);

  SPL_RETURN_TYPE SPL_Sub_vint16Q_sint16Q(      SPL_vint16Q_t*  const Sum_p,
                                       const SPL_vint16Q_t*  const X_p,
                                       const SPL_sint16Q_t*  const Term_p);

  SPL_RETURN_TYPE SPL_Sub_vint16Q_vint16(      SPL_vint16Q_t*  const Diff_p,
                                      const SPL_vint16Q_t*  const X_p,
                                      const int16_t*        const Y_p);

  SPL_RETURN_TYPE SPL_Sub_sint16Q(      SPL_sint16Q_t*  const sum_p,
                               const SPL_sint16Q_t*  const x_p,
                               const SPL_sint16Q_t*  const y_p);

  SPL_RETURN_TYPE SPL_Sub_sint32Q(      SPL_sint32Q_t*  const sum_p,
                               const SPL_sint32Q_t*  const x_p,
                               const SPL_sint32Q_t*  const y_p);

  SPL_RETURN_TYPE SPL_Sub_vint32Q(      SPL_vint32Q_t*  const Diff_p,
                               const SPL_vint32Q_t*  const X_p,
                               const SPL_vint32Q_t*  const Y_p);

  SPL_RETURN_TYPE SPL_Sum_vint16Q(      SPL_sint16Q_t*  const X_Summed_p,
                               const SPL_vint16Q_t*  const X_p,
                               const int16_t               start,
                               const int16_t               length);

  SPL_RETURN_TYPE SPL_Sum_vint32Q(      SPL_sint16Q_t*  const X_Summed_p,
                               const SPL_vint32Q_t*  const X_p,
                               const int16_t               start,
                               const int16_t               length);

  SPL_RETURN_TYPE SPL_SumElements_vint32Q(      SPL_vint32Q_t*  const X_Summed_p,
                                       const SPL_vint32Q_t*  const X_p,
                                       const int16_t               start,
                                       const int16_t               length,
                                       const int16_t*              bandwidth_p);

  SPL_RETURN_TYPE SPL_Vmax_vint16Q(      SPL_vint16Q_t*  const Max_p,
                                const SPL_vint16Q_t*  const X_p,
                                const SPL_vint16Q_t*  const Y_p);

  SPL_RETURN_TYPE SPL_Vmax_vint32Q(      SPL_vint32Q_t*  const Max_p,
                                const SPL_vint32Q_t*  const X_p,
                                const SPL_vint32Q_t*  const Y_p);

  SPL_RETURN_TYPE SPL_Vmin_vint16Q(      SPL_vint16Q_t*  const Min_p,
                                const SPL_vint16Q_t*  const X_p,
                                const SPL_vint16Q_t*  const Y_p);

  SPL_RETURN_TYPE SPL_Vmin_vint32Q(      SPL_vint32Q_t*  const Min_p,
                                const SPL_vint32Q_t*  const X_p,
                                const SPL_vint32Q_t*  const Y_p);
#endif /* SPL_FLOATING_POINT_DATA_TYPE */


/***********************************************************************
 *
 * Signal processing functions
 *
 ***********************************************************************/
SPL_RETURN_TYPE SPL_ArCepstrum(      SPL_vint16Q_t* const Cepstrum_p,
                            const SPL_vint16Q_t* const A_p);

SPL_RETURN_TYPE SPL_ArSpectrum(      SPL_vint32Q_t* const Spectrum_p,
                            const SPL_vint16Q_t* const A_p,
                            const SPL_sint16Q_t* const alpha_p,
                            const int16_t              fftLength);

SPL_RETURN_TYPE SPL_Autocorr_vint32Q_vint16(      SPL_vint32Q_t* const autoCorrelation_p,
                                         const int16_t*       const data_p,
                                         const int16_t              dataLength,
                                         const int16_t              maxLag,
                                         const SPL_Boolean_t        BiasedEstimate);

SPL_RETURN_TYPE SPL_BiQuad_vint16(      int16_t*       const Y_p,
                               const int16_t*       const X_p,
                               const int16_t              L,
                                     SPL_BiQuad_t*  const BiQuad_p); 
SPL_RETURN_TYPE SPL_BiQuadQ_vint16Q(  SPL_vint16Q_t*       const Y_p,
                                const SPL_vint16Q_t*       const X_p,
                                const int16_t                    L,
                                      SPL_BiQuadQ_t*       const BiQuadQ_p); 

SPL_RETURN_TYPE SPL_ComplexRandomVector(      SPL_vint16Q_t*  const X_p,
                                           int16_t*              seed_p,
                                           int16_t*              offset_p,
                                     const int16_t               fftLength);

SPL_RETURN_TYPE SPL_Fft_vint16Q_16x16(const SPL_vint16Q_t*       const inData_p,
                                         SPL_vint16Q_t*       const outData_p,
                                   const int16_t                    fftLength,
                                   const SPL_FFT_Data_t             dataType,
                                   const SPL_FFT_Normalize_t        normFlag);


SPL_RETURN_TYPE SPL_Fft_vint16Q_32x16(const SPL_vint16Q_t*       const inData_p,
                                         SPL_vint16Q_t*       const outData_p,
                                   const int16_t                    fftLength,
                                   const SPL_FFT_Data_t             dataType,
                                   const SPL_FFT_Normalize_t        normFlag);

SPL_RETURN_TYPE SPL_FftSpectrum(      SPL_vint32Q_t* const Spectrum_p,
                             const SPL_vint16Q_t*  const Signal_p,
                             const int16_t              fftLength);

SPL_RETURN_TYPE SPL_FirFilter_vint16(      int16_t*           const outputData_p,
                                  const int16_t*           const inputData_p,
                                  const int                      dataLength,
                                  const int16_t*           const Fir_p,
                                  const int                      filterLength,
                                  const SPL_Filter_Type_t        filterType);

SPL_RETURN_TYPE SPL_FirFilterQ_vint16(      int16_t*           const outputData_p,
                                   const int16_t*           const inputData_p,
                                   const int                      dataLength,
                                   const SPL_vint16Q_t*     const Fir_p,
                                   const SPL_Filter_Type_t        filterType);
                    
SPL_RETURN_TYPE SPL_FirFilterQ_vint16Q(      SPL_vint16Q_t*           const OutputData_p,
                                   const SPL_vint16Q_t*           const InputData_p,
                                   const int                      dataLength,
                                   const SPL_vint16Q_t*     const Fir_p,
                                   const SPL_Filter_Type_t        filterType);
SPL_RETURN_TYPE SPL_FirFrequencySampling_vint16Q(      SPL_vint16Q_t*     const TimeDomainFilter_p,
                                                 const SPL_vint16Q_t*     const FrequencyDomainFilter_p,
                                              const SPL_Filter_Type_t       outputType);

SPL_RETURN_TYPE SPL_InitCircularBuffer(      SPL_CircularBuffer_t*  const CircularBuffer_p,
                                          int16_t*               const data_p,
                                    const int                                  circularIndex,
                                    const int                                  bufferLength);


SPL_RETURN_TYPE SPL_InverseFft_vint16Q_16x16(const SPL_vint16Q_t*  const inData_p,
                                                SPL_vint16Q_t*  const outData_p,
                                          const int16_t               fftLength,
                                          const SPL_FFT_Data_t        dataType);


SPL_RETURN_TYPE SPL_Inverse_dB_sint16Q_int16(      SPL_sint16Q_t* const LinearValue_p,
                                          const int16_t              dB_Value);

SPL_RETURN_TYPE SPL_Inverse_dB_sint16Q(      SPL_sint16Q_t* const LinearValue_p,
                                    const SPL_sint16Q_t* const dB_Value_p);

SPL_RETURN_TYPE SPL_Inverse_dBovl_sint16Q_int16(      SPL_sint16Q_t* const LinearValue_p,
                                             const int16_t              dBovlValue); 

SPL_RETURN_TYPE SPL_Inverse_dBovl_sint16Q(      SPL_sint16Q_t* const LinearValue_p,
                                       const SPL_sint16Q_t* const dBovlValue_p); 

SPL_RETURN_TYPE SPL_Inverse_dB_sint32Q_sint16(      SPL_sfloatQ_t* const LinearValue_p,
                                              const int16_t               dB_Value);

SPL_RETURN_TYPE SPL_Inverse_dBovl_sint32Q_int16(      SPL_sfloatQ_t* const LinearValue_p,
                                                 const int16_t               dBovl_Value);


SPL_RETURN_TYPE SPL_InverseFft_vint16Q_32x16(const SPL_vint16Q_t*  const inData_p,
                                                SPL_vint16Q_t*  const outData_p,
                                          const int16_t               fftLength,
                                          const SPL_FFT_Data_t        dataType);
  
SPL_RETURN_TYPE SPL_Levinson(      SPL_vint16Q_t* const A_p,
                                SPL_sint16Q_t* const alpha_p,
                          const SPL_vint16Q_t* const A_old_p,
                          const SPL_sint16Q_t* const alpha_old_p,
                          const SPL_vint32Q_t* const r_p); 

SPL_RETURN_TYPE SPL_Overlap_vint16(      int16_t*               const X_extended_p,
                                      SPL_CircularBuffer_t*  const X_buffer_p,
                                const int16_t*               const X_p,
                                const int16_t                      length,   
                                const int16_t                      extendedLength);

SPL_RETURN_TYPE SPL_RemoveDc_vint16(      int16_t*        const Out_p,
                                 const int16_t*        const In_p,
                                 const int16_t               length,
                                 const SPL_sint16Q_t*  const oneOverLength);
SPL_RETURN_TYPE SPL_RemoveDc_vint16Q(      SPL_vint16Q_t*  const Out_p,
                                  const SPL_vint16Q_t*  const In_p,
                                  const SPL_sint16Q_t*  const OneOverLength);

SPL_RETURN_TYPE SPL_RandomVector(      int16_t*  const x_p,
                                    int32_t*        seed_p,
                              const int16_t         length);

SPL_RETURN_TYPE SPL_Periodogram(const SPL_vint16Q_t*  const Fft_p,
                                   SPL_vint32Q_t*  const Periodogram_p,
                             const int16_t               fftLength,
                             const int16_t               bandwidth);

SPL_RETURN_TYPE SPL_PreEmphasis_vint16(      int16_t* const Y_p,
                                          int16_t* const Memory_p,                
                                    const int16_t* const X_p,         
                                    const int16_t        alpha,         
                                    const int16_t        length); 

SPL_RETURN_TYPE SPL_ReadCircularBuffer(      int16_t*               const outData_p,
                                          SPL_CircularBuffer_t*  const CircularBuffer_p,
                                    const int16_t                      nDelay,
                                    const int16_t                      nExtract);

SPL_RETURN_TYPE SPL_SpectralFactorization(      SPL_vint16Q_t*  const H_minimumPhase_p,
  const SPL_vint16Q_t*  const H_linearPhase_p,
  const int16_t               filterLength,
  const int16_t               numberOfIterations);

SPL_RETURN_TYPE SPL_StoreCircularBuffer(const int16_t*               const newData_p,
                                           SPL_CircularBuffer_t*  const CircularBuffer_p,
                                    const int16_t                       nStore);

#ifdef SPL_FLOATING_POINT_DATA_TYPE
  SPL_RETURN_TYPE SPL_InitCircularBuffer_vfloatQ(
         SPL_CircularBuffer_vfloatQ_t*  const CircularBuffer_p,
         SPL_Float_t*                   const data_p,
   const int                                  circularIndex,
   const int                                  bufferLength);
  SPL_RETURN_TYPE SPL_StoreCircularBuffer_vfloatQ(
    const SPL_vfloatQ_t*                 const newData_p,
          SPL_CircularBuffer_vfloatQ_t*  const CircularBuffer_p,
    const int16_t                              nStore);
  SPL_RETURN_TYPE SPL_ReadCircularBuffer_vfloatQ(      
          SPL_vfloatQ_t*                 const outData_p,
          SPL_CircularBuffer_vfloatQ_t*  const CircularBuffer_p,
    const int16_t                              nDelay,
    const int16_t                              nExtract);
  #ifndef SPL_VERIFY_CALLS
    #define SPL_InitCircularBuffer_vint16Q SPL_InitCircularBuffer_vfloatQ
    #define SPL_InitCircularBuffer_vint32Q SPL_InitCircularBuffer_vfloatQ
    #define SPL_ReadCircularBuffer_vint16Q SPL_ReadCircularBuffer_vfloatQ
    #define SPL_ReadCircularBuffer_vint32Q SPL_ReadCircularBuffer_vfloatQ
    #define SPL_StoreCircularBuffer_vint16Q SPL_StoreCircularBuffer_vfloatQ
    #define SPL_StoreCircularBuffer_vint32Q SPL_StoreCircularBuffer_vfloatQ
  #endif
#else
  SPL_RETURN_TYPE SPL_InitCircularBuffer_vint16Q(
          SPL_CircularBuffer_vint16Q_t*  const CircularBuffer_p,
          SPL_int16QData_t*              const data_p,
    const int                                  circularIndex,
    const int                                  bufferLength);
  SPL_RETURN_TYPE SPL_InitCircularBuffer_vint32Q(
          SPL_CircularBuffer_vint32Q_t*  const CircularBuffer_p,
          SPL_int32QData_t*              const data_p,
    const int                                  circularIndex,
    const int                                  bufferLength);
  SPL_RETURN_TYPE SPL_StoreCircularBuffer_vint16Q(
    const SPL_vint16Q_t*                 const newData_p,
          SPL_CircularBuffer_vint16Q_t*  const CircularBuffer_p,
    const int16_t                              nStore);
  SPL_RETURN_TYPE SPL_StoreCircularBuffer_vint32Q(
    const SPL_vint32Q_t*                 const newData_p,
                                                   SPL_CircularBuffer_vint32Q_t*  const CircularBuffer_p,
                                             const int16_t                              nStore);

  SPL_RETURN_TYPE SPL_ReadCircularBuffer_vint16Q( 
          SPL_vint16Q_t*                 const outData_p,
          SPL_CircularBuffer_vint16Q_t*  const CircularBuffer_p,
    const int16_t                              nDelay,
    const int16_t                              nExtract);

  SPL_RETURN_TYPE SPL_ReadCircularBuffer_vint32Q(
          SPL_vint32Q_t*                 const outData_p,
          SPL_CircularBuffer_vint32Q_t*  const CircularBuffer_p,
    const int16_t                              nDelay,
    const int16_t                              nExtract);
#endif
/***********************************************************************
 *
 * Extended functions in SPL arm_neon
 *
 ***********************************************************************/
#ifdef SPL_SW_LIB_ARM_NEON

SPL_RETURN_TYPE SPL_Mult_vint32Q_vfloat(      SPL_vint32Q_t*  const Product_p,
                             const SPL_vint32Q_t*  const X_p,
                             const float*          const Y_p,
                             const int16_t               length);


SPL_RETURN_TYPE SPL_RunAttackRelease(      SPL_vint16Q_t* const out_p,
                          const SPL_sint32Q_t* const Target_p,
                                SPL_sint32Q_t* const StartValue_p,
                          const SPL_sint32Q_t* const Attack_p,
                          const SPL_sint32Q_t* const Release_p,
                                SPL_sint32Q_t* const Hold_p,
                          const SPL_sint32Q_t* const HtSamp_p,
                          const int16_t              Length);

SPL_RETURN_TYPE SPL_Smooth_Max(const SPL_vint32Q_t* const vec_p,
                          SPL_sint32Q_t* const Max_p,
                          SPL_sint32Q_t* const History_p,
                    const SPL_sint32Q_t* const Attack_p,
                    const SPL_sint32Q_t* const Release_p);
 
#endif  // ARM_NEON

#ifdef __cplusplus
}
#endif

#endif  // INCLUSION_GUARD

