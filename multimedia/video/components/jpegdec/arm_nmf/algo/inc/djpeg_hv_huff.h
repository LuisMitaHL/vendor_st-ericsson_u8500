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


#ifndef _DJPEG_HV_HUFF_H_
#define _DJPEG_HV_HUFF_H_

/*------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------*/

#include "djpeg_hv_huffman.h"
#include "djpeg_hamac.h"
#include "djpeg_co_bitstream.h"  /* for tps_bitstream_buffer def */
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

t_djpeg_error_code jpegdec_huff_seq_decode_mcu_crop (tps_bitstream_buffer_opt p_bitstream, 
                                        tps_mcu p_mcu, 
                                        tps_post_process_info p_pp_info,
                                        tps_huff_seq_info p_huff_info,
                                        tps_ace_info p_ace_info,
                                        tps_hamac_info p_hamac_info,
                                        tps_image_info p_image_info,
                                        T_BOOL  belong_to_window,
                                       void * param);
t_djpeg_error_code jpegdec_huff_seq_decode_mcu_crop_monochrome(tps_bitstream_buffer_opt p_bitstream, 
                                        tps_mcu p_mcu, 
                                        tps_post_process_info p_pp_info,
                                        tps_huff_seq_info p_huff_info,
                                        tps_ace_info p_ace_info,
                                        tps_hamac_info p_hamac_info,
                                        tps_image_info p_image_info,
                                        T_BOOL  belong_to_window,
                                        void * param);
t_djpeg_error_code jpegdec_huff_seq_decode_mcu_crop_420(tps_bitstream_buffer_opt p_bitstream, 
                                        tps_mcu p_mcu, 
                                        tps_post_process_info p_pp_info,
                                        tps_huff_seq_info p_huff_info,
                                        tps_ace_info p_ace_info,
                                        tps_hamac_info p_hamac_info,
                                        tps_image_info p_image_info,
                                        T_BOOL  belong_to_window,
                                        void * param);
t_djpeg_error_code jpegdec_huff_seq_decode_mcu_crop_422 (tps_bitstream_buffer_opt p_bitstream, 
                                        tps_mcu p_mcu, 
                                        tps_post_process_info p_pp_info,
                                        tps_huff_seq_info p_huff_info,
                                        tps_ace_info p_ace_info,
                                        tps_hamac_info p_hamac_info,
                                        tps_image_info p_image_info,
                                        T_BOOL  belong_to_window,
                                        void * param);
t_djpeg_error_code jpegdec_huff_seq_decode_mcu_crop_444 (tps_bitstream_buffer_opt p_bitstream, 
                                        tps_mcu p_mcu, 
                                        tps_post_process_info p_pp_info,
                                        tps_huff_seq_info p_huff_info,
                                        tps_ace_info p_ace_info,
                                        tps_hamac_info p_hamac_info,
                                        tps_image_info p_image_info,
                                        T_BOOL  belong_to_window,
                                       void * param);

t_djpeg_error_code jpegdec_huff_seq_decode_mcu_crop (tps_bitstream_buffer_opt p_bitstream, 
                                        tps_mcu p_mcu, 
                                        tps_post_process_info p_pp_info,
                                        tps_huff_seq_info p_huff_info,
                                        tps_ace_info p_ace_info,
                                        tps_hamac_info p_hamac_info,
                                        tps_image_info p_image_info,
                                        T_BOOL  belong_to_window,
                                        void * param);
t_djpeg_error_code jpegdec_huff_seq_decode_mcu_crop_monochrome_dn(tps_bitstream_buffer_opt p_bitstream, 
                                        tps_mcu p_mcu, 
                                        tps_post_process_info p_pp_info,
                                        tps_huff_seq_info p_huff_info,
                                        tps_ace_info p_ace_info,
                                        tps_hamac_info p_hamac_info,
                                        tps_image_info p_image_info,
                                        T_BOOL  belong_to_window,
                                       void * param);
t_djpeg_error_code jpegdec_huff_seq_decode_mcu_crop_420_dn(tps_bitstream_buffer_opt p_bitstream, 
                                        tps_mcu p_mcu, 
                                        tps_post_process_info p_pp_info,
                                        tps_huff_seq_info p_huff_info,
                                        tps_ace_info p_ace_info,
                                        tps_hamac_info p_hamac_info,
                                        tps_image_info p_image_info,
                                        T_BOOL  belong_to_window,
                                       void * param);
t_djpeg_error_code jpegdec_huff_seq_decode_mcu_crop_422_dn (tps_bitstream_buffer_opt p_bitstream, 
                                        tps_mcu p_mcu, 
                                        tps_post_process_info p_pp_info,
                                        tps_huff_seq_info p_huff_info,
                                        tps_ace_info p_ace_info,
                                        tps_hamac_info p_hamac_info,
                                        tps_image_info p_image_info,
                                        T_BOOL  belong_to_window,
                                        void * param);
t_djpeg_error_code jpegdec_huff_seq_decode_mcu_crop_444_dn (tps_bitstream_buffer_opt p_bitstream, 
                                        tps_mcu p_mcu, 
                                        tps_post_process_info p_pp_info,
                                        tps_huff_seq_info p_huff_info,
                                        tps_ace_info p_ace_info,
                                        tps_hamac_info p_hamac_info,
                                        tps_image_info p_image_info,
                                        T_BOOL  belong_to_window,
                                        void * param);


t_djpeg_error_code jpegdec_huff_seq_decode_mcu_crop_dn (tps_bitstream_buffer_opt p_bitstream, 
                                        tps_mcu p_mcu, 
                                        tps_post_process_info p_pp_info,
                                        tps_huff_seq_info p_huff_info,
                                        tps_ace_info p_ace_info,
                                        tps_hamac_info p_hamac_info,
                                        tps_image_info p_image_info,
                                        T_BOOL  belong_to_window,
                                        void * param);
t_djpeg_error_code jpegdec_huff_seq_decode_mcu_crop_monochrome_dn_8(tps_bitstream_buffer_opt p_bitstream, 
                                        tps_mcu p_mcu, 
                                        tps_post_process_info p_pp_info,
                                        tps_huff_seq_info p_huff_info,
                                        tps_ace_info p_ace_info,
                                        tps_hamac_info p_hamac_info,
                                        tps_image_info p_image_info,
                                        T_BOOL  belong_to_window,
                                        void * param);
t_djpeg_error_code jpegdec_huff_seq_decode_mcu_crop_420_dn_8(tps_bitstream_buffer_opt p_bitstream, 
                                        tps_mcu p_mcu, 
                                        tps_post_process_info p_pp_info,
                                        tps_huff_seq_info p_huff_info,
                                        tps_ace_info p_ace_info,
                                        tps_hamac_info p_hamac_info,
                                        tps_image_info p_image_info,
                                        T_BOOL  belong_to_window,
                                        void * param);
t_djpeg_error_code jpegdec_huff_seq_decode_mcu_crop_422_dn_8 (tps_bitstream_buffer_opt p_bitstream, 
                                        tps_mcu p_mcu, 
                                        tps_post_process_info p_pp_info,
                                        tps_huff_seq_info p_huff_info,
                                        tps_ace_info p_ace_info,
                                        tps_hamac_info p_hamac_info,
                                        tps_image_info p_image_info,
                                        T_BOOL  belong_to_window,
                                       void * param);
t_djpeg_error_code jpegdec_huff_seq_decode_mcu_crop_444_dn_8 (tps_bitstream_buffer_opt p_bitstream, 
                                        tps_mcu p_mcu, 
                                        tps_post_process_info p_pp_info,
                                        tps_huff_seq_info p_huff_info,
                                        tps_ace_info p_ace_info,
                                        tps_hamac_info p_hamac_info,
                                        tps_image_info p_image_info,
                                        T_BOOL  belong_to_window,
                                       void * param);

t_djpeg_error_code jpegdec_huff_seq_decode_mcu_crop_dn_8 (tps_bitstream_buffer_opt p_bitstream, 
                                        tps_mcu p_mcu, 
                                        tps_post_process_info p_pp_info,
                                        tps_huff_seq_info p_huff_info,
                                        tps_ace_info p_ace_info,
                                        tps_hamac_info p_hamac_info,
                                        tps_image_info p_image_info,
                                        T_BOOL  belong_to_window,
                                        void * param);



#endif /* _DJPEG_HV_HUFFMAN_H_ */
