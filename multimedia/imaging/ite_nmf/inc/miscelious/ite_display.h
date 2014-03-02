/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef __INC_ITE_DISPLAY_H
#define __INC_ITE_DISPLAY_H

//For Linux
#include <inc/type.h>
#include <stdlib.h>
#include "mcde_ioctls.h"
//#include "b2r2_driver_usr_api.h"
//The following files will not be available in Linux build
//We have to find a solution to replace it!!
//#include "mmte_mcde_driver_8500.h"
//#include "mmte_b2r2_driver_8500.h"


#include "ite_sia_buffer.h"
#include "ite_main.h"
#if !(defined(__PEPS8500_SIA) || defined(_SVP_))

#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN))
//The display is not working for Linux build at the moment

ts_b2r2_config Grb_B2R2_node1;
ts_b2r2_config Grb_B2R2_node2;
extern int VP_outXsize,VP_outYsize;
#endif

#ifdef __cplusplus
extern "C"
{
#endif

void ITE_configureMcde(tps_siapicturebuffer p_siapictbuffer);
 void ITE_StopMcde(void);
void ITE_InitFrameBuffer(tps_siapicturebuffer p_siapictbuffer);
#if !(defined(__PEPS8500_SIA) || defined(_SVP_))
//void ITE_InitB2R2(tps__b2r2_config node,tps_siapicturebuffer p_inputbuffer,tps_siapicturebuffer p_outputbuffer);
#endif
t_uint8 ITE_StoreinLCDBuffer(ts_siapicturebuffer* p_Buffer);

#endif
#ifdef __cplusplus
}
#endif

#ifdef __ARM_LINUX
//extern struct mcde_source_buffer;//__NO_WARNING__
void ITE_Configure_FrameBuffer(struct mcde_source_buffer* fb_buffer, int fbfd);
void ITE_Refresh_FrameBuffer(void);
void ITE_Configure_B2R2(tps_siapicturebuffer p_Buffer, unsigned int * fb_address);
#endif


#endif /* __INC_ITE_DISPLAY_H */
