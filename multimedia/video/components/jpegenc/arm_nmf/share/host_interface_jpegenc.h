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

#ifndef __NMF
#include "types.h"

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
    t_short_value dc_predictor_y;             /**<\brief Added in v0.96 */
    t_short_value dc_predictor_cb;            /**<\brief Added in v0.96 */
    t_short_value dc_predictor_cr;            /**<\brief Added in v0.96 */
    t_ushort_value restart_marker_id;
    t_ushort_value reserved_1;                 /**<\brief To align struct on 128b */
    t_ulong_value  reserved_2;                 /**<\brief To align struct on 128b */

} ts_t1xhv_vec_jpeg_param_inout, *tps_t1xhv_vec_jpeg_param_inout;

/** \brief Hamac decoder jpeg param in  */
/** \brief Structure for parameters FROM Host for a JPEG encode task */
typedef enum t_xyuv_t_t1xhv_encoder_info {
  VEC_ERT_NONE,
  VEC_VOID,
  VEC_INFO_SKIP,
  VEC_ERT,
  VEC_ERT_NOT_SUPPORTED,
  VEC_ERT_BAD_PARAMETER,
  VEC_ERT_FATAL_ERROR,
  VEC_ERT_ADDR_SOURCE_BUFFER,
  VEC_ERT_ADDR_FWD_REF_BUFFER,
  VEC_ERT_ADDR_GRAB_REF_BUFFER,
  VEC_ERT_ADDR_DEST_BUFFER,
  VEC_ERT_ADDR_DEBLOCKING_PARAM_BUFFER,
  VEC_ERT_ADDR_MOTION_VECTOR_BUFFER,
  VEC_ERT_ADDR_INTRA_REFRESH_BUFFER,
  VEC_ERT_ADDR_SEARCH_WINDOW_BUFFER,
  VEC_ERT_ADDR_SEARCH_WINDOW_END,
  VEC_ERT_ADDR_JPEG_RUN_LEVEL_BUFFER,
  VEC_ERT_ADDR_HEADER_BUFFER,
  VEC_ERT_ADDR_BITSTREAM_START,
  VEC_ERT_ADDR_BUFFER_START,
  VEC_ERT_ADDR_BUFFER_END,
  VEC_ERT_ADDR_WINDOW_START,
  VEC_ERT_ADDR_WINDOW_END,
  VEC_ERT_FRAME_WIDTH,
  VEC_ERT_FRAME_HEIGHT,
  VEC_ERT_WINDOW_WIDTH,
  VEC_ERT_WINDOW_HEIGHT,
  VEC_ERT_WINDOW_HORIZONTAL_OFFSET,
  VEC_ERT_WINDOW_VERTICAL_OFFSET,
  VEC_ERT_QUANT,
  VEC_ERT_VOP_FCODE_FORWARD,
  VEC_ERT_QUANT_TABLE,
  VEC_ERT_DATA_PARTITIONED,
  ERR_ENCODE_NOT_FOUND} t_t1xhv_encoder_info;


typedef struct t_xyuv_ts_t1xhv_vec_header_buf {
  t_address addr_header_buffer;
  t_ulong_value header_size;
  t_address reserved_1;
  t_address reserved_2;
} ts_t1xhv_vec_header_buf;

typedef ts_t1xhv_vec_header_buf* tps_t1xhv_vec_header_buf;

typedef struct 
{
    t_address   addr_bitstream_buf_struct; 
    t_address   addr_bitstream_start;    
    t_uint32   bitstream_offset;     
    t_uint32   reserved_1;                
} ts_t1xhv_bitstream_buf_pos,*tps_t1xhv_bitstream_buf_pos;

/** \brief This structure define a link buffer. */
typedef struct 
{
    t_address addr_next_buf_link;   
    t_address addr_prev_buf_link;   
    t_address addr_buffer_start;  
    t_address addr_buffer_end;    
} ts_t1xhv_bitstream_buf_link,*tps_t1xhv_bitstream_buf_link;

/** \brief This structure defines a Encode input frame buffers. */
typedef struct {
    t_ahb_address  addr_source_buffer;     
    t_ahb_address  addr_fwd_ref_buffer;     
    t_ahb_address  addr_grab_ref_buffer;      
    t_ahb_address  addr_intra_refresh_buffer; 
} ts_t1xhv_vec_frame_buf_in;

typedef ts_t1xhv_vec_frame_buf_in *tps_t1xhv_vec_frame_buf_in;

typedef enum {
  STATUS_JOB_COMPLETE,		/**<\brief The component run completely its job. errors should then be VDC_ERT_NONE. */
  STATUS_JOB_ABORTED,		/**<\brief The component has been aborted. */
  STATUS_JOB_UNKNOWN,		/**<\brief This should not happen. */
  STATUS_BUFFER_NEEDED		/**<\brief The component requires more buffer to complete its job*/
} t_t1xhv_status;


/** \brief This structure defines the parameters which are used during the frame encoding*/
{
	t_uint16 YQTable_hamac[64];
	t_uint16 CQTable_hamac[64];

	t_uint32 DChufftable;
	t_uint32 AChufftable;

	t_uint8 * output_bytes;
	t_sint32 jpg_put_buffer;
	t_sint32 jpg_put_bits;
	t_uint32 nStartBits;
	t_uint32 nTotalBits;
	t_uint32 next_output_byte;

	t_uint32 MCU_WIDTH;
	t_uint32 MCU_HEIGHT;

	t_uint16 mblk_column;
	t_uint16 mblk_line;
	t_uint32 bitstreamBufferSize;

	t_sint16 DC_Prec_Y;
	t_sint16 DC_Prec_Cr;
	t_sint16 DC_Prec_Cb;
	t_sint16 reserved2;

} ts_t1xhv_vec_jpeg_global;
typedef ts_t1xhv_vec_jpeg_global *tps_t1xhv_vec_jpeg_global;


/** \brief This is the main structure*/
{
	tps_t1xhv_vec_frame_buf_in     Gps_vec_jpeg_frame_buf_in;
	tps_t1xhv_vec_frame_buf_out    Gps_vec_jpeg_frame_buf_out;
	tps_t1xhv_vec_internal_buf     Gps_t1xhv_vec_internal_buf;
	tps_t1xhv_vec_header_buf	   Gps_jpeg_bitstream_buf_header;
	tps_t1xhv_bitstream_buf_pos    Gps_jpeg_bitstream_buf_pos;
	tps_t1xhv_bitstream_buf_pos    Gps_jpeg_bitstream_buf_pos_out;
	tps_t1xhv_vec_jpeg_param_in    Gps_vec_jpeg_param_in;
	tps_t1xhv_vec_jpeg_param_out   Gps_vec_jpeg_param_out;
	tps_t1xhv_vec_jpeg_param_inout Gps_vec_jpeg_param_inout;
	tps_t1xhv_vec_jpeg_param_inout Gps_vec_jpeg_param_inout_out;
	ts_t1xhv_vec_jpeg_global       Gs_t1xhv_vec_jpeg_global;
} ts_t1xhv_vec_jpeg_session;
typedef ts_t1xhv_vec_jpeg_session *tps_t1xhv_vec_jpeg_session;
/*------------------------------------------------------------------------
 * Functions (exported)
 *----------------------------------------------------------------------*/
void jpegenc_configure_algo(t_uint32 addr_session_buffer, 
							t_uint32 addr_in_frame_buffer,
							t_uint32 addr_out_frame_buffer,
							t_uint32 addr_internal_buffer,
							t_uint32 addr_in_header_buffer,
							t_uint32 addr_in_bitstream_buffer,
							t_uint32 addr_out_bitstream_buffer,
							t_uint32 addr_in_parameters,
							t_uint32 addr_out_parameters,
							t_uint32 addr_in_frame_parameters,
							t_uint32 addr_out_frame_parameters
							t_uint32 addr_vec_jpeg_session);

void jpegenc_compress(t_uint32 addr_session_buffer);
#endif
#endif /* _T1XHV_HOST_INTERFACE_JPEGENC_H_ */

