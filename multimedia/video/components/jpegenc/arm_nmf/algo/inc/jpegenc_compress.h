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

#ifndef JPEGENC_COMPRESS_H
#define JPEGENC_COMPRESS_H

#ifdef __NMF
	#include "t1xhv_vec_jpeg.idt"
#else
	#include "host_interface_jpegenc.h"
#endif

/************************/
/*		Data types		*/
/************************/
/* Derived data constructed for each Huffman table */

typedef struct 
{
  t_uint32	ehufco[256];	/* code for each symbol */
  t_uint8	ehufsi[256];		/* length of code for each symbol */
  /* If no code has been allocated for a symbol S, ehufsi[S] contains 0 */
} jpegenc_huffman_table;


/***********************/
/* Function prototypes */
/***********************/
#ifdef __NMF
t_uint16 jpegenc_compress(t_uint32);
#endif

t_uint16 jpegenc_encode_frame(tps_t1xhv_vec_jpeg_session Gps_vec_jpeg_session);
t_sint32 jpegenc_encode_block(t_uint8 *position_in,t_sint16 *Prec_DC, t_uint32 width, t_uint16 * qtable,t_uint8 compType, tps_t1xhv_vec_jpeg_session Gps_vec_jpeg_session);

void jpegenc_init_info(tps_t1xhv_vec_jpeg_session Gps_vec_jpeg_session);
void jpegenc_write_headers(tps_t1xhv_vec_jpeg_session Gps_vec_jpeg_session);//hide these functions
void jpegenc_write_EOI(tps_t1xhv_vec_jpeg_global      Gps_t1xhv_vec_jpeg_global);
void jpegenc_write_RST(t_sint32, tps_t1xhv_vec_jpeg_global      Gps_t1xhv_vec_jpeg_global);
void jpegenc_reset_data(tps_t1xhv_vec_jpeg_global      Gps_t1xhv_vec_jpeg_global);
t_sint32 jpegenc_encode_MCU(t_sint32, tps_t1xhv_vec_jpeg_session Gps_vec_jpeg_session);
t_sint32 jpegenc_encode_MCU_rotation(t_uint16 x_mecc, t_uint16 y_mecc, tps_t1xhv_vec_jpeg_session Gps_vec_jpeg_session);
void jpegenc_byte_align(tps_t1xhv_vec_jpeg_global      Gps_t1xhv_vec_jpeg_global);
void jpegenc_emit_byte (t_uint32, tps_t1xhv_vec_jpeg_global      Gps_t1xhv_vec_jpeg_global);
void jpegenc_write_n_bytes(t_uint32 nbytes, t_uint8 *p_hdr, tps_t1xhv_vec_jpeg_global      Gps_t1xhv_vec_jpeg_global);

t_sint32 jpegenc_emit_AC_events(t_uint8	run, t_uint8 category, t_sint16	value, tps_t1xhv_vec_jpeg_global      Gps_t1xhv_vec_jpeg_global);

void jpegenc_emit_bits (t_uint32 code, t_sint32 size, tps_t1xhv_vec_jpeg_global      Gps_t1xhv_vec_jpeg_global);
t_sint32 jpegenc_quant(t_sint32 f, t_sint32 qp, t_sint32 r, t_sint32 s, t_sint32 t, tps_t1xhv_vec_jpeg_session Gps_vec_jpeg_session);

void jpegenc_rasterize_block(t_uint8 *, t_sint16 *, t_uint32);
void jpegenc_rasterize_block_rot1(t_uint8 *, t_sint16 *, t_uint32);
void jpegenc_rasterize_block_rot2(t_uint8 *, t_sint16 *, t_uint32);

#endif //JPEGENC_COMPRESS_H

