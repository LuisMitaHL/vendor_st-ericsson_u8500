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

#ifndef _REC_API_H_
#define _REC_API_H_

#include "AIQ_CommonTypes.h"
#include "OMX_Core.h"
#include "IFM_Types.h"
#include "RED_Api.h"

// Set to one to initialise omx Buffer sharing
#define OMX_BUFFER_SHARING 1

/* opaque type to store context */
typedef void* RECHandler_t;

/* RED tuning parameter structure */
typedef struct {
   int tmp0;
   int tmp1;
   int tmp2;
   int tmp3;
} REC_Threshold;

/*typedef enum
{
   RER_OFF  = 0,
   RER_FULL = 1,
   RER_FAST = 2,
   RER_SNAP = 3,
} RERControlMode;*/

/* external API */

AIQ_Error RECInit          (RECHandler_t *Hdl);

AIQ_Error RECSetTuningParams  (RECHandler_t Hdl,
                        REC_Threshold *p_TuningParams);
                      
/*AIQ_Error RECSetControlMode(RECHandler_t Hdl,
                      RERControlMode controlMode);*/
                      
AIQ_Error RECProcessing    (RECHandler_t Hdl, 
                      AIQ_Frame *p_InOutput,
                      RER_metadata *metadata);
                      
AIQ_Error RECDeInit        (RECHandler_t Hdl);

#endif // _REC_API_H_
