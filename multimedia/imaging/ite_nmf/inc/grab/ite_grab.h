/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

 
#ifndef __INC_ITE_GRAB_H
#define __INC_ITE_GRAB_H


//#include <inc/type.h>
#include "ite_sensorinfo.h"
#include "ite_pageelements.h"

#define ENDOFPREGRABHQ 2
#define ENDOFPOSTGRABHQ 5
#define GRABHQMASK 0x07
#define BMSTSTMODEBIT 0x08

#ifdef __cplusplus
extern "C"
{
#endif
/*--------------------------------------------------------------------------*
 * Functions                                                                *
 *--------------------------------------------------------------------------*/
t_uint32 ITE_StartGrabNb(int Pipe, int StreamLength);
t_uint32 ITE_StopGrab(int Pipe);
t_uint32 ITE_AbortGrab(int Pipe);

t_uint32 ITE_IsGrabRunning(int Pipe);

void ITE_setGrabParams( struct s_grabParams *pGrabparams, 
                        enum e_grabFormat grabFormat, 
                        int xframeSize, 
                        int yframeSize, 
                        int xwindowSize,
                        int xwindowOffset,
                        int destBufferAddr, 
                        t_bool disableGrabCache, 
                        int cacheBufferAddr);

void ITE_ModifyGrabTask(struct s_grabParams *pGrabparams,int,int,int,int,int,int,int,int);  
void ITE_ModifyGrabTaskOutputFormat(struct s_grabParams *pGrabparams,int NbOfTasks, int pipeNumber, enum e_grabFormat outputFormat);

#ifdef __cplusplus
}
#endif

#endif /* __INC_ITE_GRAB_H */
