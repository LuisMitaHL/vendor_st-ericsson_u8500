/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _VC1_HT_MEMORY_H_
#define _VC1_HT_MEMORY_H_

/*------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------*/
#include "vc1d_common.h"
#include "vc1d_host.h"

/*------------------------------------------------------------------------
 * Functions                                                            
 *----------------------------------------------------------------------*/

void mem_allocate_picture_buffers(t_uint16 max_width,
                                    t_uint16 max_height,
                                    tps_picture p_ref1,
                                    tps_picture p_ref2,
                                    tps_picture p_ref3);


void mem_copy_reference_and_reset_mv_history(tps_picture p_dest,
                                             tps_picture p_src);

t_uint32 *mem_allocate_mv_history_list(t_uint16 max_width,
                                       t_uint16 max_height );

void mem_reset_mv_history_list_host(t_uint16 max_width, 
                               t_uint16 max_height, 
                               t_uint32 *mv_history_list);


void mem_allocate_postprocessing_buffer(tps_rec_param p_rec_param,
                                        tps_picture_buffer p_upsampling_buffer_h,
                                        tps_picture_buffer p_upsampling_buffer_v,
                                        tps_picture_buffer p_range_reduction_buffer);

void mem_free_postprocessing_buffer(tps_picture_buffer p_upsampling_buffer_h,
                                    tps_picture_buffer p_upsampling_buffer_v,
                                    tps_picture_buffer p_range_reduction_buffer);
void mem_free_picture_buffers(tps_picture p_pic1,
                              tps_picture p_pic2,
                              tps_picture p_pic3);

void mem_free_picture_buffers(tps_picture p_pic1,
                              tps_picture p_pic2,
                              tps_picture p_pic3);

#endif /* _VC1_HT_MEMORY_H_ */
