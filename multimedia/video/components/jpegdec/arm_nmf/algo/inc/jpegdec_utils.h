/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \file 	jpegdec_utils.h
 * \brief 	image write functions
 * \author  ST-Ericsson
 *
 */
/*****************************************************************************/
#ifndef _UTILS_H
#define _UTILS_H

//#include "djpeg_hv_huffman.h" /* for functions declarations */
#include "djpeg_hamac.h"
#include "djpeg_hv_postprocessing.h"




//void (*jpegdec_ptr2image)(t_uint8 [][], t_uint16 , tps_image_info , t_uint32 , t_uint32 , t_uint32 , t_component_id ) = NULL;

void   image_init(tps_hamac_info p_hamac_info, tps_image_info p_image_info,tps_post_process_info p_postproc_info, tps_t1xhv_vdc_jpeg_global jpeg_global);

void    write_image_to_buffer_any(t_uint8 *idct_array, t_uint16 block_index, tps_image_info p_image_info, t_uint32 mcu_counter_in_row, t_uint32 mcu_row, t_component_id comp_id );

void    write_image_to_buffer_any_last(t_uint8 *idct_array, t_uint16 block_index, tps_image_info p_image_info, T_BOOL is_last_row,T_BOOL is_last_col, t_component_id comp_id );


#endif
