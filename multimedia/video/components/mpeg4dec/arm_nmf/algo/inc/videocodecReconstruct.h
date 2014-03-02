/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef INCLUSION_GUARD_VIDEOCODECRECONSTRUCT_H
#define INCLUSION_GUARD_VIDEOCODECRECONSTRUCT_H

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

/*************************************************************************
* Declarations of functions
**************************************************************************/

/*************************************************************************
*
* Name:         mp4d_Reconstruct
*
* Parameters:		LayerData_p     [IN]  Layer data
*               MB_Data_p       [IN]  The data of the current macroblock.
*               QuantArray_p    [IN]  6*64 pixels. To add with predicted data
*               CurrRecImage_p  [OUT] current reconstructed image
*               MB_Nr           [IN]
*
* Returns:			void
*
* Description:	Perform inverse dct and add with predicted pixelvalues.
*               Move pixels in array to CurrRecImage_p.
*
**************************************************************************/

void mp4d_Reconstruct(LayerData_t* LayerData_p, 
                 MB_Data_t* MB_Data_p, 
                 sint16* QuantArray_p, 
                 YUV_Image_t* CurrRecImage,
                 sint16 MB_Nr) __attribute__ ((section ("Reconstruct")));

/*************************************************************************/     
#ifdef __cplusplus
}
#endif

#endif //INCLUSION_GUARD_VIDEOCODECRECONSTRUCT_H

