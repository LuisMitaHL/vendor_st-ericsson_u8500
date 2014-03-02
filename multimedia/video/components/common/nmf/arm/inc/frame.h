/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include "types.h"
#include <frameinfo.idt>

#ifndef HOST_FRAME_H
#define HOST_FRAME_H

/**
 * \brief Structure containing frame (dpb + display queue) buffer information
 */

typedef struct
{
    t_sint16    wait_tobe_free;
    t_uint16    is_available;
    t_img_pel   ptr[3];         // Luma, Cb, Cr
    t_uint32    nFlags;
    t_frameinfo frameinfo;      // frame info outputted with output_picture
} t_frames;

#endif //HOST_FRAME_H
