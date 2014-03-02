/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*
* Author:            Olivier Pothier <olivier.pothier@stericsson.com>
* Filename:          face_detector.h
* Modification date: 
* Release:
* Description:       
*
*****************************************************************************/

#ifndef _FACE_DETECTOR_H_
#define _FACE_DETECTOR_H_

#include "AIQ_CommonTypes.h" /* AIQ_CommonTypes.h should be extended to define AIQ_Faces type */

#define AIQ_FD_ID 0x123
#define AIQ_DS_ID 0x124

#ifdef FD_EXTENDED_CFG
/* algo variants type */
#define DEFAULT_ALGO 0
#define DEFAULT_PERIOD 0 
#define FULL_SEARCH_ONLY 0x000001 /* always full search  */
#define VARIANT1 0x000002 /* 0.9 */
#define VARIANT2 0x000004 /* not defined yet emulate skip mechanism ? */
#define VARIANT3 0x000008 /* not defined yet */
#endif /* FD_EXTENDED_CFG */

#define MAX_NUMBER_OF_DETECTED_FACES 10

typedef struct
{
    AIQ_U16	faceTopLeft_x;
    AIQ_U16	faceTopLeft_y;
    AIQ_U16	faceWidth;
    AIQ_U16	faceHeight;
    AIQ_U32	facePriority;
    AIQ_U32	faceID;    
} AIQ_faceDescriptor;

typedef struct
{
AIQ_U8 nFaces; 
AIQ_faceDescriptor faceDesc[MAX_NUMBER_OF_DETECTED_FACES];
} AIQ_Faces;


/* opaque type to store context                            */
/* this context shal embed all STI-lib related parameters  */
typedef void* FaceDetectorHandler_t;

/* external API */
#ifdef FD_EXTENDED_CFG
AIQ_Error FaceDetectorInit(FaceDetectorHandler_t *Hdl,
                           AIQ_U32 u32_width, 
                           AIQ_U32 u32_height, 
                           AIQ_U32 u32_stride,
                           AIQ_U32 u32_algo_type, 
                           AIQ_U32 u32_period
                           );
#else
AIQ_Error FaceDetectorInit(FaceDetectorHandler_t *Hdl,AIQ_U32 u32_width, AIQ_U32 u32_height, AIQ_U32 u32_stride);
#endif
AIQ_Error FaceDetect          (FaceDetectorHandler_t Hdl,AIQ_Frame *p_Input);
AIQ_Error FaceDetectorGetFaces(FaceDetectorHandler_t Hdl,AIQ_Faces *p_Output);
AIQ_Error FaceDetectorDeInit  (FaceDetectorHandler_t Hdl);
//AIQ_Error FaceDetectorTestConversionFaces(FaceDetectorHandler_t Hdl,
//										  /*AIQ_Faces *faces ,*/
//                                          OMX_SYMBIANDRAFT_CONFIG_ROITYPE *pOutput);

#endif // _NORCOS_H_
