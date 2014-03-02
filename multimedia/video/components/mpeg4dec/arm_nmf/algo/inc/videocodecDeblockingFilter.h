/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef INCLUSION_GUARD_VIDEOCODECDEBLOCKINGFILTER_H
#define INCLUSION_GUARD_VIDEOCODECDEBLOCKINGFILTER_H

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
* Name:         mp4d_DeblockingFilter
*
* Parameters:   Image_p      [OUT]  
*               MB_Data_p    [IN]  
*               LayerData_p  [IN]  
*               
*
* Returns:      Nothing
*
* Description:  Deblocking filter Annex J, H.263
*               Block edges are smoothed
*
**************************************************************************/

void mp4d_DeblockingFilter(YUV_Image_t* Image_p, 
                      MB_Data_t* MB_Data_p,	
                      LayerData_t* LayerData_p);

/*************************************************************************/

#ifdef __cplusplus
}
#endif

#endif //INCLUSION_GUARD_VIDEOCODECDEBLOCKINGFILTER_H
