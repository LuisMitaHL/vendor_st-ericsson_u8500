/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef INCLUSION_GUARD_VIDEOCODECAICMODEDEC_H
#define INCLUSION_GUARD_VIDEOCODECAICMODEDEC_H

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
* Name:         mp4d_InvProcessAIC_MB
*
* Parameters:   LayerData_p    [IN]  
*               MB_Data_p      [IN/OUT]  
*               DCT_Array1_p   [IN]  
*               LastCoeffs[6]  [IN]  
*
* Returns:      Nothing
*
* Description:	Unpack, inv quantize AIC coefficients for a MB.
*               H263 Annex I
*
**************************************************************************/

void mp4d_InvProcessAIC_MB(LayerData_t* LayerData_p, 
                      MB_Data_t* MB_Data_p, 
                      sint16* DCT_Array1_p, 
                      int LastCoeffs[6]);

/*************************************************************************/
#ifdef __cplusplus
}
#endif

#endif //INCLUSION_GUARD_VIDEOCODECAICMODEDEC_H
