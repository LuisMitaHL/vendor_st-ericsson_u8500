/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef INCLUSION_GUARD_VIDEOCODECDECODECOEFF_H
#define INCLUSION_GUARD_VIDEOCODECDECODECOEFF_H

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************************
* Includes
**************************************************************************/
#include "videocodecDecInternal.h"

/*************************************************************************
* Types, constants and external variables
**************************************************************************/
#define RUN_SHIFT      0    // bit 0-5
#define LAST_SHIFT     6    // bit 6
#define SIGN_SHIFT     7    // bit 7
#define LEN_SHIFT      8    // bit 8-11
#define INDEX_SHIFT   12    // bit 12-18
#define LEVEL_SHIFT   20    // bit 20-31

#define RUN_MASK   (63UL << RUN_SHIFT)     
#define LAST_MASK  (1UL << LAST_SHIFT)  
#define SIGN_MASK  (1UL << SIGN_SHIFT)    
#define LEN_MASK   (15UL << LEN_SHIFT)
#define INDEX_MASK (127UL << INDEX_SHIFT)
#define LEVEL_MASK (4095UL << LEVEL_SHIFT)

#define SET_32_ITEMS_TO_ZERO(Array) \
  do { \
  Array[0] = 0; \
  Array[1] = 0; \
  Array[2] = 0; \
  Array[3] = 0; \
  Array[4] = 0; \
  Array[5] = 0; \
  Array[6] = 0; \
  Array[7] = 0; \
  Array[8] = 0; \
  Array[9] = 0; \
  Array[10] = 0; \
  Array[11] = 0; \
  Array[12] = 0; \
  Array[13] = 0; \
  Array[14] = 0; \
  Array[15] = 0; \
  Array[16] = 0; \
  Array[17] = 0; \
  Array[18] = 0; \
  Array[19] = 0; \
  Array[20] = 0; \
  Array[21] = 0; \
  Array[22] = 0; \
  Array[23] = 0; \
  Array[24] = 0; \
  Array[25] = 0; \
  Array[26] = 0; \
  Array[27] = 0; \
  Array[28] = 0; \
  Array[29] = 0; \
  Array[30] = 0; \
  Array[31] = 0; \
  } while (0)


#define H263_NormalTable MP4_InterCoeffTable

extern const sint32* MP4_IntraCoeffTable[];
extern const sint32* MP4_InterCoeffTable[];
extern const sint32* H263_AdvIntraTable[];

extern const boolean IntraDC_VLC_Table[8][32]; 
/*************************************************************************
* Declarations of functions
**************************************************************************/

/*************************************************************************
*
* Name:         mp4d_DecodeCoeff
*
* Parameters:   LayerData_p     [IN]  
*               CoeffTable_p[]  [IN]  
*               
*
* Returns:      sint32          [RET] 
*
* Description:  Decodes a VLC coefficient
*               MPEG4 & H.263 general
*
**************************************************************************/

sint32 mp4d_DecodeCoeff(LayerData_t* LayerData_p, 
                         const sint32* CoeffTable_p[]) __attribute__ ((section ("GetCoeff")));

/*************************************************************************
*
* Name:         mp4d_MPEG4_GetMacroblockCoefficients
*
* Parameters:   LayerData_p     [IN]  common data
*               MB_Data_p       [IN]  current mb status
*               Coefficients_p  [OUT] 6*64 coeffs
*               LastCoeffs      [OUT] for better speed in IDCT
*
* Returns:      void
*
* Description:  Gets all TCOEF for a macroblock. The array last_coeffs
*               returns the number of the last coefficient in each block.
*               Except in AIC-mode, the coefficients are also invquantized
*               and zigzag-scanned.
*
**************************************************************************/

void mp4d_MPEG4_GetMacroblockCoefficients(LayerData_t* LayerData_p,
                                     MB_Data_t* MB_Data_p,
                                     sint16* Coefficients_p,
                                     int LastCoeffs[6])     __attribute__ ((section ("GetCoeff")));  
/*************************************************************************
*
* Name:         mp4d_MPEG4_DecodeIntraDC
*
* Parameters:   Instream_p  [IN]
*               LuminaceDC  [IN]
*
*
* Returns:      DC value
*
* Description:  Read and decode DC value for intra prediction.
*
**************************************************************************/

sint16 mp4d_MPEG4_DecodeIntraDC(InstreamBuffer_t* Instream_p,
                           boolean LuminaceDC)     __attribute__ ((section ("GetCoeff")));

/*************************************************************************/                                  
#ifdef __cplusplus
}
#endif

#endif //INCLUSION_GUARD_VIDEOCODECDECODECOEFF_H
