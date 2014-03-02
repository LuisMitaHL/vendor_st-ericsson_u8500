/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef INCLUSION_GUARD_VIDEOCODECGMC_H
#define INCLUSION_GUARD_VIDEOCODECGMC_H

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************************
* Includes
**************************************************************************/

#include "videocodecDecInternal.h"
#include "videocodecInstreamBuffer.h"

/*************************************************************************
* Types, constants and external variables
**************************************************************************/
#define ROUND(x) ((x >=0) ? (x) + 0.5 : (x) -0.5 )
#define DIV_2(x,y) (int)((double)(ROUND((double)x/(double)y)))

/*************************************************************************
* Declarations of functions
**************************************************************************/

void mp4d_calculate_sprite_ref_points(LayerData_t* LayerData_p);
void mp4d_sprite_trajectory(InstreamBuffer_t* Instream_p, LayerData_t* LayerData_p);

//void (*gmc_sample_reconstruction)(LayerData_t * , sint16);
void mp4d_gmc_sample_reconstruction_0(LayerData_t* LayerData_p, sint16 MB_Number);
void mp4d_gmc_sample_reconstruction_1(LayerData_t* LayerData_p, sint16 MB_Number);
//void mp4d_gmc_sample_reconstruction_2_3(LayerData_t* LayerData_p, sint16 MB_Number);

#ifdef __cplusplus
}
#endif

#endif //INCLUSION_GUARD_VIDEOCODECGMC_H

