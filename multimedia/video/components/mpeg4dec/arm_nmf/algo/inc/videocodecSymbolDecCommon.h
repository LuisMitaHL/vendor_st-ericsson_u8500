/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef INCLUSION_GUARD_VIDEOCODECSYMBOLDECH263GENERIC_H
#define INCLUSION_GUARD_VIDEOCODECSYMBOLDECH263GENERIC_H

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
extern const int MVD_Table[]; 

/*************************************************************************
* Declarations of functions
**************************************************************************/

/*************************************************************************
*
* Name:         mp4d_MPEG4_GetSegmentHeader
*
* Parameters:   Instream_p   [IN]
*               LayerData_p  [IN/OUT]
*               LayerData_p  [OUT]
*
* Returns:      void
*
* Description:  Read Video Packet header, including resync marker.
*               MPEG4 6.3.5
*
**************************************************************************/

void mp4d_MPEG4_GetSegmentHeader(InstreamBuffer_t* Instream_p,
                           LayerData_t* LayerData_p);/*,
                           LayerData_t* TempLayerData_p);*/

/*************************************************************************
*
* Name:         mp4d_MPEG4_GetPictureHeader
*
* Parameters:   Instream_p   [IN]
*               LayerData_p  [OUT] decoded header data
*
* Returns:      void
*
* Description:  Decode all type of headers.
*
**************************************************************************/

void mp4d_MPEG4_GetPictureHeader(InstreamBuffer_t* Instream_p,
                           LayerData_t* LayerData_p);



/*************************************************************************
*
* Name:         mp4d_Median
*
* Parameters:   a     [IN]  
*               b     [IN]  
*               c     [IN]  
*               
*
* Returns:			mp4d_Median value
*
* Description:	Find the median of three values.
*
**************************************************************************/

int mp4d_Median(int a, 
           int b, 
           int c);



/*************************************************************************
*
* Name:         mp4d_GetMVD
*
* Parameters:   LayerData_p  [IN]  
*               MB_Data_p   [IN/OUT]  
*
* Returns:			void
*
* Description:	Read motion-vectors from stream.
*
**************************************************************************/

void mp4d_GetMVD(LayerData_t* LayerData_p, 
            MB_Data_t* MB_Data_p, 
            sint16 MB_Nr);


/*************************************************************************
*
* Name:         mp4d_SetSkipMB
*
* Parameters:		MB_Data_p   [IN/OUT]  
*
* Returns:			void
*
* Description:	Sets appropriate data for skipped MB.
*
**************************************************************************/

void mp4d_SetSkipMB(MB_Data_t* MB_Data_p)  __attribute__ ((section ("GetCoeff")));

/*************************************************************************
*
* Name:         mp4d_DecodeMCBPC
*
* Parameters:		LayerData_p [IN]  common data
*               MB_Data_p   [OUT] CodeBlock filled in 
*               MB_Type_p   [OUT]
*
* Returns:			void
*
* Description:	Decodes mb_type and cbpc from bitstream.
*               Sets mb_type = 5 in case of stuffing. H263 5.3.2
*
**************************************************************************/

void mp4d_DecodeMCBPC(LayerData_t* LayerData_p, 
                 MB_Data_t* MB_Data_p, 
                 int* MB_Type_p)     __attribute__ ((section ("GetCoeff")));

/*************************************************************************
*
* Name:         mp4d_DecodeCBPY
*
* Parameters:   LayerData_p  [IN]  
*               MB_Data_p   [OUT] CodeBlock filled in 
*
* Returns:			void
*
* Description:	Decodes CBPY from bitstream.
*
**************************************************************************/

void mp4d_DecodeCBPY(LayerData_t* LayerData_p, 
                MB_Data_t* MB_Data_p)     __attribute__ ((section ("GetCoeff")));


/*************************************************************************/                                  
#ifdef __cplusplus
}
#endif

#endif //INCLUSION_GUARD_VIDEOCODECSYMBOLDECH263GENERIC_H
