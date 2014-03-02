/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \file 	djpeg_bitstream_optmize.h
 * \brief 	common jpeg bitstream low level parsing function declaration
 * \author  ST-Ericsson
 *    
 *  This file declares common jpeg bitstream low level parsing functions.
 */
/*****************************************************************************/

#ifndef _DJPEG_BITSTREAM_OPTIMIZE_H_
#define _DJPEG_BITSTREAM_OPTIMIZE_H_

/*------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------*/
#include "djpeg_common.h"  /* for types definitions */
#ifdef __NMF
#include <t1xhv_vdc_jpeg.idt>
#include <jpegdec.idt>
//#include "t1xhv_vdc_jpeg.idt"
#include "t1xhv_common.idt"
#include "t1xhv_vdc.idt"
#else
#include "djpeg_interface.h"
#endif

#define BSWAP(x) \
	x = ((((x) & 0xff000000) >> 24) | \
			(((x) & 0x00ff0000) >>  8) | \
			(((x) & 0x0000ff00) <<  8) | \
			(((x) & 0x000000ff) << 24))


typedef struct s_bitstream_buffer_opt {

	ts_t1xhv_bitstream_buf_pos buf_pos;
	ts_t1xhv_bitstream_buf_link  p_buf_link;	
	t_uint32  size_in_bits; /**< \brief bitstream size in bits   */
	
	unsigned int ui32_rdbfr;       /* input read buffer 32-bits */
	t_uint32 *rdbfr;    /* input read buffer */
	t_uint32 *rdptr;    /* input read buffer pointer */
	t_uint32 a,b; // internal variables
	t_uint32 inbfr[2]; /* 16 */ /* internal buffer */
	t_uint32 * last_wrd_ptr;
	t_uint32  last_wrd_bits;
	t_uint32 incnt;                       /* internal counter */
	t_uint32 *startptr1;
	t_uint32 *startptr2;
	t_uint32 int_buf_cnt;  // for locating exact location of last successfully decoded mcu
	t_uint32 *endptr, *wrptr;
	t_uint32 ff_flag1,ff_flag2,prev_ff_flag;
	t_uint32 bytes_in_last_word;
	t_uint32 bits_left; // bits left in last word
	t_uint32 threshold;
}ts_bitstream_buffer_opt;

typedef ts_bitstream_buffer_opt *tps_bitstream_buffer_opt; /**< \brief pointer on ts_bitstream_buffer   */

t_djpeg_error_code refill_buffer(tps_bitstream_buffer_opt p_bitstream,t_uint32 Offset);
void  btst_init_from_host_address_opt(tps_bitstream_buffer_opt p_bitstream,ts_t1xhv_bitstream_buf_pos* p_bitstream_buf_info,T_BOOL new_jpeg_image);
void initbfr_omx(tps_bitstream_buffer_opt p_bitstream) ;
t_djpeg_error_code fill_buffer(tps_bitstream_buffer_opt p_bitstream,t_uint32 ** buffer);
t_uint32 btst_get_current_bit_offset_opt(tps_bitstream_buffer_opt p_bitstream);
t_ulong_value btst_get_current_byte_address_opt(tps_bitstream_buffer_opt p_bitstream,t_ulong_value * mcu_add, t_uint32 * offset);
t_djpeg_error_code btst_align_byte_opt(tps_bitstream_buffer_opt p_bitstream);
t_djpeg_error_code btst_flushbits_opt(tps_bitstream_buffer_opt p_bitstream, int n );
t_djpeg_error_code btst_getbits_opt(tps_bitstream_buffer_opt p_bitstream, t_uint16 n, t_uint16 *p_bits);

t_uint32 armGetBits(t_uint32 **ppBitStream, t_uint32 *pOffset, t_uint16 N);
t_djpeg_error_code btst_showbits_opt(t_uint32 N,t_uint16 * p_bits);
void check_first_word(t_uint32 * word,T_BOOL * ff_flag,t_uint16 * new_bytes_needed,t_uint32 *offset_in_bits);
void check_word(t_uint32 * word,T_BOOL * ff_flag,t_uint16 * new_bytes_needed);

#endif  //#define _DJPEG_BITSTREAM_OPTIMIZE_H_
