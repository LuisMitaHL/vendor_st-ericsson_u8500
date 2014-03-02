/******************************************************************************
 Copyright (c) 2009-2011, ST-Ericsson SA
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:
 
   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials
      provided with the distribution.
   3. Neither the name of the ST-Ericsson SA nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior written
      permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/


#ifndef _T1XHV_HOST_INTERFACE_JPEGENC_H_
#define _T1XHV_HOST_INTERFACE_JPEGENC_H_

#include "t1xhv_retarget.h"

/** \brief Structure for parameters FROM Host for a JPEG encode task */
typedef struct t1xhv_vec_jpeg_param_in {

    t_ushort_value frame_width;                /**<\brief Nb of pixel per line                 */
    t_ushort_value frame_height;               /**<\brief Nb of line                           */
    t_ushort_value window_width;               /**<\brief Nb of pixels per line to be encoded  */
    t_ushort_value window_height;              /**<\brief Nb of lines to be encoded            */
    t_ushort_value window_horizontal_offset;   /**<\brief Nb of pixels for horizontal offset   */
    t_ushort_value window_vertical_offset;     /**<\brief Nb of pixels for vertical offset     */
    t_ushort_value sampling_mode;              /**<\brief Added in v0.96                       */
    t_ushort_value restart_interval;           /**<\brief Nb of MCUs between 2 restart markers */
    t_ushort_value quant_luma[64];             /**<\brief Quantization table for luma          */
    t_ushort_value quant_chroma[64];           /**<\brief Quantization table for chroma        */
    t_ushort_value last_slice;                 /**<\brief Added in v0.96                       */
    t_ushort_value enable_optimized_quant;     /**<\brief Added in v0.96                       */
    t_ushort_value target_bpp;                 /**<\brief Added in v0.96                       */
    t_ushort_value enable_optimized_huffman;   /**<\brief Added in v0.96                       */
    t_ushort_value rotation;                   /**<\brief 0=no rotate,1=rotate 90,2=rotate -90 */
    t_ushort_value reserved_1;                 /**<\brief reserved 32              */
    t_ulong_value  reserved_2;                 /**<\brief reserved 32              */
   
} ts_t1xhv_vec_jpeg_param_in, *tps_t1xhv_vec_jpeg_param_in;

/** \brief Structure for output parameters of JPEG encode task */ 
typedef struct t1xhv_vec_jpeg_param_out {

    t_ushort_value error_type;                 /**<\brief Error status                      */
    t_ushort_value reserved;                   /**<\brief To align next field  on 32b boundary */
    t_ulong_value  bitstream_size;             /**<\brief Size of encoded bitstream in bits */
    t_ulong_value  reserved_1;                 /**<\brief To align struct size on 128b           */
    t_ulong_value  reserved_2;                 /**<\brief To align struct size on 128b           */

} ts_t1xhv_vec_jpeg_param_out, *tps_t1xhv_vec_jpeg_param_out;

/** \brief Structure for output parameters of JPEG encode task */
typedef struct t1xhv_vec_jpeg_param_inout {

    t_ushort_value restart_mcu_count;          /**<\brief Added in v0.96 */
    t_ushort_value dc_predictor_y;             /**<\brief Added in v0.96 */
    t_ushort_value dc_predictor_cb;            /**<\brief Added in v0.96 */
    t_ushort_value dc_predictor_cr;            /**<\brief Added in v0.96 */
    t_ushort_value restart_marker_id;
    t_ushort_value reserved_1;                 /**<\brief To align struct on 128b */
    t_ulong_value  reserved_2;                 /**<\brief To align struct on 128b */

} ts_t1xhv_vec_jpeg_param_inout, *tps_t1xhv_vec_jpeg_param_inout;

#endif /* _T1XHV_HOST_INTERFACE_JPEGENC_H_ */

