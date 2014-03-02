/***********************************************************************/
/**
 *  © ST-Ericsson, 2010 - All rights reserved
 *
 *  Reproduction and Communication of this document is strictly prohibited
 *  unless specifically authorized in writing by ST-Ericsson.
 */
/***********************************************************************/

/*
 * spl_fixed_point_functions.h
 *
 *  Created on: Jan 8, 2010
 *      Author: esarjon
 */

#ifndef SPL_FIXED_POINT_FUNCTIONS_H_
#define SPL_FIXED_POINT_FUNCTIONS_H_

#include "spl_flt_to_fix.h"

SPL_Return_t SPL_AbsSquare_Cx_vint16Q_FIX(      SPL_FIX_vint32Q_t*  const X_Squared_p,
                                          const SPL_FIX_vint16Q_t*  const X_p);

SPL_Return_t SPL_AbsSquare_Cx_vint32Q_FIX(      SPL_FIX_vint32Q_t*  const X_Squared_p,
                                          const SPL_FIX_vint32Q_t*  const X_p);

SPL_Return_t SPL_Add_sint16Q_FIX(      SPL_FIX_sint16Q_t*  const sum_p,
                                 const SPL_FIX_sint16Q_t*  const x_p,
                                 const SPL_FIX_sint16Q_t*  const y_p);

SPL_Return_t SPL_Add_vint16Q_sint16Q_FIX(      SPL_FIX_vint16Q_t*  const Sum_p,
                                         const SPL_FIX_vint16Q_t*  const X_p,
                                         const SPL_FIX_sint16Q_t*  const Term_p);

SPL_Return_t SPL_Add_vint16Q_FIX(      SPL_FIX_vint16Q_t*  const Sum_p,
                                 const SPL_FIX_vint16Q_t*  const X_p,
                                 const SPL_FIX_vint16Q_t*  const Y_p);

SPL_Return_t SPL_Add_vint32Q_sint16Q_FIX(      SPL_FIX_vint32Q_t*  const Sum_p,
                                         const SPL_FIX_vint32Q_t*  const X_p,
                                         const SPL_FIX_sint16Q_t*  const Term_p);

SPL_Return_t SPL_Add_vint32Q_FIX(      SPL_FIX_vint32Q_t*  const Sum_p,
                                 const SPL_FIX_vint32Q_t*  const X_p,
                                 const SPL_FIX_vint32Q_t*  const Y_p);

SPL_Return_t SPL_Average_sint16Q_FIX(      SPL_FIX_sint16Q_t*  const Average_p,
                                     const SPL_FIX_sint16Q_t*  const X_p,
                                     const SPL_FIX_sint16Q_t*  const Y_p,
                                     const SPL_FIX_sint16Q_t*  const Rho_p);

SPL_Return_t SPL_Average_vint16Q_FIX(      SPL_FIX_vint16Q_t*  const Average_p,
                                     const SPL_FIX_vint16Q_t*  const X_p,
                                     const SPL_FIX_vint16Q_t*  const Y_p,
                                     const SPL_FIX_vint16Q_t*  const Rho_p);

SPL_Return_t SPL_Average_vint32Q_FIX(       SPL_FIX_vint32Q_t*  const Average_p,
                                      const SPL_FIX_vint32Q_t*  const X_p,
                                      const SPL_FIX_vint32Q_t*  const Y_p,
                                      const SPL_FIX_vint16Q_t*  const Rho_p     );

SPL_Return_t SPL_Compare_sint16Q_FIX(      SPL_Boolean_t*     const Flag_p,
                                     const SPL_FIX_sint16Q_t* const X_p,
                                     const SPL_FIX_sint16Q_t* const Y_p,
                                     const SPL_Compare_t            comparison);

SPL_Return_t SPL_Compare_vint16Q_FIX(      sint16*            const Flag_p,
                                     const SPL_FIX_vint16Q_t* const X_p,
                                     const SPL_FIX_vint16Q_t* const Y_p,
                                     const SPL_Compare_t            comparison);

SPL_Return_t SPL_Compare_vint32Q_FIX(      sint16*            const Flag_p,
                                     const SPL_FIX_vint32Q_t* const X_p,
                                     const SPL_FIX_vint32Q_t* const Y_p,
                                     const SPL_Compare_t            comparison);

SPL_Return_t SPL_ConditionalAverage_vint16Q_FIX(      SPL_FIX_vint16Q_t*  const Average_p,
                                                const SPL_FIX_vint16Q_t*  const Y_p,
                                                const sint16                    rhoSft,
                                                const sint16*             const mask_p);

SPL_Return_t SPL_ConditionalCopy_vint16Q_FIX(      SPL_FIX_vint16Q_t*  const X_p,
                                             const SPL_FIX_vint16Q_t*  const Y_p,
                                             const sint16*             const flag_p);

SPL_Return_t SPL_ConditionalCopy_vint32Q_FIX(      SPL_FIX_vint32Q_t*  const X_p,
                                             const SPL_FIX_vint32Q_t*  const Y_p,
                                             const sint16*             const flag_p);

SPL_Return_t SPL_Convert_vint32Q_vint16Q_FIX(      SPL_FIX_vint16Q_t* const Y_p,
                                             const SPL_FIX_vint32Q_t* const X_p);

SPL_Return_t SPL_Copy_vint16Q_FIX(      SPL_FIX_vint16Q_t* const OutData_p,
                                  const SPL_FIX_vint16Q_t* const InData_p);

SPL_Return_t SPL_Copy_vint32Q_FIX(      SPL_FIX_vint32Q_t*  const OutData_p,
                                  const SPL_FIX_vint32Q_t*  const InData_p);

SPL_Return_t SPL_Find_vint32Q_FIX(      sint16*         const Flag_p,
                                  const SPL_FIX_vint32Q_t*  const X_p,
                                  const SPL_FIX_sint32Q_t*  const Value_p,
                                  const SPL_Compare_t         comparison);

SPL_Return_t SPL_FirFilterQ_vint16_F_fix(      sint16*            const outputData_p,
                                         const sint16*            const inputData_p,
                                         const int                      dataLength,
                                         const SPL_FIX_vint16Q_t* const Fir_FIX_p,
                                         const SPL_Filter_Type_t        filterType);

SPL_Return_t SPL_Get_vint16_FIX(      sint16*         const data_p,
                                const SPL_FIX_vint16Q_t*  const Data_p,
                                const sint16                Q);

SPL_Boolean_t SPL_IsZeroVector_vint16( const sint16 * const v_p, sint16 L );

SPL_Boolean_t SPL_IsZeroVector_vint16Q_FIX( const SPL_FIX_vint16Q_t* const X_p );

SPL_Boolean_t SPL_IsZeroVector_vint32Q_FIX( const SPL_FIX_vint32Q_t* const X_p );

SPL_Return_t SPL_Limit_vint16Q_FIX(      SPL_FIX_vint16Q_t*  const X_limit_p,
                                   const SPL_FIX_vint16Q_t*  const X_p,
                                   const SPL_FIX_sint16Q_t*  const Limit_p,
                                   const SPL_Limit_t               comparison);

SPL_Return_t SPL_Limit_vint32Q_FIX(      SPL_FIX_vint32Q_t*  const X_limit_p,
                                   const SPL_FIX_vint32Q_t*  const X_p,
                                   const SPL_FIX_sint32Q_t*  const Limit_p,
                                   const SPL_Limit_t               comparison);

SPL_Return_t SPL_Log2_sint16Q_sint16Q_FIX( SPL_FIX_sint16Q_t * In_p, SPL_FIX_sint16Q_t * Out_p );

SPL_Return_t SPL_Mac_Cx_vint16Q_Conj_FIX(      SPL_FIX_vint16Q_t* const Acc_p,
                                         const SPL_FIX_vint16Q_t* const X_p,
                                         const SPL_FIX_vint16Q_t* const Y_p   );

SPL_Return_t SPL_Mac_Cx_vint16Q_Not_Conj_FIX(      SPL_FIX_vint16Q_t* const Acc_p,
                                             const SPL_FIX_vint16Q_t* const X_p,
                                             const SPL_FIX_vint16Q_t* const Y_p   );

SPL_Return_t SPL_Mask_vint16Q_FIX(      SPL_FIX_vint16Q_t*  const X_masked_p,
                                  const SPL_FIX_vint16Q_t*  const X_p,
                                  const sint16*         const mask_p);

SPL_Return_t SPL_Mask_vint32Q_FIX(      SPL_FIX_vint32Q_t*  const X_masked_p,
                                  const SPL_FIX_vint32Q_t*  const X_p,
                                  const sint16*         const mask_p);

SPL_Return_t SPL_Max_vint16Q_FIX(      SPL_FIX_sint16Q_t*  const Max_p,
                                       sint16*             const index_p,
                                 const SPL_FIX_vint16Q_t*  const X_p);

SPL_Return_t SPL_Max_vint32Q_FIX(       SPL_FIX_sint32Q_t*  const Max_p,
							  	        sint16*             const index_p,
								  const SPL_FIX_vint32Q_t*  const X_p      );

SPL_Return_t SPL_Mult_Cx_vint16Q_R_vint16Q_FIX(      SPL_FIX_vint16Q_t* const Product_p,
                                               const SPL_FIX_vint16Q_t* const X_p,
                                               const SPL_FIX_vint16Q_t* const Y_p,
                                               const sint16                   bandwidth_Y);

SPL_Return_t SPL_Mult_sint16Q_sint16Q_FIX(      SPL_FIX_sint16Q_t*  const product_p,
                                          const SPL_FIX_sint16Q_t*  const x_p,
                                          const SPL_FIX_sint16Q_t*  const y_p);

SPL_Return_t SPL_Mult_vint16_sint16Q_FIX(      sint16*            const Product_p,
                                         const sint16*            const X_p,
                                         const sint16                   length,
                                         const SPL_FIX_sint16Q_t* const Factor_p);

SPL_Return_t SPL_Mult_vint16_sint16_FIX(      sint16*        const Product_p,
                                        const sint16*        const X_p,
                                        const sint16               factor,
                                        const sint16               length);

SPL_Return_t SPL_Mult_vint16Q_sint16Q_FIX(      SPL_FIX_vint16Q_t* const Product_p,
                                          const SPL_FIX_vint16Q_t* const X_p,
                                          const SPL_FIX_sint16Q_t* const Factor_p);

SPL_Return_t SPL_Mult_vint16Q_vint16_FIX(      SPL_FIX_vint16Q_t* const Product_p,
                                         const SPL_FIX_vint16Q_t* const X_p,
                                         const sint16*            const Y_p,
                                         const sint16                   length);

SPL_Return_t SPL_Mult_vint16Q_FIX(      SPL_FIX_vint16Q_t* const Product_p,
                                  const SPL_FIX_vint16Q_t* const X_p,
                                  const SPL_FIX_vint16Q_t* const Y_p);

SPL_Return_t SPL_Mult_vint32Q_FIX(       SPL_FIX_vint32Q_t*  const Product_p,
                                   const SPL_FIX_vint32Q_t*  const X_p,
                                   const SPL_FIX_vint32Q_t*  const Y_p        );

sint16 SPL_normalize_vint16( sint16 * X_p, sint16 L, sint16 desiredHeadroom );

sint16 SPL_normalize_vint32( sint32 * data_p, sint16 L, sint16 desiredHeadroom );

SPL_Return_t SPL_normalize_vint16Q( SPL_FIX_vint16Q_t * X_p );

SPL_Return_t SPL_normalize_vint32Q( SPL_FIX_vint32Q_t * X_p );

SPL_Return_t SPL_normalize_vint16Q_vint32Q( SPL_FIX_vint16Q_t * Output_p,
                                            SPL_FIX_vint32Q_t * Input_p );

SPL_Return_t SPL_Sub_sint16Q_FIX(      SPL_FIX_sint16Q_t*  const Diff_p,
                                 const SPL_FIX_sint16Q_t*  const X_p,
                                 const SPL_FIX_sint16Q_t*  const Y_p);

SPL_Return_t SPL_Sub_vint16Q_FIX(      SPL_FIX_vint16Q_t*  const Diff_p,
                                 const SPL_FIX_vint16Q_t*  const X_p,
                                 const SPL_FIX_vint16Q_t*  const Y_p);

SPL_Return_t SPL_Sub_vint32Q_FIX(      SPL_FIX_vint32Q_t*  const Diff_p,
                                 const SPL_FIX_vint32Q_t*  const X_p,
                                 const SPL_FIX_vint32Q_t*  const Y_p);

SPL_Return_t SPL_Sum_vint16Q_FIX(      SPL_FIX_sint16Q_t* const X_Summed_p,
                                 const SPL_FIX_vint16Q_t* const X_p,
                                 const sint16                   start,
                                 const sint16                   length);

SPL_Return_t SPL_Sum_vint32Q_FIX(      SPL_FIX_sint32Q_t* const X_Summed_p,
                                 const SPL_FIX_vint32Q_t* const X_p,
                                 const sint16                   start,
                                 const sint16                   length);

SPL_Return_t SPL_Vmax_vint16Q_FIX(      SPL_FIX_vint16Q_t*  const Max_p,
                                  const SPL_FIX_vint16Q_t*  const X_p,
                                  const SPL_FIX_vint16Q_t*  const Y_p);

SPL_Return_t SPL_Vmax_vint16Q_FIX(      SPL_FIX_vint16Q_t*  const Max_p,
                                  const SPL_FIX_vint16Q_t*  const X_p,
                                  const SPL_FIX_vint16Q_t*  const Y_p);

SPL_Return_t SPL_Vmax_vint32Q_FIX(      SPL_FIX_vint32Q_t* const Max_p,
                                  const SPL_FIX_vint32Q_t* const X_p,
                                  const SPL_FIX_vint32Q_t* const Y_p);

SPL_Return_t SPL_Vmin_vint16Q_FIX(      SPL_FIX_vint16Q_t*  const Min_p,
                                  const SPL_FIX_vint16Q_t*  const X_p,
                                  const SPL_FIX_vint16Q_t*  const Y_p);

SPL_Return_t SPL_Vmin_vint32Q_FIX(      SPL_FIX_vint32Q_t*  const Min_p,
                                  const SPL_FIX_vint32Q_t*  const X_p,
                                  const SPL_FIX_vint32Q_t*  const Y_p);

void SPL_BitReverseFFT_Data_FIX(       sint16 * const outData_p,
								 const uint32         numBits    );

// returns the change in Q value - add function return value to Q value (for data pointed to by Data_p)
sint16 SPL_PackFFT_Data_FIX(       sint16 *  const Data_p,
							 const sint32          numBits );

SPL_Return_t SPL_InverseFft_vint16Q_FIX( const SPL_FIX_vint16Q_t *  const inData_p,
										       SPL_FIX_vint16Q_t *  const outData_p,
                                         const sint16                     numBits,
                                         const SPL_FFT_Data_t             dataType   );

/* SPL_ComplexIFFT_Core_FIX() returns the change in Q value:
 */
sint16 SPL_ComplexIFFT_Core_FIX(       sint16 * const Data_p,
                                 const sint16         numBits );

// returns the change in Q value - add function return value to Q value (for data pointed to by Data_p)
sint16 SPL_UnPackFFT_Data_FIX(       sint16 * const Data_p,
                               const uint32         numBits );

SPL_Return_t SPL_Fft_vint16Q_FIX( const SPL_FIX_vint16Q_t * const  inData_p,
										SPL_FIX_vint16Q_t * const  outData_p,
                                  const sint16                     numBits,
                                  const SPL_FFT_Data_t             dataType,
                                  const SPL_FFT_Normalize_t        normFlag   );

/* SPL_ComplexFFT_Core_FIX() returns the change in Q value:
 */
sint16 SPL_ComplexFFT_Core_FIX(       sint16 * const Data_p,
                                const uint32         numBits );


SPL_Return_t SPL_BlockSum_vint32Q_FIX(       SPL_FIX_vint32Q_t *  const X_Summed_p,
									   const SPL_FIX_vint32Q_t *  const X_p,
									   const sint16                     start,
									   const sint16                     length,
									   const sint16                     bandwidth    );



#endif /* SPL_FIXED_POINT_FUNCTIONS_H_ */
