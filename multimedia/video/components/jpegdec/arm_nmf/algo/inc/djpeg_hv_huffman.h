/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
  * \file 	djpeg_hv_huffman.h
 * \brief    huffman relative functions declarations
 * \author  ST-Ericsson
 *  This file declares function relatives to huffman decoding
 */
/*****************************************************************************/


#ifndef _DJPEG_HV_HUFFMAN_H_
#define _DJPEG_HV_HUFFMAN_H_

/*------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------*/



#include "djpeg_hamac.h"
#include "djpeg_co_bitstream.h"  /* for tps_bitstream_buffer def */
#include "djpeg_bitstream_lib.h"
#include "djpeg_hv_ace_preprocessing.h"
#include "djpeg_hv_idct.h"
#include "djpeg_hv_postprocessing.h"
#include "jpegdec_utils.h"

#ifdef __NMF
#include <t1xhv_vdc_jpeg.idt>
#include <jpegdec.idt>
#else
#include "djpeg_interface.h"
#include "djpeg_global.h"
#endif

/*------------------------------------------------------------------------
 * Defines							       
 *----------------------------------------------------------------------*/




#if 0
#define HUFF_EXTEND(x,s)  ((x) < (1<<((s)-1)) ? (x) + (((-1)<<(s)) + 1) : (x))
#endif
#if 1
#define HUFF_EXTEND(x,s)  ((x) < extend_test[s] ? (x) + extend_offset[s] : (x))

static const t_sint16 extend_test[16] =   /* entry n is 2**(n-1) */
  { 0, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080,
    0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000 };

static const t_sint16 extend_offset[16] = /* entry n is (-1 << n) + 1 */
  { 0, ((-1)<<1) + 1, ((-1)<<2) + 1, ((-1)<<3) + 1, ((-1)<<4) + 1,
    ((-1)<<5) + 1, ((-1)<<6) + 1, ((-1)<<7) + 1, ((-1)<<8) + 1,
    ((-1)<<9) + 1, ((-1)<<10) + 1, ((-1)<<11) + 1, ((-1)<<12) + 1,
    ((-1)<<13) + 1, ((-1)<<14) + 1, ((-1)<<15) + 1 };
#endif

void jpegdec_huff_seq_init(tps_t1xhv_vdc_jpeg_session djpeg_session);

t_djpeg_error_code jpegdec_huff_seq_decode_mcu (tps_bitstream_buffer_opt p_bitstream, 
                                        tps_mcu p_mcu, 
                                        tps_post_process_info p_pp_info,
                                        tps_huff_seq_info p_huff_info,
                                        tps_ace_info p_ace_info,
                                        tps_hamac_info p_hamac_info,
                                        tps_image_info p_image_info ,
                                       void * param);

t_djpeg_error_code jpegdec_huff_seq_decode_mcu_420 (tps_bitstream_buffer_opt p_bitstream, 
                                        tps_mcu p_mcu, 
                                        tps_post_process_info p_pp_info,
                                        tps_huff_seq_info p_huff_info,
                                        tps_ace_info p_ace_info,
                                        tps_hamac_info p_hamac_info,
                                        tps_image_info p_image_info,
                                        void * param);

t_djpeg_error_code jpegdec_huff_seq_decode_mcu_422(tps_bitstream_buffer_opt p_bitstream, 
                                        tps_mcu p_mcu, 
                                        tps_post_process_info p_pp_info,
                                        tps_huff_seq_info p_huff_info,
                                        tps_ace_info p_ace_info,
                                        tps_hamac_info p_hamac_info,
                                        tps_image_info p_image_info,
                                       void * param);

t_djpeg_error_code jpegdec_huff_seq_decode_mcu_444 (tps_bitstream_buffer_opt p_bitstream, 
                                        tps_mcu p_mcu, 
                                        tps_post_process_info p_pp_info,
                                        tps_huff_seq_info p_huff_info,
                                        tps_ace_info p_ace_info,
                                        tps_hamac_info p_hamac_info,
                                        tps_image_info p_image_info,
                                        void * param);
t_djpeg_error_code jpegdec_huff_seq_decode_mcu_dn (tps_bitstream_buffer_opt p_bitstream, 
                                        tps_mcu p_mcu, 
                                        tps_post_process_info p_pp_info,
                                        tps_huff_seq_info p_huff_info,
                                        tps_ace_info p_ace_info,
                                        tps_hamac_info p_hamac_info,
                                        tps_image_info p_image_info ,
                                       void * param);

t_djpeg_error_code jpegdec_huff_seq_decode_mcu_420_dn (tps_bitstream_buffer_opt p_bitstream, 
                                        tps_mcu p_mcu, 
                                        tps_post_process_info p_pp_info,
                                        tps_huff_seq_info p_huff_info,
                                        tps_ace_info p_ace_info,
                                        tps_hamac_info p_hamac_info,
                                        tps_image_info p_image_info,
                                        void * param);

t_djpeg_error_code jpegdec_huff_seq_decode_mcu_422_dn(tps_bitstream_buffer_opt p_bitstream, 
                                        tps_mcu p_mcu, 
                                        tps_post_process_info p_pp_info,
                                        tps_huff_seq_info p_huff_info,
                                        tps_ace_info p_ace_info,
                                        tps_hamac_info p_hamac_info,
                                        tps_image_info p_image_info,
                                        void * param);

t_djpeg_error_code jpegdec_huff_seq_decode_mcu_444_dn (tps_bitstream_buffer_opt p_bitstream, 
                                        tps_mcu p_mcu, 
                                        tps_post_process_info p_pp_info,
                                        tps_huff_seq_info p_huff_info,
                                        tps_ace_info p_ace_info,
                                        tps_hamac_info p_hamac_info,
                                        tps_image_info p_image_info,
                                        void * param);                                   

void jpegdec_huff_prog_init(tps_component p_y, 
                    tps_component p_cb,
                    tps_component p_cr, 
                    tps_stop_state p_stop_state,
                    tps_frame  p_frame,
                    tps_hv_scan p_scan,
                    tps_mcu p_frame_mcu, 
                    t_ulong_value add_coeff_buffer,
                    tps_huff_prog_info p_huff_info);


void jpegdec_huff_save_stop_state_opt(tps_bitstream_buffer_opt p_bitstream, 
                            tps_mcu_predictor p_pred, 
                            t_uint32 mcu_index, 
                            t_uint32 crop_mcu_index, 
                            t_uint32 crop_mcu_index_in_row,
                            t_uint32 end_of_band_run, 
                            tps_stop_state p_stop_state);



void jpegdec_calculate_mcu_address(tps_t1xhv_vdc_jpeg_global jpeg_global,tps_image_info p_image_info,tps_stop_state p_stop_state);

t_djpeg_error_code jpegdec_huff_prog_decode_scan (tps_bitstream_buffer p_bitstream, tps_huff_prog_info p_huff_info);



void jpegdec_huff_prog_retrieve_iq_idct_mcu(tps_huff_prog_info p_huff_info,
                                    tps_mcu p_mcu,
                                    tps_post_process_info p_pp_info,
                                    t_uint32 mcu_index, 
                                    tps_ace_info p_ace_info);
#endif /* _DJPEG_HV_HUFFMAN_H_ */
