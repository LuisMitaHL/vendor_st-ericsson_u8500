/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \file 	djpeg_ht_memory.h
 * \brief 	host memory allocation functions declaration
 * \author  ST-Ericsson
 *  
 *  This file declares the  hamac main entry point function 
 */
/*****************************************************************************/


#ifndef _DJPEG_HT_MEMORY_H_
#define _DJPEG_HT_MEMORY_H_

/*------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------*/
#include "djpeg_host.h"  

/*------------------------------------------------------------------------
 * Defines							       
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Types
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Functions
 *----------------------------------------------------------------------*/


t_sint32 mem_alloc_reconstructed_frame(tps_reconstructed_frame p_recon_frame, 
                                     t_uint16 nb_lines, 
                                     t_uint16 nb_sample_per_line, 
                                     t_uint16 downsampling_factor,
                                     t_uint16 output_format);
t_sint32 mem_alloc_mcu_line_buffer(tps_mcu_line_buffer p_line_buffer, 
                                 t_uint16 nb_sample_per_line, 
                                 t_uint16 downsampling_factor);

t_sint32 mem_alloc_coeff_buffer(  tps_coeff_buffer  p_coeff_buffer, 
                                tps_jpg_frame p_frame);
void  mem_reset_reconstructed_frame(tps_reconstructed_frame p_recon_frame);

#endif /* _DJPEG_HT_MEMORY_H_ */
