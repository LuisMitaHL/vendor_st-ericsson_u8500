/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _VC1D_CO_BITSTREAM_H_
#define _VC1D_CO_BITSTREAM_H_

/*
 * Includes							       
 */
#include "vc1d_common.h"  /* for types definitions */
#include "types.h"
/*
 * Defines							       
 */
typedef enum
{
/** \brief all is ok!   */
    BTST_NO_ERROR = 0x0,
/** \brief end of bitstream data reached in current bitstream buffer   */
//    BTST_END_OF_DATA = 0x1,
/** \brief error opening bitstream file for reading */
    BTST_FILE_ERROR= 0x2,
/** \brief no sufficient memory to store bitstream */
    BTST_MALLOC_ERROR = 0x4,
/** \brief end of file reached */
    BTST_FILE_END =0x8,
/** \brief error detected in bitstream */
    BTST_BITSTREAM_ERROR =0x10,
/** \brief vlc error detected in bitstream */
    BTST_VLC_ERROR =0x20,
/** \brief sync marker error detected in bitstream */
    BTST_SYNC_ERROR= 0x40,
/** \brief findinf more than 64 coeff in a block   */
    BTST_COEFF_ERROR = 0x80

} t_btst_error_code;


typedef enum
{
    BTST_FORWARD,
    BTST_BACKWARD

} t_btst_direction;

typedef enum
{
    UPDATE_FROM_FILE_POS,
    UPDATE_FROM_BITSTREAM_POS
} t_update_mode;

/*
 * Types
 */

/** @{ \name bitstream handling structure definition */
/*---------------------------------------------------*/
/** \brief bitstream handling structure */
typedef struct s_bitstream_buffer 
{
    t_uint8 	*data;	/**< \brief buffer allocated to store bitstream data (bytes)   */
    t_uint8 	*pt_current_byte; /**< \brief pointer on current byte in bitstream data   */
    t_uint32 	index_current_bit; /**< \brief index of current bit in bitstream data from 0 to size_in_bits -1    */
    t_uint32      size_in_bits; /**< \brief bitstream size in bits   */
    t_btst_direction direction;
    t_update_mode      update_mode; /**< \brief sets wether the update of the bitstream content must be done
                                             from the last byte read in the bitstream or the last byte read in the
                                            file*/
}  ts_bitstream_buffer;

typedef ts_bitstream_buffer *tps_bitstream_buffer; /**< \brief pointer on ts_bitstream_buffer   */

/** @} end of bitstream handling structure definition */


/*
 * Functions
 */
void  btst_align_byte(tps_bitstream_buffer p_bitstream);
t_bool  btst_is_byte_aligned(tps_bitstream_buffer p_bitstream);
t_uint16 btst_showbits(tps_bitstream_buffer p_bitstream,t_uint16 n);
t_uint32 btst_showbits32(tps_bitstream_buffer p_bitstream,t_uint16 n);
void  btst_flushbits(tps_bitstream_buffer p_bitstream, t_uint16 n);
t_btst_error_code btst_flushbits32(tps_bitstream_buffer p_bitstream,t_uint32 n);
t_uint16  btst_getbits(tps_bitstream_buffer p_bitstream, t_uint16 n);
t_uint32 btst_getbits32(tps_bitstream_buffer p_bitstream, t_uint16 n);
t_uint32  btst_get_bit_index(tps_bitstream_buffer p_bitstream);
void btst_print_32bits(tps_bitstream_buffer p_bitstream);
t_address btst_get_current_byte_address(tps_bitstream_buffer p_bitstream);
t_uint32 btst_get_current_bit_offset(tps_bitstream_buffer p_bitstream);
t_uint32 btst_get_current_byte_index(tps_bitstream_buffer p_bitstream);
t_uint32  btst_get_byte_size(tps_bitstream_buffer p_bitstream);
t_uint32 btst_get_bits_left_count(tps_bitstream_buffer p_bitstream);

/* MC: duplicated definitions to be used by host part of the decoder */
void  arm_btst_align_byte(tps_bitstream_buffer p_bitstream);
t_bool  arm_btst_is_byte_aligned(tps_bitstream_buffer p_bitstream);
t_uint16 arm_btst_showbits(tps_bitstream_buffer p_bitstream,t_uint16 n);
t_uint32 arm_btst_showbits32(tps_bitstream_buffer p_bitstream,t_uint16 n);
void  arm_btst_flushbits(tps_bitstream_buffer p_bitstream, t_uint16 n);
t_btst_error_code arm_btst_flushbits32(tps_bitstream_buffer p_bitstream,t_uint32 n);
t_uint16  arm_btst_getbits(tps_bitstream_buffer p_bitstream, t_uint16 n);
t_uint32 arm_btst_getbits32(tps_bitstream_buffer p_bitstream, t_uint16 n);
t_uint32  arm_btst_get_bit_index(tps_bitstream_buffer p_bitstream);
void arm_btst_print_32bits(tps_bitstream_buffer p_bitstream);
t_address arm_btst_get_current_byte_address(tps_bitstream_buffer p_bitstream);
t_uint32 arm_btst_get_current_bit_offset(tps_bitstream_buffer p_bitstream);
t_uint32 arm_btst_get_current_byte_index(tps_bitstream_buffer p_bitstream);
t_uint32  arm_btst_get_byte_size(tps_bitstream_buffer p_bitstream);
t_uint32 arm_btst_get_bits_left_count(tps_bitstream_buffer p_bitstream);

/* MC: double duplicated definitions to be used by host part of the decoder */
void  testapp_btst_align_byte(tps_bitstream_buffer p_bitstream);
t_bool  testapp_btst_is_byte_aligned(tps_bitstream_buffer p_bitstream);
t_uint16 testapp_btst_showbits(tps_bitstream_buffer p_bitstream,t_uint16 n);
t_uint32 testapp_btst_showbits32(tps_bitstream_buffer p_bitstream,t_uint16 n);
void  testapp_btst_flushbits(tps_bitstream_buffer p_bitstream, t_uint16 n);
t_btst_error_code testapp_btst_flushbits32(tps_bitstream_buffer p_bitstream,t_uint32 n);
t_uint16  testapp_btst_getbits(tps_bitstream_buffer p_bitstream, t_uint16 n);
t_uint32 testapp_btst_getbits32(tps_bitstream_buffer p_bitstream, t_uint16 n);
t_uint32  testapp_btst_get_bit_index(tps_bitstream_buffer p_bitstream);
void testapp_btst_print_32bits(tps_bitstream_buffer p_bitstream);
t_address testapp_btst_get_current_byte_address(tps_bitstream_buffer p_bitstream);
t_uint32 testapp_btst_get_current_bit_offset(tps_bitstream_buffer p_bitstream);
t_uint32 testapp_btst_get_current_byte_index(tps_bitstream_buffer p_bitstream);
t_uint32  testapp_btst_get_byte_size(tps_bitstream_buffer p_bitstream);
t_uint32 testapp_btst_get_bits_left_count(tps_bitstream_buffer p_bitstream);

#endif /* _VC1D_CO_BITSTREAM_H_ */
