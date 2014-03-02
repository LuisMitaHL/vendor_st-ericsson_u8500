/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef INCLUSION_GUARD_VIDEOCODECMPEG4INTRADEC_H
#define INCLUSION_GUARD_VIDEOCODECMPEG4INTRADEC_H

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
* Name:         mp4d_InvProcessIntraMB_MPEG4
*
* Parameters:		LayerData_p   [IN]  
*               MB_Data_p     [IN]  
*               CoeffArray_p  [IN/OUT]  
*               LastCoeffs    [IN]  
*               MB_Nr         [IN]
*
* Returns:			void
*
* Description:	Process an Intra block when decoding. 
*               Computes prediction direction and add the 
*               predicted values to the coefficient array.
*
**************************************************************************/

void mp4d_InvProcessIntraMB_MPEG4(LayerData_t* LayerData_p, 
                             MB_Data_t* MB_Data_p, 
                             sint16* CoeffArray_p, 
                             int LastCoeffs[6],
                             sint16 MB_Nr) __attribute__ ((section ("GetCoeff")));


/*************************************************************************/                                  
#ifdef __cplusplus
}
#endif

#endif //INCLUSION_GUARD_VIDEOCODECMPEG4INTRADEC_H
