/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef INCLUSION_GUARD_VIDEOCODECSYMBOLDECH263_H
#define INCLUSION_GUARD_VIDEOCODECSYMBOLDECH263_H

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
* Name:         mp4d_H263_GetPictureHeader
*
* Parameters:   Instream_p   [IN]
*               LayerData_p  [IN]
*
*
* Returns:      void          [RET]
*
* Description:  This function reads the picture layer header from the
*               bitstream. H263 5.1
*
**************************************************************************/

void mp4d_H263_GetPictureHeader(InstreamBuffer_t* Instream_p,
                          LayerData_t* LayerData_p);

/*************************************************************************
*
* Name:         mp4d_H263_GetMCBPQ
*
* Parameters:   LayerData_p  [IN]
*               MB_Data_p    [IN]
*
*
* Returns:      Nothing
*
* Description:  Read macroblock specific data. H263 5.3
*
**************************************************************************/

void mp4d_H263_GetMCBPQ(LayerData_t* LayerData_p,
                   MB_Data_t* MB_Data_p);

/*************************************************************************
*
* Name:         mp4d_H263_GetSegmentHeader
*
* Parameters:   Instream_p      [IN]
*               LayerData_p     [IN]
*               TempLayerData_p [OUT]
*
* Returns:      void          [RET]
*
* Description:  Read Slice or GOB headers.
*
**************************************************************************/

void mp4d_H263_GetSegmentHeader(InstreamBuffer_t* Instream_p,
                          LayerData_t* LayerData_p);/*,
                          LayerData_t* TempLayerData_p);*/

/*************************************************************************
*
* Name:         mp4d_H263_GetMacroblockCoefficients
*
* Parameters:   LayerData_p     [IN]
*               MB_Data_p       [IN]
*               Coefficients_p  [IN]
*               LastCoeffs[6]   [IN/OUT]  used in pred
*
* Returns:      void
*
* Description:  Gets all TCOEF for a macroblock. The array last_coeffs
*               returns the number of the last coefficient in each block.
*               Except in AIC-mode, the coefficients are also invquantized
*               and zigzag-scanned. H263 5.4
*
**************************************************************************/

void mp4d_H263_GetMacroblockCoefficients(LayerData_t* LayerData_p,
                                    MB_Data_t* MB_Data_p,
                                    sint16* Coefficients_p,
                                    int LastCoeffs[6]);

/*************************************************************************
*
* Name:         mp4d_H263_DecodeIntraDC
*
* Parameters:   Instream_p  [IN]
*
*
* Returns:      dc value
*
* Description:  Reads one IntraDC coefficient from bitstream. H263 5.4.1
*
**************************************************************************/

sint16 mp4d_H263_DecodeIntraDC(InstreamBuffer_t* Instream_p);

/*************************************************************************
*
* Name:         mp4d_H263_DecodeMVD
*
* Parameters:   LayerData_p  [IN]
*               MB_Data_p   [IN/OUT]  motion vectors filled in
*
* Returns:      void
*
* Description:  Read motion-vectors from stream.
*
**************************************************************************/

void mp4d_H263_DecodeMVD(LayerData_t* LayerData_p,
                    MB_Data_t* MB_Data_p, 
                    sint16 MB_Nr);


/*************************************************************************/
#ifdef __cplusplus
}
#endif

#endif //INCLUSION_GUARD_VIDEOCODECSYMBOLDECH263_H
