/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef INCLUSION_GUARD_VIDEOCODECDECAPI_H
#define INCLUSION_GUARD_VIDEOCODECDECAPI_H

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
* Name:         mp4d_VideoDecInitDecoder
*
* Parameters:		Standard    [IN] Which decoder
*               Width       [IN] 
*               Height      [IN]
*               Level       [IN] Which level
*               InputType   [IN] File/PS/CS
*
* Returns:			Handle to the decoder
*
* Description:	This function allocates memory for internal data 
*               structures and initializes the decoder with default 
*               decoder parameters.
*
**************************************************************************/
void mp4d_VideoDecInitDecoder(LayerData_t *           LayerData_p);


/*************************************************************************
*
* Name:         mp4d_VideoDecReleaseDecoder
*
* Parameters:   decoder  [IN]  
*               
*
* Returns:      int      [RET] 
*
* Description:	Frees the resources allocated by the Decoder 
*
**************************************************************************/
int mp4d_VideoDecReleaseDecoder(LayerData_t *           LayerData_p); //@AKC Api Change

/*************************************************************************
*
* Name:         mp4d_VideoDecDecodePicture
*
* Parameters:   decoder  [IN]  
*               
*
* Returns:      int      [RET] 
*
* Description:	Find the next valid picture and decode it
*
**************************************************************************/
int mp4d_VideoDecDecodePictureAlgo(LayerData_t* LayerData_p, uint32* ConsumedBytes_p);

 void mp4d_PictureDecoded(LayerData_t* LayerData_p);

 void mp4d_CorrectVOL_Header(/*SessionData_t* SessionData_p,*/
                              LayerData_t*   LayerData_p/*,
                              PictureData_t* PictureData_p*/);

 void mp4d_CorrectPictureHeader(/*SessionData_t* SessionData_p,*/
                                 LayerData_t*   LayerData_p/*,
                                 LayerData_t*   TempLayerData_p,
                                 PictureData_t* TempPictureData_p*/);

 void mp4d_ResetBeforePicture(LayerData_t* LayerData_p,t_uint16);

sint16 mp4d_MPEG4_GetLastMB_Number(InstreamBuffer_t* Instream_p, LayerData_t* LayerData_p);

void StartupInstream(LayerData_t *           LayerData_p);



#ifdef __cplusplus
}
#endif

#endif //INCLUSION_GUARD_VIDEOCODECDECAPI_H
