/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef ITE_PAGEELEMENTS_H_
#define ITE_PAGEELEMENTS_H_

//#include "share/inc/type.h"
#include "VhcElementDefs.h"
#include "ite_vpip.h"
#include "ite_pipeinfo.h"

#include "ite_sia_interface_data.h"

#include <cm/inc/cm_macros.h>
#include "grab_types.idt.h"

#define ADDRI2CSTM 0x20

//extern struct s_page_element PE_ViewfinderS861[];
extern t_uint16 patchS861[][2];


#ifdef __cplusplus
extern "C"
{
#endif

/* for backwards compatibility */
int ITE_ISP_Run(enum e_grabPipeID pipe,    // PIPE_LR/PIPE_HR
                OutputFormat_te format, // PipeOutputFormat_RGB565 PipeOutputFormat_YUV422 ....
                t_uint32 OutputSizeX, 	// Pipe Output Size X
                t_uint32 OutputSizeY,   // Pipe Output Size Y
                t_uint32 StreamLength,  // 0 for Infinity, n for n frames
		t_uint32 FrameRateMode, // FrameRateMode_Manual / FrameRateMode_Auto
		t_uint32 fps);		// for manual mode: desired frame rate


#ifdef __cplusplus
}
#endif

#endif /*ITE_PAGEELEMENTS_H_*/
