/****************************************************************************
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \file 	djpeg_bitstream_lib.h
 * \brief 	jpeg decoder common header file
 * \author  ST-Ericsson
 *
 *
 *
 */
/*****************************************************************************/

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
#include "djpeg_global.h"
#endif

//#include "djpeg_bitstream_optimize.h"
#include "djpeg_hamac.h"


typedef t_sint32 bit_buf_type;	/* type of bit-extraction buffer */
#define BIT_BUF_SIZE  32	/* size of buffer in bits */
#define DCTSIZE		    8	/* The basic DCT block is 8x8 samples */
#define DCTSIZE2	    64	/* DCTSIZE squared; # of elements in a block */


/*
 * Out-of-line code for bit fetching (shared with jdphuff.c).
 * See jdhuff.h for info about usage.
 * Note: current values of get_buffer and bits_left are passed as parameters,
 * but are returned in the corresponding fields of the state struct.
 *
 * On most machines MIN_GET_BITS should be 25 to allow the full 32-bit width
 * of get_buffer to be used.  (On machines with wider words, an even larger
 * buffer could be used.)  However, on some machines 32-bit shifts are
 * quite slow and take time proportional to the number of places shifted.
 * (This is true with most PC compilers, for instance.)  In this case it may
 * be a win to set MIN_GET_BITS to the minimum value of 15.  This reduces the
 * average shift distance at the cost of more calls to jpeg_fill_bit_buffer.
 */

#ifdef SLOW_SHIFT_32
#define MIN_GET_BITS  15	/* minimum allowable value */
#else
#define MIN_GET_BITS  (BIT_BUF_SIZE-7)
#endif

typedef struct {		/* Bitreading working state within an MCU */
  /* Current data source location */
  /* We need a copy, rather than munging the original, in case of suspension */
  t_uint8 * next_input_byte; /* => next byte to read from source */
  t_uint32 bytes_in_buffer;	/* # of bytes remaining in source buffer */
  /* Bit input buffer --- note these values are kept in register variables,
   * not in this struct, inside the inner loops.
   */
  bit_buf_type get_buffer;	/* current bit-extraction buffer */
  t_sint16 bits_left;		/* # of unused bits in it */
  /* Pointer needed by jpeg_fill_bit_buffer. */
  tps_bitstream_buffer_opt bitstream_info;	/* back link to decompress master record */
} bitread_working_state;

/* Macros to declare and load/save bitread local variables. */
#define BITREAD_STATE_VARS  \
	register bit_buf_type get_buffer;  \
	register t_sint16 bits_left;  \
	bitread_working_state br_state

#define BITREAD_LOAD_STATE(p_bitstream)  \
	br_state.bitstream_info = p_bitstream; \
	br_state.next_input_byte = p_bitstream->next_input_byte; \
	br_state.bytes_in_buffer = p_bitstream->bytes_in_buffer; \
	get_buffer = p_bitstream->bitstate.get_buffer; \
	bits_left = p_bitstream->bitstate.bits_left;

#define BITREAD_SAVE_STATE(p_bitstream)  \
	p_bitstream->next_input_byte = br_state.next_input_byte; \
	p_bitstream->bytes_in_buffer = br_state.bytes_in_buffer; \
	p_bitstream->bitstate.get_buffer = get_buffer; \
	p_bitstream->bitstate.bits_left = bits_left


/*
 * These macros provide the in-line portion of bit fetching.
 * Use CHECK_BIT_BUFFER to ensure there are N bits in get_buffer
 * before using GET_BITS, PEEK_BITS, or DROP_BITS.
 * The variables get_buffer and bits_left are assumed to be locals,
 * but the state struct might not be (jpeg_huff_decode needs this).
 *	CHECK_BIT_BUFFER(state,n,action);
 *		Ensure there are N bits in get_buffer; if suspend, take action.
 *      val = GET_BITS(n);
 *		Fetch next N bits.
 *      val = PEEK_BITS(n);
 *		Fetch next N bits without removing them from the buffer.
 *	DROP_BITS(n);
 *		Discard next N bits.
 * The value N should be a simple variable, not an expression, because it
 * is evaluated multiple times.
 */

#define CHECK_BIT_BUFFER(state,nbits,action) \
	{   register T_BOOL res; \
	    if (bits_left < (nbits)) {  \
	    if (!(res= jpeg_fill_bit_buffer(&(state),get_buffer,bits_left,nbits)))  \
	      { action; }  \
	    get_buffer = (state).get_buffer; bits_left = (state).bits_left; } }

#define ALIGN_BIT_BUFFER(state,action) \
	{bits_left-=(bits_left%8);}
		
#define GET_BITS(nbits) \
	(((t_sint16) (get_buffer >> (bits_left -= (nbits)))) & ((1<<(nbits))-1))

#define PEEK_BITS(nbits) \
	(((t_sint16) (get_buffer >> (bits_left -  (nbits)))) & ((1<<(nbits))-1))

#define DROP_BITS(nbits) \
	(bits_left -= (nbits))


/* Out-of-line case for Huffman code fetching */
extern t_sint16  jpegdec_huff_decode_ac(bitread_working_state * state, register bit_buf_type get_buffer,
	     register t_sint32 bits_left, ts_ac_table * htbl, t_sint32 min_bits);

extern t_sint16  jpegdec_huff_decode_dc(bitread_working_state * state, register bit_buf_type get_buffer,
	     register t_sint32 bits_left, ts_dc_table * htbl, t_sint32 min_bits);

 T_BOOL  jpeg_fill_bit_buffer (bitread_working_state * state,
		      register bit_buf_type get_buffer, register t_sint32 bits_left,
		      t_sint32 nbits);

 t_djpeg_error_code btst_align_byte_opt(tps_bitstream_buffer_opt p_bitstream);

/*
 * Code for extracting next Huffman-coded symbol from input bit stream.
 * Again, this is time-critical and we make the main paths be macros.
 *
 * We use a lookahead table to process codes of up to HUFF_LOOKAHEAD bits
 * without looping.  Usually, more than 95% of the Huffman codes will be 8
 * or fewer bits long.  The few overlength codes are handled with a loop,
 * which need not be inline code.
 *
 * Notes about the HUFF_DECODE macro:
 * 1. Near the end of the data segment, we may fail to get enough bits
 *    for a lookahead.  In that case, we do it the hard way.
 * 2. If the lookahead table contains no entry, the next code must be
 *    more than HUFF_LOOKAHEAD bits long.
 * 3. jpeg_huff_decode returns -1 if forced to suspend.
 */

#define HUFF_DECODE_DC(result,state,htbl,failaction,slowlabel) \
{ register t_sint16 nb, look; \
   register T_BOOL res; \
  if (bits_left < HUFF_LOOKAHEAD) { \
    if (! (res = jpeg_fill_bit_buffer(&state,get_buffer,bits_left, 0))) {failaction;} \
    get_buffer = state.get_buffer; bits_left = state.bits_left; \
    if (bits_left < HUFF_LOOKAHEAD) { \
      nb = 1; goto slowlabel; \
    } \
  } \
  look = PEEK_BITS(HUFF_LOOKAHEAD); \
  if ((nb = htbl->look_nbits[look]) != 0) { \
    DROP_BITS(nb); \
    result = htbl->look_sym[look]; \
  } else { \
    nb = HUFF_LOOKAHEAD+1; \
slowlabel: \
    if ((result=jpegdec_huff_decode_dc(&state,get_buffer,bits_left,htbl,nb)) < 0) \
	{ failaction; } \
    get_buffer = state.get_buffer; bits_left = state.bits_left; \
  } \
}



/*
 * Code for extracting next Huffman-coded symbol from input bit stream.
 * Again, this is time-critical and we make the main paths be macros.
 *
 * We use a lookahead table to process codes of up to HUFF_LOOKAHEAD bits
 * without looping.  Usually, more than 95% of the Huffman codes will be 8
 * or fewer bits long.  The few overlength codes are handled with a loop,
 * which need not be inline code.
 *
 * Notes about the HUFF_DECODE macro:
 * 1. Near the end of the data segment, we may fail to get enough bits
 *    for a lookahead.  In that case, we do it the hard way.
 * 2. If the lookahead table contains no entry, the next code must be
 *    more than HUFF_LOOKAHEAD bits long.
 * 3. jpeg_huff_decode returns -1 if forced to suspend.
 */

#define HUFF_DECODE_AC(result,state,htbl,failaction,slowlabel) \
{ register t_sint16 nb, look; \
   register T_BOOL res; \
  if (bits_left < HUFF_LOOKAHEAD) { \
    if (!(res = jpeg_fill_bit_buffer(&state,get_buffer,bits_left, 0))) {failaction;} \
    get_buffer = state.get_buffer; bits_left = state.bits_left; \
    if (bits_left < HUFF_LOOKAHEAD) { \
      nb = 1; goto slowlabel; \
    } \
  } \
  look = PEEK_BITS(HUFF_LOOKAHEAD); \
  if ((nb = htbl->look_nbits[look]) != 0) { \
    DROP_BITS(nb); \
    result = htbl->look_sym[look]; \
  } else { \
    nb = HUFF_LOOKAHEAD+1; \
slowlabel: \
    if ((result=jpegdec_huff_decode_ac(&state,get_buffer,bits_left,htbl,nb)) < 0) \
	{ failaction; } \
    get_buffer = state.get_buffer; bits_left = state.bits_left; \
  } \
}
void  btst_init_from_host_address_lib(tps_bitstream_buffer_opt p_bitstream,ts_t1xhv_bitstream_buf_pos* p_bitstream_buf_info,T_BOOL new_jpeg_image);

