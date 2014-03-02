/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _VC1D_HOST_H_
#define _VC1D_HOST_H_

/*------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------*/
#include "vc1d_common.h"

/*------------------------------------------------------------------------
 * Types							       
 *----------------------------------------------------------------------*/

typedef struct
{
    t_bool frame_interpolation_hint_enabled;
    t_uint8 b_fraction_numerator;
    t_uint8 b_fraction_denominator;
    t_uint8 buffer_fullness;
    t_vc1_picture_resolution picture_res;
    t_uint16 max_picture_height;
    t_uint16 max_picture_width;
    t_uint16 picture_width;     /**< \brief size of the picture wrt resolution   */
    t_uint16 picture_height;    /**< \brief size of the picture wrt resolution   */
    t_uint32 time_stamp;
    t_vc1_picture_type picture_type;
    t_bool range_reduction_frame_enabled; /**< \brief RANGEREDFRM field of picture   */
    t_uint32 frame_num; /**< \brief to ease debugging   */
} ts_post_proc_param,*tps_post_proc_param;

typedef struct
{
    ts_post_proc_param post_proc_param;
    ts_picture_buffer  pict_buff;
    /* Only used for SVA programming */
    t_bool intensity_compensate_enabled;
    t_uint16 luma_scale;
    t_uint16 luma_shift;
//    t_uint32 *mv_history_list;
} ts_picture, *tps_picture;

#endif /* _VC1D_HOST_H_ */
