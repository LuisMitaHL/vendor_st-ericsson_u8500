/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef INCLUSION_GUARD_VIDEOCODECSYMBOLDECMPEG4_H
#define INCLUSION_GUARD_VIDEOCODECSYMBOLDECMPEG4_H

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
#define READ_DQUANT_LATER 25  // Does not matter as long as > 3

/*************************************************************************
* Declarations of functions
**************************************************************************/

/*************************************************************************
*
* Name:         mp4d_MPEG4_GetMCBPQ
*
* Parameters:   LayerData_p  [IN]
*               MB_Data_p    [OUT] mb coding type
*
* Returns:      void
*
* Description:  Read and sets MB coding type.
*
**************************************************************************/

void mp4d_MPEG4_GetMCBPQ_DP(LayerData_t* LayerData_p,
                    MB_Data_t* MB_Data_p);
                    
void mp4d_MPEG4_GetMCBPQ(LayerData_t* LayerData_p,
                    MB_Data_t* MB_Data_p) __attribute__ ((section ("GetCoeff")));

/*************************************************************************
*
* Name:         mp4d_MPEG4_DecodeMVD
*
* Parameters:   LayerData_p  [IN]
*               MB_Data_p    [IN/OUT]
*
* Returns:      void
*
* Description:  Read and decode motion vectors.
*
**************************************************************************/

void mp4d_MPEG4_DecodeMVD(LayerData_t* LayerData_p,
                     MB_Data_t* MB_Data_p,
                     sint16 MB_Nr) __attribute__ ((section ("GetCoeff")));

/*************************************************************************
*
* Name:         MPEG4_GetMODB
*
* Parameters:   LayerData_p   [IN]
*               MB_Data_p  [IN]
*
*
* Returns:             [RET]
*
* Description:  Retrieves MODB, MB_Type, CPBP from the bitstream. Used in B-VOP. 
*
**************************************************************************/
void mp4d_MPEG4_B_GetMODB(LayerData_t* LayerData_p, MB_Data_t* MB_Data_p);

/*************************************************************************
*
* Name:         mp4d_MPEG4_B_ComputeMV_DirectMode
*
* Parameters:   LayerData_p   [IN]
*               MB_Data_p     [IN]
*               MB_Nr         [IN]
*
* Returns:      void          [RET]
*
* Description:  See 7.6.9.5.2 
*
**************************************************************************/
void mp4d_MPEG4_B_ComputeMV_DirectMode(LayerData_t* LayerData_p, MB_Data_t* MB_Data_p, int MB_Nr);


/*************************************************************************/
#ifdef __cplusplus
}
#endif

#endif //INCLUSION_GUARD_VIDEOCODECSYMBOLDECMPEG4_H
