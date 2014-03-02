/***********************************************************************
 *  © ST-Ericsson, 2010 - All rights reserved
 *
 *  Reproduction and Communication of this document is strictly prohibited
 *  unless specifically authorized in writing by ST-Ericsson.
 ***********************************************************************
 * DESCRIPTION
 *
 * $Id: r_spl.h,v 1.55 2009/11/19 14:47:02 erauae Exp $ 
 * Filename:   
 * Author:     
 * Created:    
 *
 ***********************************************************************
 * REVISION HISTORY
 *
 * $Log: r_spl.h,v $
 *
 ***********************************************************************/

#ifndef INCLUSION_GUARD_R_SPL_H
#define INCLUSION_GUARD_R_SPL_H

#include <stdio.h>

#include "t_spl.h"
#include "spl_flt_to_fix.h"
#include "spl_fixed_point_functions.h"
#include "spl_fix_function_usage.h"
//#ifdef __arm__
//#include "spl_fix_internal.h"
//#endif

/***********************************************************************
 *
 * INCLUDE FILES
 *
 ***********************************************************************/


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


/***********************************************************************
 *
 * CPP MACROS FOR CLEANER REFERENCE CODE WITH ERROR HANDLING
 *
 ***********************************************************************/
 /* Verification on return code */
#define SPL_VERIFY_CALL(X) \
do {\
 SPL_Return_t returnValue; \
 if((returnValue = (X)) != SPL_NO_ERROR) \
 printf("%s line %d: Error %d in SPL function call %s\n", __FILE__, __LINE__, returnValue, #X); \
} while (0) 

/* Check for ininitalization of parameters */
#define SPL_VERIFY_DATA_INIT(X, Y)\
do {\
  if ((X) == SPL_NO_ERROR) {\
    if ((Y) == NULL) {\
      (X) = SPL_UNINITIALIZED_PARAMETER;\
    }\
    else if ((Y)->v_p == NULL) {\
      (X) = SPL_UNINITIALIZED_DATA;\
    }\
  }\
} while (0)

/* Check vector lengths */
#define SPL_VERIFY_DATA_LENGTH(X, Y, Z)\
do {\
  if ((X) == SPL_NO_ERROR) {\
    if ((Y)->L != (Z)->L) {\
      (X) = SPL_CONFLICTING_VECTOR_LENGTHS;\
    }\
  }\
} while (0)

/* Check vector lengths */
#define SPL_VERIFY_LENGTH(X, Y, Z)\
do {\
  if ((X) == SPL_NO_ERROR) {\
    if ((Y)->L < (Z)) {\
      (X) = SPL_CONFLICTING_VECTOR_LENGTHS;\
    }\
  }\
} while (0)


/***********************************************************************
 *
 * Arithmetic functions
 *
 ***********************************************************************/

#define SPL_AbsSquare_Cx_vint16Q(A, B) \
  SPL_VERIFY_CALL(SPL_AbsSquare_Cx_vint16Q_F((A), (B)))

#define SPL_AbsSquare_Cx_vint32Q(A, B) \
  SPL_VERIFY_CALL(SPL_AbsSquare_Cx_vint32Q_F((A), (B)))

#define SPL_Add_vint16(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_Add_vint16_F((A), (B), (C), (D)))

#define SPL_Add_sint16Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Add_sint16Q_F((A), (B), (C)))

#define SPL_Add_vint16Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Add_vint16Q_F((A), (B), (C)))

#define SPL_Add_vint32Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Add_vint32Q_F((A), (B), (C)))

#define SPL_Add_vint16Q_sint16Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Add_vint16Q_sint16Q_F((A), (B), (C)))

#define SPL_Add_vint32Q_sint16Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Add_vint32Q_sint16Q_F((A), (B), (C)))

#define SPL_Add_vint32Q_sint32Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Add_vint32Q_sint32Q_F((A), (B), (C)))

#define SPL_BlockSum_vint16(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_BlockSum_vint16_F((A), (B), (C), (D)))

#define SPL_BlockSum_vint16Q(A, B, C, D, E) \
  SPL_VERIFY_CALL(SPL_BlockSum_vint16Q_F((A), (B), (C), (D), (E)))

#define SPL_BlockSum_vint32Q(A, B, C, D, E) \
  SPL_VERIFY_CALL(SPL_BlockSum_vint32Q_F((A), (B), (C), (D), (E)))

#define SPL_Convert_sint32Q_sint16Q(A, B) \
  SPL_VERIFY_CALL(SPL_Convert_sint32Q_sint16Q_F((A), (B)))

#define SPL_Convert_sint32Q_vint16Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Convert_sint32Q_vint16Q_F((A), (B), (C)))

#define SPL_Convert_vint32Q_vint16Q(A, B) \
  SPL_VERIFY_CALL(SPL_Convert_vint32Q_vint16Q_F((A), (B)))

#define SPL_Div_sint16Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Div_sint16Q_F((A), (B), (C)))

#define SPL_Div16Q_sint32Q_sint32Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Div16Q_sint32Q_sint32Q_F((A), (B), (C)))

#define SPL_Div16Q_vint32Q_vint32Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Div16Q_vint32Q_vint32Q_F((A), (B), (C)))

#define SPL_Div_vint16Q_vint32Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Div_vint16Q_vint32Q_F((A), (B), (C)))

#define SPL_Exp_sint16Q_sint16(A, B, C) \
  SPL_VERIFY_CALL(SPL_Exp_sint16Q_sint16_F((A), (B), (C)))

#define SPL_Exp_sint16Q_sint16Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Exp_sint16Q_sint16Q_F((A), (B), (C)))
                    
#define SPL_Limit_vint16Q(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_Limit_vint16Q_F((A), (B), (C), (D)))

#define SPL_Limit_vint32Q(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_Limit_vint32Q_F((A), (B), (C), (D)))

#define SPL_Log_sint16(A, B, C) \
  SPL_VERIFY_CALL(SPL_Log_sint16_F((A), (B), (C)))

#define SPL_Log_sint16Q_sint16(A, B, C) \
  SPL_VERIFY_CALL(SPL_Log_sint16Q_sint16_F((A), (B), (C)))

#define SPL_Log_sint16Q_sint16Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Log_sint16Q_sint16Q_F((A), (B), (C)))

#define SPL_Mac_Cx_vint16Q(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_Mac_Cx_vint16Q_F((A), (B), (C), (D)))

#define SPL_Mac_Cx_vint16Q_Conj(A, B, C) \
  SPL_VERIFY_CALL(SPL_Mac_Cx_vint16Q_Conj_F((A), (B), (C)))

#define SPL_Mac_Cx_vint16Q_Not_Conj(A, B, C) \
  SPL_VERIFY_CALL(SPL_Mac_Cx_vint16Q_Not_Conj_F((A), (B), (C)))

#define SPL_Mult_Cx_vint16Q(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_Mult_Cx_vint16Q_F((A), (B), (C), (D)))

#define SPL_Mult_Cx_vint16Q_R_vint16Q(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_Mult_Cx_vint16Q_R_vint16Q_F((A), (B), (C), (D)))
 
#define SPL_Mult_sint16Q_sint16Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Mult_sint16Q_sint16Q_F((A), (B), (C)))

#define SPL_Mult_sint32Q_sint16Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Mult_sint32Q_sint16Q_F((A), (B), (C)))

#define SPL_Mult_vint16(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_Mult_vint16_F((A), (B), (C), (D)))

#define SPL_Mult_vint16_sint16(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_Mult_vint16_sint16_F((A), (B), (C), (D)))

#define SPL_Mult_vint16_sint16Q(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_Mult_vint16_sint16Q_F((A), B, (C), (D)))

#define SPL_Mult_vint16Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Mult_vint16Q_F((A), (B), (C)))
                             
#define SPL_Mult_vint16Q_sint16Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Mult_vint16Q_sint16Q_F((A), (B), (C)))
 
#define SPL_Mult_vint16Q_vint16(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_Mult_vint16Q_vint16_F((A), (B), (C), (D)))
 
#define SPL_Mult_vint16Q_vint32Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Mult_vint16Q_vint32Q_F((A), (B), (C)))

#define SPL_Mult_vint32Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Mult_vint32Q_F((A), (B), (C)))
                                            
#define SPL_Mult_vint32Q_sint16Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Mult_vint32Q_sint16Q_F((A), (B), (C)))

#define SPL_Mult_vint32Q_sint32Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Mult_vint32Q_sint32Q_F((A), (B), (C)))

#define SPL_Mult_vint32Q_vint16(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_Mult_vint32Q_vint16_F((A), (B), (C), (D)))

#define SPL_Mult_vint32Q_vint16Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Mult_vint32Q_vint16Q_F((A), (B), (C)))

#define SPL_Norm_vint16(A, B, C) \
  SPL_VERIFY_CALL(SPL_Norm_vint16_F((A), (B), (C)))

#define SPL_Norm_vint32(A, B, C) \
  SPL_VERIFY_CALL(SPL_Norm_vint32_F((A), (B), (C)))

#define SPL_SetQ_sint16Q(A, B) \
  SPL_VERIFY_CALL(SPL_SetQ_sint16Q_F((A), (B)))

#define SPL_ShiftLeft_vint16(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_ShiftLeft_vint16_F((A), (B), (C), (D)))

#define SPL_ShiftLeft_vint32(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_ShiftLeft_vint32_F((A), (B), (C), (D)))

#define SPL_ShiftRightRound_vint16(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_ShiftRight_vint16_F((A), (B), (C), (D)))

#define SPL_ShiftRightRound_vint32(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_ShiftRightRound_vint32_F((A), (B), (C), (D)))

#define SPL_ShiftRight_vint16(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_ShiftRight_vint16_F((A), (B), (C), (D)))

#define SPL_ShiftRight_vint32(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_ShiftRight_vint32_F((A), (B), (C), (D)))

#define SPL_Sqrt_sint16Q(A, B) \
  SPL_VERIFY_CALL(SPL_Sqrt_sint16Q_F((A), (B)))

#define SPL_Sqrt_vint16Q(A, B) \
  SPL_VERIFY_CALL(SPL_Sqrt_vint16Q_F((A), (B)))

#define SPL_Sqrt_vint32Q(A, B) \
  SPL_VERIFY_CALL(SPL_Sqrt_vint32Q_F((A), (B)))

#define SPL_SquaredSum_vint16(A, B, C) \
  SPL_VERIFY_CALL(SPL_SquaredSum_vint16_F((A), (B), (C)))

#define SPL_SquaredSum_vint16Q(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_SquaredSum_vint16Q_F((A), (B), (C), (D)))

#define SPL_Sub_vint16(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_Sub_vint16_F((A), (B), (C), (D)))

#define SPL_Sub_vint16_sint16(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_Sub_vint16_sint16_F((A), (B), (C), (D)))

#define SPL_Sub_vint16_vint16Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Sub_vint16_vint16Q_F((A), (B), (C)))

#define SPL_Sub_vint16Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Sub_vint16Q_F((A), (B), (C)))

#define SPL_Sub_vint16Q_sint16Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Sub_vint16Q_sint16Q_F((A), (B), (C)))

#define SPL_Sub_vint16Q_vint16(A, B, C) \
  SPL_VERIFY_CALL(SPL_Sub_vint16Q_vint16_F((A), (B), (C)))

#define SPL_Sub_vint32Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Sub_vint32Q_F((A), (B), (C)))

#define SPL_Sum_vint16(A, B, C) \
  SPL_VERIFY_CALL(SPL_Sum_vint16_F((A), (B), (C)))

#define SPL_Sum_vint16Q(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_Sum_vint16Q_F((A), (B), (C), (D)))

#define SPL_Sum_vint32Q(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_Sum_vint32Q_F((A), (B), (C), (D)))


/***********************************************************************
 *
 * Logic functions
 *
 ***********************************************************************/
#define SPL_And_vint16(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_And_vint16_F((A), (B), (C), (D)))

#define SPL_BlockMax_vint32Q(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_BlockMax_vint32Q_F((A), (B), (C), (D)))

#define SPL_Compare_sint16Q(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_Compare_sint16Q_F((A), (B), (C), (D)))

#define SPL_Compare_sint32Q(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_Compare_sint32Q_F((A), (B), (C), (D)))

#define SPL_Compare_vint16Q(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_Compare_vint16Q_F((A), (B), (C), (D)))
 
#define SPL_Compare_vint32Q(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_Compare_vint32Q_F((A), (B), (C), (D)))

#define SPL_Find_vint16Q(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_Find_vint16Q_F((A), (B), (C), (D)))

#define SPL_Find_vint16(A, B, C, D, E) \
  SPL_VERIFY_CALL(SPL_Find_vint16_F((A), (B), (C), (D), (E)))

#define SPL_Find_vint32(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_Find_vint32_F((A), (B), (C), (D)))

#define SPL_Find_vint32Q(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_Find_vint32Q_F((A), (B), (C), (D)))

#define SPL_Mask_vint16(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_Mask_vint16_F((A), (B), (C), (D)))
  
#define SPL_Mask_vint16Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Mask_vint16Q_F((A), (B), (C)))

#define SPL_Mask_vint32(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_Mask_vint32_F((A), (B), (C), (D)))

#define SPL_Mask_vint32Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Mask_vint32Q_F((A), (B), (C)))
                             
#define SPL_Max_vint16(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_Max_vint16_F((A), (B), (C), (D)))

#define SPL_Max_sint16Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Max_sint16Q_F((A), (B), (C)))

#define SPL_Max_vint16Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Max_vint16Q_F((A), (B), (C)))

#define SPL_Max_vint32Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Max_vint32Q_F((A), (B), (C)))

#define SPL_Min_vint32Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Min_vint32Q_F((A), (B), (C)))

#define SPL_Min_vint16Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Min_vint16Q_F((A), (B), (C)))

#define SPL_Not_vint16(A, B, C) \
  SPL_VERIFY_CALL(SPL_Not_vint16_F((A), (B), (C)))

#define SPL_Or_vint16(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_Or_vint16_F((A), (B), (C), (D)))

#define SPL_Vmax_vint16(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_Vmax_vint16_F((A), (B), (C), (D)))

#define SPL_Vmax_vint16Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Vmax_vint16Q_F((A), (B), (C)))

#define SPL_Vmax_vint32Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Vmax_vint32Q_F((A), (B), (C)))

#define SPL_Vmin_vint16(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_Vmin_vint16_F((A), (B), (C), (D)))

#define SPL_Vmin_vint16Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Vmin_vint16Q_F((A), (B), (C)))

#define SPL_Vmin_vint32Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Vmin_vint32Q_F((A), (B), (C)))


/***********************************************************************
 *
 * Signal processing functions
 *
 ***********************************************************************/

#define SPL_Average_sint16Q(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_Average_sint16Q_F((A), (B), (C), (D)))

#define SPL_Average_vint16Q(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_Average_vint16Q_F((A), (B), (C), (D)))

#define SPL_Average_vint32Q(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_Average_vint32Q_F((A), (B), (C), (D)))

#define SPL_BiQuad_vint16(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_BiQuad_vint16_F((A), (B), (C), (D)))


#define SPL_Fft_vint16Q_16x16(A, B, C, D, E) \
  SPL_VERIFY_CALL( SPL_Fft_vint16Q_F( (A), (B), (C), (D), (E)) )
//  SPL_VERIFY_CALL(SPL_Fft_vint16Q_16x16_F((A), (B), (C), (D), (E)))

#define SPL_Fft_vint16Q_32x16(A, B, C, D, E) \
  SPL_VERIFY_CALL(SPL_Fft_vint16Q_FLT((A), (B), (C), (D), (E)))
//  SPL_VERIFY_CALL(SPL_Fft_vint16Q_32x16_F((A), (B), (C), (D), (E)))

#define SPL_FirFilterQ_vint16(A, B, C, D, E) \
  SPL_VERIFY_CALL(SPL_FirFilterQ_vint16_F((A), (B), (C), (D), (E)))

#define SPL_FirFrequencySampling_vint16Q(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_FirFrequencySampling_vint16Q_F((A), (B), (C), (D)))


#define SPL_InverseFft_vint16Q_16x16(A, B, C, D) \
  SPL_VERIFY_CALL( SPL_InverseFft_vint16Q_F((A), (B), (C), (D)) )
//  SPL_VERIFY_CALL(SPL_InverseFft_vint16Q_16x16_F((A), (B), (C), (D)))

#define SPL_InverseFft_vint16Q_32x16(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_InverseFft_vint16Q_FLT((A), (B), (C), (D)))
//  SPL_VERIFY_CALL(SPL_InverseFft_vint16Q_32x16_F((A), (B), (C), (D)))

    
#define SPL_Periodogram(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_Periodogram_F((A), (B), (C), (D)))


/***********************************************************************
 *
 * Data storage functions
 *
 ***********************************************************************/
 
#define SPL_InitCircularBuffer(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_InitCircularBuffer_F((A), (B), (C), (D)))

#define SPL_ReadCircularBuffer(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_ReadCircularBuffer_F((A), (B), (C), (D)))

#define SPL_StoreCircularBuffer(A, B, C) \
  SPL_VERIFY_CALL(SPL_StoreCircularBuffer_F((A), (B), (C)))


/***********************************************************************
 *
 * Memory functions
 *
 ***********************************************************************/
#define SPL_Copy_vint16Q(A, B) \
  SPL_VERIFY_CALL(SPL_Copy_vint16Q_F((A), (B)))

#define SPL_Copy_vint16Q_vint16(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_Copy_vint16Q_vint16_F((A), (B), (C), (D)))

#define SPL_Copy_vint32Q(A, B) \
  SPL_VERIFY_CALL(SPL_Copy_vint32Q_F((A), (B)))

#define SPL_Copy_vint32Q_vint16(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_Copy_vint32Q_vint16_F((A), (B), (C), (D)))

#define SPL_Get_vint16(A, B, C) \
  SPL_VERIFY_CALL(SPL_Get_vint16_F((A), (B), (C)))

#define SPL_Get_sint16(A, B, C) \
  SPL_VERIFY_CALL(SPL_Get_sint16_F((A), (B), (C)))

#define SPL_Resample_vint16Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_Resample_vint16Q_F((A), (B), (C)))

#define SPL_Resample_vint16(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_Resample_vint16_F((A), (B), (C), (D)))

#define SPL_Set_vint16Q(A, B) \
  SPL_VERIFY_CALL(SPL_Set_vint16Q_F((A), (B)))

#define SPL_Set_vint16(A, B, C) \
  SPL_VERIFY_CALL(SPL_Set_vint16_F((A), (B), (C)))

#define SPL_Set_vint32Q(A, B) \
  SPL_VERIFY_CALL(SPL_Set_vint32Q_F((A), (B)))



/***********************************************************************
 *
 * Composite functions based on lower level functions
 *
 ***********************************************************************/

#define SPL_ConditionalAverage_vint16Q(A, B, C, D) \
  SPL_VERIFY_CALL(SPL_ConditionalAverage_vint16Q_F((A), (B), (C), (D)))

#define SPL_ConditionalCopy_vint16Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_ConditionalCopy_vint16Q_F((A), (B), (C)))

#define SPL_ConditionalCopy_vint32Q(A, B, C) \
  SPL_VERIFY_CALL(SPL_ConditionalCopy_vint32Q_F((A), (B), (C)))




/***********************************************************************
 *
 * FUNCTION HEADERS
 *
 ***********************************************************************/

/***********************************************************************
 *
 * Arithmetic functions
 *
 ***********************************************************************/

SPL_Return_t SPL_AbsSquare_Cx_vint16Q_F(      SPL_vint32Q_t*  const X_Squared_p,
                                       const SPL_vint16Q_t*  const X_p);

SPL_Return_t SPL_AbsSquare_Cx_vint32Q_F(      SPL_vint32Q_t*  const X_Squared_p,
                                       const SPL_vint32Q_t*  const X_p);

SPL_Return_t SPL_Add_vint16_F(      sint16*         const Sum_p,
                             const sint16*         const X_p,
                             const sint16*         const Y_p,
                             const sint16                L);

SPL_Return_t SPL_Add_sint16Q_F(      SPL_sint16Q_t*  const sum_p,
                               const SPL_sint16Q_t*  const x_p,
                               const SPL_sint16Q_t*  const y_p);

SPL_Return_t SPL_Add_vint16Q_F(      SPL_vint16Q_t*  const Sum_p,
                              const SPL_vint16Q_t*  const X_p,
                              const SPL_vint16Q_t*  const Y_p);

SPL_Return_t SPL_Add_vint32Q_F(      SPL_vint32Q_t*  const Sum_p,
                              const SPL_vint32Q_t*  const X_p,
                              const SPL_vint32Q_t*  const Y_p);

SPL_Return_t SPL_Add_vint16Q_sint16Q_F(     SPL_vint16Q_t*  const Sum_p,
                                      const SPL_vint16Q_t*  const X_p,
                                      const SPL_sint16Q_t*  const Term_p);

SPL_Return_t SPL_Add_vint32Q_sint16Q_F(     SPL_vint32Q_t*  const Sum_p,
                                      const SPL_vint32Q_t*  const X_p,
                                      const SPL_sint16Q_t*  const Term_p);

SPL_Return_t SPL_Add_vint32Q_sint32Q_F(     SPL_vint32Q_t*  const Sum_p,
                                      const SPL_vint32Q_t*  const X_p,
                                      const SPL_sint32Q_t*  const Term_p);

SPL_Return_t SPL_BlockSum_vint16_F(      sint16*  const x_Summed_p,
                                   const sint16*  const x_p,
                                   const sint16         length,
                                   const sint16         bandwidth);

SPL_Return_t SPL_BlockSum_vint16Q_F(      SPL_vint16Q_t*  const X_Summed_p,
                                    const SPL_vint16Q_t*  const X_p,
                                    const sint16                start,
                                    const sint16                length,
                                    const sint16                bandwidth);

SPL_Return_t SPL_BlockSum_vint32Q_F(      SPL_vint32Q_t*  const X_Summed_p,
                                    const SPL_vint32Q_t*  const X_p,
                                    const sint16                start,
                                    const sint16                length,
                                    const sint16                bandwidth);

SPL_Return_t SPL_Convert_sint32Q_sint16Q_F(      SPL_sint16Q_t* const Y_p,
                                           const SPL_sint32Q_t* const X_p);

SPL_Return_t SPL_Convert_sint32Q_vint16Q_F(      SPL_vint16Q_t* const Y_p,
                                           const SPL_sint32Q_t* const X_p,
                                           const sint16               length);

SPL_Return_t SPL_Convert_vint32Q_vint16Q_F(      SPL_vint16Q_t* const Y_p,
                                           const SPL_vint32Q_t* const X_p);

SPL_Return_t SPL_Div_sint16Q_F(      SPL_sint16Q_t*        Quotient_p,
                               const SPL_sint16Q_t*  const Num_p,
                               const SPL_sint16Q_t*  const Denom_p);

SPL_Return_t SPL_Div16Q_sint32Q_sint32Q_F(      SPL_sint16Q_t*  const Quotient_p,
                                          const SPL_sint32Q_t*  const Num_p, 
                                          const SPL_sint32Q_t*  const Denom_p); 

SPL_Return_t SPL_Div16Q_vint32Q_vint32Q_F(      SPL_vint16Q_t*  Quotient_p,
                                         const SPL_vint32Q_t*  Num_p, 
                                         const SPL_vint32Q_t*  Denom_p); 

SPL_Return_t SPL_Div_vint16Q_vint32Q_F(      SPL_vint16Q_t*  Quotient_p,
                                      const SPL_vint16Q_t*  Num_p, 
                                      const SPL_vint32Q_t*  Denom_p);

SPL_Return_t SPL_Exp_sint16Q_sint16_F(      SPL_sint16Q_t* const Out_p,
                                      const sint16               in,
                                      const SPL_Exp_Type_t       expType);

SPL_Return_t SPL_Exp_sint16Q_sint16Q_F(      SPL_sint16Q_t* const Out_p,
                                       const SPL_sint16Q_t* const in_p,
                                       const SPL_Exp_Type_t       expType);
                    
SPL_Return_t SPL_Limit_vint16Q_F(      SPL_vint16Q_t*  const X_limit_p,
                                 const SPL_vint16Q_t*  const X_p,
                                 const SPL_sint16Q_t*  const Limit_p,
                                 const SPL_Limit_t           comparison);

SPL_Return_t SPL_Limit_vint32Q_F(      SPL_vint32Q_t*  const X_limit_p,
                                 const SPL_vint32Q_t*  const X_p,
                                 const SPL_sint32Q_t*  const Limit_p,
                                 const SPL_Limit_t           comparison);

SPL_Return_t SPL_Log_sint16_F(      sint16*        const out_p,
                              const sint16               in,
                              const SPL_Log_Type_t       logType);

SPL_Return_t SPL_Log_sint16Q_sint16Q_F(      SPL_sint16Q_t* const Out_p,
                                       const SPL_sint16Q_t* const In_p,
                                       const SPL_Log_Type_t       logType);

SPL_Return_t SPL_Log_sint16Q_sint16_F(      SPL_sint16Q_t* const Out_p,
                                      const sint16               in,
                                      const SPL_Log_Type_t       logType);
                           
SPL_Return_t SPL_Mac_Cx_vint16Q_F(      SPL_vint16Q_t*        const Acc_p,
                                  const SPL_vint16Q_t*        const X_p,
                                  const SPL_vint16Q_t*        const Y_p,
                                  const SPL_Conjugate_Data_t        conjugate_Y);

SPL_Return_t SPL_Mac_Cx_vint16Q_Conj_F(      SPL_vint16Q_t* const Acc_p,
                                       const SPL_vint16Q_t* const X_p,
                                       const SPL_vint16Q_t* const Y_p   );


SPL_Return_t SPL_Mac_Cx_vint16Q_Not_Conj_F(      SPL_vint16Q_t* const Acc_p,
                                           const SPL_vint16Q_t* const X_p,
                                           const SPL_vint16Q_t* const Y_p   );


SPL_Return_t SPL_Mult_Cx_vint16Q_F(      SPL_vint32Q_t*        const Product_p,
                                   const SPL_vint16Q_t*        const X_p,
                                   const SPL_vint16Q_t*        const Y_p,
                                   const SPL_Conjugate_Data_t        conjugate_Y);

SPL_Return_t SPL_Mult_Cx_vint16Q_R_vint16Q_F(      SPL_vint16Q_t*  const Product_p,
                                             const SPL_vint16Q_t*  const X_p,
                                             const SPL_vint16Q_t*  const Y_p,
                                             const sint16                bandwidth_Y);

SPL_Return_t SPL_Mult_sint16Q_sint16Q_F(      SPL_sint16Q_t*  const product_p,
                                        const SPL_sint16Q_t*  const x_p,
                                        const SPL_sint16Q_t*  const y_p);

SPL_Return_t SPL_Mult_sint32Q_sint16Q_F(      SPL_sint32Q_t*  const product_p,
                                        const SPL_sint32Q_t*  const x_p,
                                        const SPL_sint16Q_t*  const y_p);

SPL_Return_t SPL_Mult_vint16_F(      sint16*  const Product_p,
                               const sint16*  const X_p,
                               const sint16*  const Y_p,
                               const sint16         L);

SPL_Return_t SPL_Mult_vint16_sint16_F(      sint16*        const Product_p,
                                      const sint16*        const X_p,
                                      const sint16               factor,
                                      const sint16               length);

SPL_Return_t SPL_Mult_vint16_sint16Q_F(      sint16*        const Product_p,
                                       const sint16*        const X_p,
                                       const sint16               length,
                                       const SPL_sint16Q_t* const Factor_p);

SPL_Return_t SPL_Mult_vint32Q_F(      SPL_vint32Q_t*  const Product_p,
                                const SPL_vint32Q_t*  const X_p,
                                const SPL_vint32Q_t*  const Y_p);

SPL_Return_t SPL_Mult_vint16Q_F(      SPL_vint16Q_t*  const Product_p,
                                const SPL_vint16Q_t*  const X_p,
                                const SPL_vint16Q_t*  const Y_p);
                             
SPL_Return_t SPL_Mult_vint16Q_sint16Q_F(      SPL_vint16Q_t*  const Product_p,
                                        const SPL_vint16Q_t*  const X_p,
                                        const SPL_sint16Q_t*  const Factor_p);
 
SPL_Return_t SPL_Mult_vint16Q_vint16_F(      SPL_vint16Q_t*  const Product_p,
                                       const SPL_vint16Q_t*  const X_p,
                                       const sint16*         const Y_p,
                                       const sint16                length);
 
SPL_Return_t SPL_Mult_vint16Q_vint32Q_F(      SPL_vint16Q_t*  const Product_p,
                                        const SPL_vint16Q_t*  const X_p,
                                        const SPL_vint32Q_t*  const Y_p);
                                            
SPL_Return_t SPL_Mult_vint32Q_sint16Q_F(      SPL_vint32Q_t*  const Product_p,
                                        const SPL_vint32Q_t*  const X_p,
                                        const SPL_sint16Q_t*  const Factor_p);

SPL_Return_t SPL_Mult_vint32Q_sint32Q_F(      SPL_vint32Q_t*  const Product_p,
                                        const SPL_vint32Q_t*  const X_p,
                                        const SPL_sint32Q_t*  const Factor_p);

SPL_Return_t SPL_Mult_vint32Q_vint16_F(      SPL_vint32Q_t*  const Product_p,
                                       const SPL_vint32Q_t*  const X_p,
                                       const sint16*         const Y_p,
                                       const sint16                length);

SPL_Return_t SPL_Mult_vint32Q_vint16Q_F(      SPL_vint32Q_t*  const Product_p,
                                        const SPL_vint32Q_t*  const X_p,
                                        const SPL_vint16Q_t*  const Y_p);

SPL_Return_t SPL_Norm_vint16_F(      sint16*         const norm_p,
                               const sint16*         const X_p,
                               const sint16                L);

SPL_Return_t SPL_Norm_vint32_F(      sint16*         const norm_p,
                               const sint32*         const X_p,
                               const sint16                L);

SPL_Return_t SPL_SetQ_sint16Q_F(      SPL_sint16Q_t*  const x_p,
                                const sint16                       Q);

SPL_Return_t SPL_ShiftLeft_vint16_F(      sint16*         const Out_p,
                                     const sint16*         const In_p,
                                     const sint16                shiftValue,
                                     const sint16                L);

SPL_Return_t SPL_ShiftLeft_vint32_F(      sint32*         const Out_p,
                                    const sint32*         const In_p,
                                    const sint16                shiftValue,
                                    const sint16                L);

SPL_Return_t SPL_ShiftRightRound_vint16_F(      sint16*         const Out_p,
                                          const sint16*         const In_p,
                                          const sint16                shiftValue,
                                          const sint16                L);

SPL_Return_t SPL_ShiftRightRound_vint32_F(      sint32*         const Out_p,
                                          const sint32*         const In_p,
                                          const sint16                shiftValue,
                                          const sint16                L);

SPL_Return_t SPL_ShiftRight_vint16_F(      sint16*         const Out_p,
                                     const sint16*         const In_p,
                                     const sint16                shiftValue,
                                     const sint16                L);

SPL_Return_t SPL_ShiftRight_vint32_F(      sint32*         const Out_p,
                                     const sint32*         const In_p,
                                     const sint16                shiftValue,
                                     const sint16                L);

SPL_Return_t SPL_Sqrt_sint16Q_F(      SPL_sint16Q_t*  X_Root_p,
                                const SPL_sint16Q_t*  X_p);

SPL_Return_t SPL_Sqrt_vint16Q_F(      SPL_vint16Q_t*  X_Root_p,
                                const SPL_vint16Q_t*  X_p);

SPL_Return_t SPL_Sqrt_vint32Q_F(      SPL_vint16Q_t*  X_Root_p,
                                const SPL_vint32Q_t*  X_p);

SPL_Return_t SPL_SquaredSum_vint16_F(      SPL_sint16Q_t*  const SquaredSum_p,
                                     const sint16*         const x_p,
                                     const sint16                length);

SPL_Return_t SPL_SquaredSum_vint16Q_F(      SPL_sint32Q_t*  const SquaredSum_p,
                                      const SPL_vint16Q_t*  const X_p,
                                      const sint16                start,
                                      const sint16                length);

SPL_Return_t SPL_Sub_vint16_F(      sint16*         const Diff_p,
                              const sint16*         const X_p,
                              const sint16*         const Y_p,
                              const sint16                L);

SPL_Return_t SPL_Sub_vint16_sint16_F(       sint16*        const Diff_p,
                                     const sint16*        const X_p, 
                                     const sint16               term,
                                     const sint16               L);

SPL_Return_t SPL_Sub_vint16_vint16Q_F(      sint16*        const Diff_p,
                                      const sint16*        const X_p, 
                                      const SPL_vint16Q_t* const Y_p);
 
SPL_Return_t SPL_Sub_vint16Q_F(      SPL_vint16Q_t*  const Diff_p,
                               const SPL_vint16Q_t*  const X_p,
                               const SPL_vint16Q_t*  const Y_p);

SPL_Return_t SPL_Sub_vint16Q_sint16Q_F(      SPL_vint16Q_t*  const Sum_p,
                                       const SPL_vint16Q_t*  const X_p,
                                       const SPL_sint16Q_t*  const Term_p);

SPL_Return_t SPL_Sub_vint16Q_vint16_F(      SPL_vint16Q_t*  const Diff_p,
                                      const SPL_vint16Q_t*  const X_p,
                                      const sint16*         const Y_p);

SPL_Return_t SPL_Sub_vint32Q_F(      SPL_vint32Q_t*  const Diff_p,
                               const SPL_vint32Q_t*  const X_p,
                               const SPL_vint32Q_t*  const Y_p);

SPL_Return_t SPL_Sum_vint16_F(      sint32*  const y_p,
                              const sint16*  const x_p,
                              const sint16         L);

SPL_Return_t SPL_Sum_vint16Q_F(      SPL_sint16Q_t*  const X_Summed_p,
                               const SPL_vint16Q_t*  const X_p,
                               const sint16                start,
                               const sint16                length);

SPL_Return_t SPL_Sum_vint32Q_F(      SPL_sint32Q_t*  const X_Summed_p,
                               const SPL_vint32Q_t*  const X_p,
                               const sint16                start,
                               const sint16                length);


/***********************************************************************
 *
 * Logic functions
 *
 ***********************************************************************/
SPL_Return_t SPL_And_vint16_F(      sint16*  const flag_p,
                              const sint16*  const x_p,
                              const sint16*  const y_p,
                              const sint16         L);
                            
SPL_Return_t SPL_BlockMax_vint32Q_F(      SPL_vint32Q_t*  const Max_p,
                                          sint16*         const index_p,
                                    const SPL_vint32Q_t*  const X_p,
                                    const sint16                bandwidthNumBits);

SPL_Return_t SPL_Compare_sint16Q_F(      SPL_Boolean_t*  const Flag_p,
                                  const SPL_sint16Q_t*  const X_p,
                                  const SPL_sint16Q_t*  const Y_p,
                                  const SPL_Compare_t         comparison);

SPL_Return_t SPL_Compare_sint32Q_F(      SPL_Boolean_t*  const Flag_p,
                                  const SPL_sint32Q_t*  const X_p,
                                  const SPL_sint32Q_t*  const Y_p,
                                  const SPL_Compare_t         comparison);


SPL_Return_t SPL_Compare_vint16Q_F(      sint16*         const Flag_p,
                                  const SPL_vint16Q_t*  const X_p,
                                  const SPL_vint16Q_t*  const Y_p,
                                  const SPL_Compare_t         comparison);


SPL_Return_t SPL_Compare_vint32Q_F(      sint16*         const Flag_p,
                                  const SPL_vint32Q_t*  const X_p,
                                  const SPL_vint32Q_t*  const Y_p,
                                  const SPL_Compare_t         comparison);


SPL_Return_t SPL_Find_vint16Q_F(      sint16*         const Flag_p,
                               const SPL_vint16Q_t*  const X_p,
                               const SPL_sint16Q_t*  const Value_p,
                               const SPL_Compare_t         comparison);


SPL_Return_t SPL_Find_vint16_F(      sint16*        const Flag_p,
                              const sint16*        const X_p,
                              const sint16               Value,
                              const SPL_Compare_t        comparison,
                              const sint16               L);


SPL_Return_t SPL_Find_vint32_F(      sint16*        const Flag_p,
                              const sint32*        const X_p,
                              const sint32               Value,
                              const SPL_Compare_t        comparison,
                              const sint16               L);


SPL_Return_t SPL_Find_vint32Q_F(      sint16*         const Flag_p,
                               const SPL_vint32Q_t*  const X_p,
                               const SPL_sint32Q_t*  const Value_p,
                               const SPL_Compare_t         comparison);


SPL_Boolean_t SPL_IsZeroVector_vint16Q(const SPL_vint16Q_t* const X_p);

SPL_Boolean_t SPL_IsZeroVector_vint32Q( const SPL_vint32Q_t* const X_p );


SPL_Return_t SPL_Mask_vint16_F(      sint16*  const x_masked_p,
                              const sint16*  const x_p,
                              const sint16*  const mask_p,
                              const sint16         L);


SPL_Return_t SPL_Mask_vint16Q_F(      SPL_vint16Q_t*  const X_masked_p,
                               const SPL_vint16Q_t*  const X_p,
                               const sint16*         const mask_p);


SPL_Return_t SPL_Mask_vint32_F(      sint32*  const x_masked_p,
                              const sint32*  const x_p,
                              const sint16*  const mask_p,
                              const sint16         L);


SPL_Return_t SPL_Mask_vint32Q_F(      SPL_vint32Q_t*  const X_masked_p,
                               const SPL_vint32Q_t*  const X_p,
                               const sint16*         const mask_p);
                             

SPL_Return_t SPL_Max_vint16_F(      sint16*  const Max_p,
                                    sint16*  const index_p,
                              const sint16*  const x_p,
                              const sint16         L);

SPL_Return_t SPL_Max_sint16Q_F(      SPL_sint16Q_t*  const Max_p,
                               const SPL_sint16Q_t*  const X_p,
                               const SPL_sint16Q_t*  const Y_p);

SPL_Return_t SPL_Max_vint16Q_F(      SPL_sint16Q_t*  const Max_p,
                                            sint16*  const index_p,
                               const SPL_vint16Q_t*  const X_p);


SPL_Return_t SPL_Max_vint32Q_F(      SPL_sint32Q_t*  const Max_p,
                                            sint16*  const index_p,
                               const SPL_vint32Q_t*  const X_p);


SPL_Return_t SPL_Min_vint16Q_F(      SPL_sint16Q_t*  const Min_p,
                                     sint16*         const index_p,
                               const SPL_vint16Q_t*  const X_p);


SPL_Return_t SPL_Min_vint32Q_F(      SPL_sint32Q_t*  const Min_p,
                                            sint16*  const index_p,
                               const SPL_vint32Q_t*  const X_p);


SPL_Return_t SPL_Not_vint16_F(      sint16*  const flag_p,
                              const sint16*  const x_p,
                              const sint16         L);

                             
SPL_Return_t SPL_Or_vint16_F(      sint16*  const flag_p,
                            const sint16*  const x_p,
                            const sint16*  const y_p,
                            const sint16         L);


SPL_Return_t SPL_Vmax_vint16_F(      sint16*  const Max_p,
                               const sint16*  const X_p,
                               const sint16*  const Y_p,
                               const sint16         L);


SPL_Return_t SPL_Vmax_vint16Q_F(      SPL_vint16Q_t*  const Max_p,
                                const SPL_vint16Q_t*  const X_p,
                                const SPL_vint16Q_t*  const Y_p);


SPL_Return_t SPL_Vmax_vint32Q_F(      SPL_vint32Q_t*  const Max_p,
                                const SPL_vint32Q_t*  const X_p,
                                const SPL_vint32Q_t*  const Y_p);


SPL_Return_t SPL_Vmin_vint16_F(      sint16*  const Min_p,
                               const sint16*  const X_p,
                               const sint16*  const Y_p,
                               const sint16         L);


SPL_Return_t SPL_Vmin_vint16Q_F(      SPL_vint16Q_t*  const Min_p,
                                const SPL_vint16Q_t*  const X_p,
                                const SPL_vint16Q_t*  const Y_p);


SPL_Return_t SPL_Vmin_vint32Q_F(      SPL_vint32Q_t*  const Min_p,
                                const SPL_vint32Q_t*  const X_p,
                                const SPL_vint32Q_t*  const Y_p);


/***********************************************************************
 *
 * Signal processing functions
 *
 ***********************************************************************/

SPL_Return_t SPL_Average_sint16Q_F(      SPL_sint16Q_t*  const Average_p,
                                   const SPL_sint16Q_t*  const X_p,
                                   const SPL_sint16Q_t*  const Y_p,
                                   const SPL_sint16Q_t*  const Rho_p);


SPL_Return_t SPL_Average_vint16Q_F(      SPL_vint16Q_t*  const Average_p,
                                  const SPL_vint16Q_t*  const X_p,
                                  const SPL_vint16Q_t*  const Y_p,
                                  const SPL_vint16Q_t*  const Rho_p);


SPL_Return_t SPL_Average_vint32Q_F(      SPL_vint32Q_t*  const Average_p,
                                  const SPL_vint32Q_t*  const X_p,
                                  const SPL_vint32Q_t*  const Y_p,
                                  const SPL_vint16Q_t*  const Rho_p);

                                
SPL_Return_t SPL_BiQuad_vint16_F(const sint16 *const InputBuffer_p ,
                                       sint16 *const OutputBuffer_p,
                                 const sint16  L,
                                       SPL_BiQuad_t *const StateBuffer_p);

// todo: both 16- and 32-bit fft routines are needed in addition to float ref
SPL_Return_t SPL_Fft_vint16Q_F( const SPL_vint16Q_t*       const inData_p,
                                      SPL_vint16Q_t*       const outData_p,
                                const sint16                     numBits,
                                const SPL_FFT_Data_t             dataType,
                                const SPL_FFT_Normalize_t        normFlag   );

SPL_Return_t SPL_Fft_vint16Q_FLT(const SPL_vint16Q_t*       const inData_p,
                                     SPL_vint16Q_t*       const outData_p,
                               const sint16                     numBits,
                               const SPL_FFT_Data_t             dataType,
                               const SPL_FFT_Normalize_t        normFlag);

SPL_Return_t SPL_Fft_vint16Q_16x16_F(const SPL_vint16Q_t*       const inData_p,
                                           SPL_vint16Q_t*       const outData_p,
                                     const sint16                     numBits,
                                     const SPL_FFT_Data_t             dataType,
                                     const SPL_FFT_Normalize_t        normFlag);


SPL_Return_t SPL_Fft_vint16Q_32x16_F(const SPL_vint16Q_t*       const inData_p,
                                           SPL_vint16Q_t*       const outData_p,
                                     const sint16                     numBits,
                                     const SPL_FFT_Data_t             dataType,
                                     const SPL_FFT_Normalize_t        normFlag);

SPL_Return_t SPL_FirFilterQ_vint16_F(      sint16*           const outputData_p,
                                    const sint16*           const inputData_p,
                                    const int                     dataLength,
                                    const SPL_vint16Q_t*    const Fir_p,
                                    const SPL_Filter_Type_t       filterType);

                    
SPL_Return_t SPL_FirFrequencySampling_vint16Q_F(      SPL_vint16Q_t*    const g_p,
                                                const sint16*           const G_p,
                                                const int                     lengthNumBits,
                                                const SPL_Filter_Type_t       outputType);


// todo: both 16- and 32-bit fft routines are needed in addition to float ref
SPL_Return_t SPL_InverseFft_vint16Q_16x16_F(const SPL_vint16Q_t*  const inData_p,
                                                  SPL_vint16Q_t*  const outData_p,
                                            const sint16                numBits,
                                            const SPL_FFT_Data_t        dataType);

SPL_Return_t SPL_InverseFft_vint16Q_32x16_F(const SPL_vint16Q_t*  const inData_p,
                                                  SPL_vint16Q_t*  const outData_p,
                                            const sint16                numBits,
                                            const SPL_FFT_Data_t        dataType);
  
SPL_Return_t SPL_Periodogram_F(const SPL_vint16Q_t*  const Fft_p,
                                     SPL_vint32Q_t*  const Periodogram_p,
                               const sint16                fftLength,
                               const sint16                bandwidthNumBits);


SPL_Return_t SPL_InverseFft_vint16Q_F( const SPL_vint16Q_t*  const inData_p,
                                             SPL_vint16Q_t*  const outData_p,
                                       const sint16                numBits,
                                       const SPL_FFT_Data_t        dataType  );
  
SPL_Return_t SPL_InverseFft_vint16Q_FLT( const SPL_vint16Q_t*  const inData_p,
                                               SPL_vint16Q_t*  const outData_p,
                                         const sint16                numBits,
                                         const SPL_FFT_Data_t        dataType   );

SPL_Return_t SPL_FIR_vint16(const sint16*        const InputData_p,
                                  sint16*        const OutputData_p,
                            const int                  FrameLength,
                                  SPL_vint16Q_t*       FIR_p,
                                  sint16*              Delayline_p);

SPL_Return_t SPL_FIR_vint32(const sint32*        const InputData_p,
                                  sint32*        const OutputData_p,
                            const int                  FrameLength,
                                  SPL_vint16Q_t*       FIR_p,
                                  sint32*              Delayline_p);

SPL_Return_t SPL_FIR_vint16_to_vint32(const sint16*        const InputData_p,
                                            sint32*        const OutputData_p,
                                      const int                  FrameLength,
                                            SPL_vint16Q_t*       FIR_p,
                                            sint16*              Delayline_p);

SPL_Return_t SPL_FIR_vint32_to_vint16(const sint32*        const InputData_p,
                                            sint16*        const OutputData_p,
                                      const int                  FrameLength,
                                            SPL_vint16Q_t*       FIR_p,
                                            sint32*              Delayline_p);


/***********************************************************************
 *
 * Data storage functions
 *
 ***********************************************************************/
 
SPL_Return_t SPL_InitCircularBuffer_F(      SPL_CircularBuffer_t*  const CircularBuffer_p,
                                           sint16*                const data_p,
                                     const int                          circularIndex,
                                     const int                          bufferLength);

SPL_Return_t SPL_ReadCircularBuffer_F(      sint16*                       const outData_p,
                                                  SPL_CircularBuffer_t*  const CircularBuffer_p,
                                            const sint16                       nDelay,
                                            const sint16                       nExtract);
  
SPL_Return_t SPL_StoreCircularBuffer_F(const sint16*                const newData_p,
                                            SPL_CircularBuffer_t*  const CircularBuffer_p,
                                      const sint16                       nStore);


/***********************************************************************
 *
 * Memory functions
 *
 ***********************************************************************/
SPL_Return_t SPL_Copy_vint16Q_F(      SPL_vint16Q_t*  const OutData_p,
                               const SPL_vint16Q_t*  const InData_p);


SPL_Return_t SPL_Copy_vint16Q_vint16_F(      SPL_vint16Q_t*  const OutData_p,
                                      const sint16*         const inData_p,
                                      const sint16                start,
                                      const sint16                length);

SPL_Return_t SPL_Copy_vint32Q_F(      SPL_vint32Q_t*  const OutData_p,
                               const SPL_vint32Q_t*  const InData_p);


SPL_Return_t SPL_Copy_vint32Q_vint16_F(      SPL_vint32Q_t*  const OutData_p,
                                       const sint16*         const inData_p,
                                       const sint16                start,
                                       const sint16                length);


SPL_Return_t SPL_Get_sint16_F(      sint16*         const data_p,
                              const SPL_sint16Q_t*  const Data_p,
                              const sint16                Q);


SPL_Return_t SPL_Get_vint16_F(      sint16*         const data_p,
                             const SPL_vint16Q_t*  const Data_p,
                             const sint16                Q);


SPL_Return_t SPL_Resample_vint16Q_F(      SPL_vint16Q_t* const X_resampled_p,
                                   const SPL_vint16Q_t* const X_p,
                                   const int                  resampleFactor);


SPL_Return_t SPL_Resample_vint16_F(      sint16*  const X_resampled_p,
                                  const sint16*  const X_p,
                                  const int            resampleFactor,
                                  const int            L);


SPL_Return_t SPL_Set_vint16Q_F(      SPL_vint16Q_t*  const Data_p,
                              const sint16                value);


SPL_Return_t SPL_Set_vint16_F(      sint16*  const data_p,
                             const sint16         value,
                             const sint16         L);


SPL_Return_t SPL_Set_vint32Q_F(      SPL_vint32Q_t*  const Data_p,
                              const sint32                value);



/***********************************************************************
 *
 * Composite functions based on lower level functions
 *
 ***********************************************************************/

SPL_Return_t SPL_ConditionalAverage_vint16Q_F(      SPL_vint16Q_t*  const Average_p,
                                             const SPL_vint16Q_t*  const Y_p,
                                             const sint16                rho,
                                             const sint16*         const mask_p);


SPL_Return_t SPL_ConditionalCopy_vint16Q_F(      SPL_vint16Q_t*  const X_p,
                                          const SPL_vint16Q_t*  const Y_p,
                                          const sint16*         const mask_p);


SPL_Return_t SPL_ConditionalCopy_vint32Q_F(      SPL_vint32Q_t*  const X_p,
                                           const SPL_vint32Q_t*  const Y_p,
                                           const sint16*         const mask_p);


/***********************************************************************
 *
 * Low level fixed point instructions based on ITU-T STL 
 *
 ***********************************************************************/
//#ifndef __arm__
sint16 SPL_add (sint16 var1, sint16 var2);

sint16 SPL_sub (sint16 var1, sint16 var2);

sint16 SPL_abs_s (sint16 var1);

sint16 SPL_shl(sint16 var1, sint16 var2);

sint16 SPL_shr (sint16 var1, sint16 var2);

sint16 SPL_mult (sint16 var1, sint16 var2);

sint32 SPL_L_mult (sint16 var1, sint16 var2);

sint16 SPL_negate (sint16 var1);

sint16 SPL_extract_h (sint32 L_var1);

sint16 SPL_extract_l (sint32 L_var1);

sint16 SPL_round (sint32 L_var1);

sint32 SPL_L_mac (sint32 L_var3, sint16 var1, sint16 var2);

sint32 SPL_L_msu (sint32 L_var3, sint16 var1, sint16 var2);

sint32 SPL_L_macNs (sint32 L_var3, sint16 var1, sint16 var2);

sint32 SPL_L_msuNs (sint32 L_var3, sint16 var1, sint16 var2);

sint32 SPL_L_add (sint32 L_var1, sint32 L_var2);

sint32 SPL_L_sub (sint32 L_var1, sint32 L_var2);

sint32 SPL_L_add_c (sint32 L_var1, sint32 L_var2);

sint32 SPL_L_sub_c (sint32 L_var1, sint32 L_var2);

sint32 SPL_L_negate (sint32 L_var1);

sint16 SPL_mult_r (sint16 var1, sint16 var2);

sint32 SPL_L_shl (sint32 L_var1, sint16 var2);

sint32 SPL_L_shr (sint32 L_var1, sint16 var2);

sint16 SPL_shr_r (sint16 var1, sint16 var2);

sint16 SPL_mac_r (sint32 L_var3, sint16 var1, sint16 var2);

sint16 SPL_msu_r (sint32 L_var3, sint16 var1, sint16 var2);

sint32 SPL_L_deposit_h (sint16 var1);

sint32 SPL_L_deposit_l (sint16 var1);

sint32 SPL_L_shr_r (sint32 L_var1, sint16 var2);

sint32 SPL_L_abs (sint32 L_var1);

sint32 SPL_L_sat (sint32 L_var1);

sint16 SPL_norm_s (sint16 var1);

sint16 SPL_div_s (sint16 var1, sint16 var2);

sint16 SPL_norm_l (sint32 L_var1);

//#endif

sint16 SPL_Headroom( sint64 sample );

#endif

