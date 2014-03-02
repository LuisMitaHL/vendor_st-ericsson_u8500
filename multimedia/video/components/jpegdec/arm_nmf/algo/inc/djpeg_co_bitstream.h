/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \file 	djpeg_co_bitstream.h
 * \brief 	common jpeg bitstream low level parsing function declaration
 * \author  ST-Ericsson
 *    
 *  This file declares common jpeg bitstream low level parsing functions.
 */
/*****************************************************************************/

#ifndef _DJPEG_CO_BITSTREAM_H_
#define _DJPEG_CO_BITSTREAM_H_

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
/*------------------------------------------------------------------------
 * Defines							       
 *----------------------------------------------------------------------*/

/* /\** @{ \name  bitstream handling function error code   *\/ */

/* /\** \brief all is ok!   *\/ */
/* #define  BTST_OK                0x0U    */

/* /\** \brief end of bitstream data reached in current bitstream buffer   *\/  */
/* #define  BTST_END_OF_DATA       0x1U    */

/* /\** \brief error opening bitstream file for reading *\/ */
/* #define  BTST_FILE_ERROR        0x2U    */

/* /\** \brief no sufficient memory to store bitstrean *\/ */
/* #define  BTST_MALLOC_ERROR      0x4U  */

/* /\** \brief end of file reached *\/ */
/* #define  BTST_FILE_END         0x8U  */
  
/* /\** @} end of bitstrean error codes *\/ */


/*------------------------------------------------------------------------
 * Types
 *----------------------------------------------------------------------*/

/** @{ \name bitstream handling structure definition */
/*---------------------------------------------------*/
/** \brief bitstream handling structure */
typedef struct s_bitstream_buffer 
{
    FILE        *p_file;  /**< \brief bitstream file */
    ts_t1xhv_bitstream_buf_pos buf_pos;
    ts_t1xhv_bitstream_buf_link  p_buf_link;	
    t_uint8 is_next_buf_link_used;
    t_uint16 nextbuf_link_cnt_byte_removed;
    t_uint8 	*data;	/**< \brief buffer allocated to store bitstream data (bytes)   */
    t_uint8 	*pt_current_byte; /**< \brief pointer on current byte in bitstream data   */
    t_uint32 	index_current_bit; /**< \brief index of current bit in bitstream data from 0 to size_in_bits -1    */
    t_uint32      size_in_bits; /**< \brief bitstream size in bits   */
    T_BOOL      remove_stuffed_bytes;   /**< \brief set to true to remove 0x00 in 0xff00 sequences   */
    t_uint16      cnt_byte_removed;        /**< \brief flag memorising how many bytes were removed during last show function */
    t_uint8       last_read_byte;         /**< \brief for stuffed bytes removal   */

}  ts_bitstream_buffer;

typedef ts_bitstream_buffer *tps_bitstream_buffer; /**< \brief pointer on ts_bitstream_buffer   */

/** @} end of bitstream handling structure definition */


/*------------------------------------------------------------------------
 * Functions
 *----------------------------------------------------------------------*/



t_djpeg_error_code  btst_align_byte(tps_bitstream_buffer p_bitstream);
void  btst_align_byte_ns(tps_bitstream_buffer p_bitstream);
T_BOOL  btst_is_byte_aligned(tps_bitstream_buffer p_bitstream);
t_djpeg_error_code  btst_showbits( tps_bitstream_buffer p_bitstream, t_uint16 n, t_uint16 *p_bits);
t_djpeg_error_code  btst_flushbits(tps_bitstream_buffer p_bitstream, t_uint16 n);
t_djpeg_error_code  btst_getbits(tps_bitstream_buffer p_bitstream, t_uint16 n, t_uint16 *p_bits);
t_uint16  btst_showbits_ns( tps_bitstream_buffer p_bitstream, t_uint16 n);
void  btst_flushbits_ns(tps_bitstream_buffer p_bitstream, t_uint16 n);
t_uint16  btst_getbits_ns(tps_bitstream_buffer p_bitstream, t_uint16 n);
t_uint32  btst_get_bit_index(tps_bitstream_buffer p_bitstream);
void btst_print_32bits(tps_bitstream_buffer p_bitstream);
t_ulong_value btst_get_current_byte_address(tps_bitstream_buffer p_bitstream);
t_uint32 btst_get_current_bit_offset(tps_bitstream_buffer p_bitstream);
t_uint32 btst_get_current_byte_index(tps_bitstream_buffer p_bitstream);

#endif /* _DJPEG_CO_BITSTREAM_H_ */
