/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __INC_HOST_VALIDATION_IO_H
#define __INC_HOST_VALIDATION_IO_H

#ifdef __cplusplus
extern "C"
{
#endif
#include "types.h"

/*--------------------------------------------------------------------------*
 * Defines                                                                  *
 *--------------------------------------------------------------------------*/
/* Values for output_digests_flag variable */
#define OUTPUT_YUV_ONLY 0
#define OUTPUT_DIGESTS_ONLY 1


/*--------------------------------------------------------------------------*
 * Functions                                                                *
 *--------------------------------------------------------------------------*/
void ComputeYUVFrameDigests( t_uint32  a_yuv_frame_width_in_byte  ,
                             t_uint32  a_yuv_frame_height_in_byte ,
                             t_uint8  *ap_yuv420pl_frame_Y_buffer ,
                             t_uint8  *ap_yuv420pl_frame_U_buffer ,
                             t_uint8  *ap_yuv420pl_frame_V_buffer );

void ComputeYUVFrameDigestsForFrameCropping( t_uint32 a_yuv_frame_width_in_byte  ,
                                             t_uint32 a_yuv_frame_height_in_byte );
void close_file(char *outname);

#ifdef __cplusplus
}
#endif
#endif /* __INC_HOST_VALIDATION_IO_H */
