#ifndef INCLUSION_GUARD_SPL_FIX_FUNCTION_USAGE
#define INCLUSION_GUARD_SPL_FIX_FUNCTION_USAGE

/*
 * $Id: spl_fix_function_usage.h,v 1.1 2009/10/02 12:08:27 erauae Exp $
 *
 */

/*************************************************************************
*  © ST-Ericsson, 2010 - All rights reserved
*
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson.
**************************************************************************
*
* DESCRIPTION:
*
* Controls the use of fix routines
*
**************************************************************************
*
* REVISION HISTORY
*
* $Log: spl_fix_function_usage.h,v $
* Revision 1.1  2009/10/02 12:08:27  erauae
* Added SPL library for float to fix conversion.
*
* Revision 1.1  2009/09/04 14:43:07  erauae
* Added files to assist in float to fix conversion.
*
*
*
*************************************************************************/

#define SPL_FIX_ADD_VINT16Q 1
#define SPL_FIX_OUTPUT_ADD_VINT16Q 0

//#define SPL_BLOCKSUM_VINT32Q

#define INTERMEDIATE_LOG_SINT16Q_SINT16Q //rev done
#define INTERMEDIATE_LOG_SINT16Q_SINT16 //rev done 

#define INTERMEDIATE_SQUAREDSUM_VINT16Q //rev done
#define INTERMEDIATE_SQUAREDSUM_VINT16 //rev done

//#define INTERMEDIATE_FFT_VINT16Q
//#define INTERMEDIATE_INVERSE_FFT_VINT16Q
#define INTERMEDIATE_COPY_VINT16Q 

#define USE_64_BIT_ACCUMULATORS_IN_FIXED_POINT_FFT_PACK_ROUTINE
#define USE_64_BIT_ACCUMULATORS_IN_FIXED_POINT_FFT_UNPACK_ROUTINE

#define INTERMEDIATE_ABS_SQUARE_CX_VINT16Q //rev done

#define INTERMEDIATE_ABS_SQUARE_CX_VINT32Q //rev done

#define INTERMEDIATE_ADD_SINT_16Q //rev done

#define FIXED_POINT_ADD_VINT16 //rev done

#define INTERMEDIATE_ADD_VINT16Q //rev done
                                 // If this is defined then INTERMEDIATE_IS_ZERO_VECTOR_VINT16Q
                                 // must also be defined

//#define INTERMEDIATE_ADD_VINT16Q_SINT16Q  // is not used by the echo canceller

#define INTERMEDIATE_ADD_VINT32Q //rev done

#define INTERMEDIATE_ADD_VINT32Q_SINT16Q //rev done
#define INTERMEDIATE_AVERAGE_SINT16Q //rev done

#define INTERMEDIATE_AVERAGE_VINT16Q //rev done

#define INTERMEDIATE_AVERAGE_VINT32Q //rev done
#define INTERMEDIATE_COMPARE_SINT16Q //rev done
#define INTERMEDIATE_COMPARE_VINT16Q //rev done
#define INTERMEDIATE_COMPARE_VINT32Q //rev done

#define INTERMEDIATE_CONDITIONAL_AVERAGE_VINT16Q // If this is defined then INTERMEDIATE_ADD_VINT16Q and INTERMEDIATE_SUB_VINT16Q must be defined
#define INTERMEDIATE_CONDITIONAL_COPY_VINT16Q // If this is defined then INTERMEDIATE_MASK_VINT16Q and INTERMEDIATE_ADD_VINT16Q must be defined
#define INTERMEDIATE_CONDITIONAL_COPY_VINT32Q // If this is defined INTERMEDIATE_MASK_VINT32Q and INTERMEDIATE_ADD_VINT32Q must be defined
#define INTERMEDIATE_CONVERT_VINT32Q_VINT16Q

#define INTERMEDIATE_COPY_VINT32  // rev done 

#define INTERMEDIATE_FIND_VINT32Q // rev done

#define INTERMEDIATE_FIR_FILTERQ_VINT16

#define INTERMEDIATE_GET_SINT16 

#define INTERMEDIATE_GET_VINT16

#define INTERMEDIATE_IS_ZERO_VECTOR_VINT16Q // rev done

#define INTERMEDIATE_IS_ZERO_VECTOR_VINT32Q // rev done

//#define INTERMEDIATE_LIMIT_VINT16Q // makes the AEC converge a bit slower 

#define INTERMEDIATE_LIMIT_VINT32Q

//#define INTERMEDIATE_MAC_CX_VINT16Q_CONJ

//#define INTERMEDIATE_MAC_CX_VINT16Q_NOT_CONJ 

#define INTERMEDIATE_MASK_VINT16Q
#define INTERMEDIATE_MASK_VINT32Q

#define INTERMEDIATE_MAX_VINT16Q 
#define INTERMEDIATE_MAX_VINT32Q
#define INTERMEDIATE_MULT_CX_VINT16Q_R_VINT16Q
#define INTERMEDIATE_MULT_SINT16Q_SINT16Q
#define INTERMEDIATE_MULT_VINT16_SINT16Q
#define FIXED_POINT_MULT_VINT16_SINT16
#define INTERMEDIATE_MULT_VINT16Q_SINT16Q

#define INTERMEDIATE_MULT_VINT16Q_VINT16Q 
#define INTERMEDIATE_MULT_VINT16Q_VINT32Q
#define INTERMEDIATE_MULT_VINT16Q
#define FIXED_POINT_MULT_VINT16

#define INTERMEDIATE_MULT_VINT32Q 
#define INTERMEDIATE_SET_VINT16Q

#define INTERMEDIATE_SET_VINT32Q

#define INTERMEDIATE_SUB_SINT16Q 
#define INTERMEDIATE_SUB_VINT16Q // If this is defined then INTERMEDIATE_IS_ZERO_VECTOR_VINT16Q must also be defined

#define INTERMEDIATE_SUB_VINT32Q // If this is defined then INTERMEDIATE_IS_ZERO_VECTOR_VINT32Q must also be defined
#define FIXED_POINT_SUB_VINT16
#define INTERMEDIATE_SUM_VINT16Q // works for current test case, but must find another way of taking care of overflow (or do we?)
#define INTERMEDIATE_SUM_VINT32Q
#define INTERMEDIATE_VMAX_VINT16Q
#define INTERMEDIATE_VMAX_VINT32Q
#define INTERMEDIATE_VMIN_VINT16Q
#define INTERMEDIATE_VMIN_VINT32Q

#define INTERMEDIATE_MULT_VINT32Q_VINT16Q
#define INTERMEDIATE_MULT_VINT32Q_SINT16Q
#define INTERMEDIATE_COPY_VINT16Q_VINT16
#define INTERMEDIATE_MULT_SINT32_SINT16
#define INTERMEDIATE_COMPARE_SINT32Q
#define INTERMEDIATE_CONVERT_SINT32Q_SINT16Q
#define INTERMEDIATE_SETQ_SINT16Q
/*
#define INTERMEDIATE_DIV_SINT16Q
#define INTERMEDIATE_DIV16Q_VINT32Q_VINT32Q
#define INTERMEDIATE_DIV16Q_SINT32Q_SINT32Q
#define INTERMEDIATE_DIV_VINT16Q_VINT32Q

#ifdef __arm__
#define INTERMEDIATE_SQRT_SINT16Q
#define INTERMEDIATE_SQRT_VINT16Q
#define INTERMEDIATE_SQRT_VINT32Q
#endif
*/

#endif

