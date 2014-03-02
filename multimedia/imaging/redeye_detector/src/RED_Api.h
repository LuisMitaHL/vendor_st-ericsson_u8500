/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*
* Author:            Olivier Pothier <olivier.pothier@stericsson.com>
* Filename:          REC_Api.h
* Modification date: 
* Release:
* Description:       
*
*****************************************************************************/
#ifdef _MSC_VER
#pragma once
#endif

#ifndef _RED_API_H_
#define _RED_API_H_

#include "AIQ_CommonTypes.h"
#include "OMX_Core.h"
#include "IFM_Types.h"
#include "wrapper_openmax_tools.h"

//*********************************************************************
//    Structs for Red Eye
//*********************************************************************
#define MAX_NUMBER_OF_EYES_TO_CORRECT 20 /* number to define with you */

enum eyeType
{
	redEye=0,
	goldenEye=1,
};

typedef struct
{
	int min_light;
	int max_x;
	int max_y;
	int min_x;
	int min_y;
	eyeType type;
	void Clear() { min_light = 0; max_x=0; max_y=0; min_x=0; min_y=0; type=redEye;}
}detectedEye_data;

typedef struct
{
	detectedEye_data detectedEyes[MAX_NUMBER_OF_EYES_TO_CORRECT];
	int detectedEyes_number;
	int detectionImage_width; /* width of the downsampled image on which the detection is made (see note below) */
} RER_metadata;

//*********************************************************************
//    APIs for Red Eye Detection
//*********************************************************************
/* opaque type to store context */
typedef void* REDHandler_t;

/* RED tuning parameter structure */
typedef struct {
   int ThresholdBitClassRed;
   int ThresholdBitClassGolden;
   } RED_Threshold;

typedef enum
{
   RER_OFF  = 0,
   RER_FULL = 1,
   RER_FAST = 2,
   RER_SNAP = 3,
} RERControlMode;

typedef enum
{
   RER_REDANDGOLDEN  = 0,
   RER_REDONLY = 1,   
} RERColorDefect;

/* external API */

AIQ_Error REDInit    (REDHandler_t *Hdl,
                      AIQ_U32 u32_width,
                      AIQ_U32 u32_height,
                      AIQ_U32 u32_stride);
                      
AIQ_Error REDSetTuningParams  (REDHandler_t Hdl,
                        RED_Threshold *p_TuningParams);
                      
AIQ_Error REDSetControlMode(REDHandler_t Hdl,
                      RERControlMode controlMode);
                      
AIQ_Error REDSetColorDefect(REDHandler_t Hdl,
                      RERColorDefect colorDefect);

AIQ_Error REDProcessing    (REDHandler_t Hdl, 
                      AIQ_Frame *p_Input,
                      RER_metadata *metadata);

AIQ_Error REDDeInit        (REDHandler_t Hdl);

#endif // _RED_API_H_
