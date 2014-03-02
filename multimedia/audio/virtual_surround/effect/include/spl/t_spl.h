/***********************************************************************
 *  © ST-Ericsson, 2010 - All rights reserved
 *
 *  Reproduction and Communication of this document is strictly prohibited
 *  unless specifically authorized in writing by ST-Ericsson.
 ***********************************************************************
 * DESCRIPTION
 *
 * $Id: t_spl.h,v 1.26 2009/10/02 11:22:11 erauae Exp $ 
 *
 * Filename:   
 * Author:     
 * Created:    
 *
 ***********************************************************************
 * REVISION HISTORY
 *
 * $Log: t_spl.h,v $
 *
 *
 ***********************************************************************/

#ifndef INCLUSION_GUARD_T_SPL_H
#define INCLUSION_GUARD_T_SPL_H


/***********************************************************************
 *
 * CONSTANTS AND GERNERAL TYPES
 *
 ***********************************************************************/

#define SPL_MAX_SINT32 (sint32) 0x7fffffffL
#define SPL_MIN_SINT32 (sint32) 0x80000000L

#define SPL_MAX_SINT16 (sint16) 0x7fff
#define SPL_MIN_SINT16 (sint16) 0x8000

#define SPL_MAX_Q 0 /* should be set to SPL_MAX_SINT16 when all SPL functions
                       modify Q values using saturating arithmetic             */

/***********************************************************************
 *
 * ENUM TYPES
 *
 ***********************************************************************/

typedef enum {
  SPL_FFT_IS_COMPLEX = 0,    /* Input is complex valued */
  SPL_FFT_IS_REAL,           /* Input is real valued    */
  SPL_FFT_IS_REAL_MUXED      /* Input is real valued, but multiplexed */ 
                             /* to complex valued       */
} SPL_FFT_Data_t;

typedef enum {
  SPL_FFT_NO_NORMALIZATION = 0,    /* No input data normalization */
  SPL_FFT_NORMALIZE,               /* Normalize input data        */
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
 * ERROR CODE HANDLING FOR SPL
 *
 ***********************************************************************/

/* Error code */ 
typedef enum {
  SPL_NO_ERROR = 0,
  SPL_UNINITIALIZED_PARAMETER,
  SPL_UNINITIALIZED_DATA,
  SPL_CONFLICTING_VECTOR_LENGTHS,
  SPL_VECTOR_INDEX_OUT_OF_RANGE,
  SPL_DIVISION_BY_ZERO,
  SPL_UNKNOWN_ERROR
} SPL_Return_t;


/* Max vector length for sanity check */
#define SPL_MAX_VECTOR_LENGTH 	2048
#define SPL_MAX_VECTOR_LENGTH_BY_TWO  1024
#define SPL_MAX_FLOAT 			1e30;
#define SPL_MAX_FFT_LENGTH 		1024



/***********************************************************************
 *
 * TYPE DECLARATIONS
 *
 ***********************************************************************/
/* General fixed point variables */
#define sint16 signed short
#define sint32 signed int
#define sint64 signed long long

#define uint16 unsigned short
#define uint32 unsigned int
#define uint64 unsigned long long


#define SPL_NO_OF_BIQUAD_NUMERATOR_COEF (3)
#define SPL_BIQUAD_NUMERATOR_START_INDEX (0)

#define SPL_NO_OF_BIQUAD_DENOMINATOR_COEF (2)
#define SPL_BIQUAD_DENOMINATOR_START_INDEX (SPL_NO_OF_BIQUAD_NUMERATOR_COEF)

#define SPL_NO_OF_BIQUAD_FILTER_COEF (SPL_NO_OF_BIQUAD_NUMERATOR_COEF+SPL_NO_OF_BIQUAD_DENOMINATOR_COEF)

#define SPL_NO_OF_BIQUAD_STATES 2


#define SPL_Float_t double

#ifdef SPL_USE_FLOAT_DATATYPE

/* Biquad type defs */


/* Single precision variables (48 dB dynamic range) */ 
#define SPL_vint16QData_t SPL_Float_t   
/* Double precision variables (96 dB dynamic range) */
#define SPL_vint32QData_t SPL_Float_t   

#else

/* Single precision varaibles (48 dB dynamic range) */ 
#define SPL_vint16QData_t sint16   
/* Double precision variables (96 dB dynamic range) */
#define SPL_vint32QData_t sint32   

#endif


/* Floating Q-value scalar types */
typedef struct {
  short              Q; 
  SPL_vint16QData_t  v;
} SPL_sint16Q_t;

typedef struct {
  short               Q; 
  SPL_vint32QData_t   v;
} SPL_sint32Q_t;

/* Floating Q-value vector types */
typedef struct {
  short              Q; 
  short              L;    
  SPL_vint16QData_t* v_p;
} SPL_vint16Q_t;

typedef struct {
  short             Q; 
  short             L;    
  SPL_vint32QData_t *v_p;
} SPL_vint32Q_t;


/* Float vector type */
/* 
 * Not supported yet
 *
 * typedef struct {
 *  short        Q; 
 *  SPL_Float_t  *v_p;
 * } SPL_vfloat_t;
 */


/***********************************************************************
 * 
 * CPP MACROS FOR SPL VINT DECLARATIONS
 *
 ***********************************************************************/

#define SPL_DECLARE_VINT16Q(VARIABLE) SPL_vint16Q_t (VARIABLE) = {0, 0, NULL}
#define SPL_DECLARE_VINT32Q(VARIABLE) SPL_vint32Q_t (VARIABLE) = {0, 0, NULL}

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
  sint16*  data_p; 
  int      circularIndex;
  int      bufferLength;
} SPL_CircularBuffer_t;

typedef struct {
 sint16	FilterCoefficients[SPL_NO_OF_BIQUAD_FILTER_COEF];
 sint32 State[SPL_NO_OF_BIQUAD_STATES];
} SPL_BiQuad_t;
  
#endif
