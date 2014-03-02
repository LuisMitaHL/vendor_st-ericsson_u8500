/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


/*------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------*/

#include "vc1d_ht_bitstream.h"
#include "vc1d_target.h"

/*------------------------------------------------------------------------
 * Defines                                                             
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Variables                                                             
 *----------------------------------------------------------------------*/

/* Debug: */

#if defined(PRINT_HT_BITSTREAM) | defined(PRINT_ALL)
#define PRINTF(value) if (Dbg) printf value /**< \brief debug printf function: use double (( when calling   */
#else
#define PRINTF(value)
#endif


/*****************************************************************************/
/**
 * \brief 	return byte address of first byte of bitstream data
 * \author 	jean-marc volle
 * \param 	p_bitstream pointer on bitstream struct
 * \return 	address 
 *  
 */
/****************************************************************************/
t_address  btst_get_start_address(tps_bitstream_buffer p_bitstream)
{
    return (t_address) p_bitstream->data;
}


/*****************************************************************************/
/**
 * \brief 	update bit/byte indexes in bitstream buffer
 * \author 	jean-marc volle
 * \param 	p_bitstream pointer on bitstream buffer
 * \param 	pos address of byte pointer to set
 * \param 	offset bit offset from byte address
 *
 */
/*****************************************************************************/
void btst_update_from_sva_pos(tps_bitstream_buffer p_bitstream,
                                t_address pos, t_uint32 offset)
{
    p_bitstream->pt_current_byte   = (t_uint8*) pos;
    p_bitstream->index_current_bit = 8*(pos-(t_uint32)(p_bitstream->data))
                                     + offset;

}

/*****************************************************************************/
/**
 * \brief 	save the current position in the bitstream
 * \author 	jean-marc volle
 * \param 	p_bitstream pointer on bitstream buffer
 * \param 	p_pos index to save
 *
 */
/*****************************************************************************/
void btst_save_pos(tps_bitstream_buffer p_bitstream,
                           t_uint32 *p_pos)
{
    *p_pos = p_bitstream->index_current_bit;
}

/*****************************************************************************/
/**
 * \brief 	set a new position (bit index) in the bitstream
 * \author 	jean-marc volle
 * \param 	p_bitstream pointer on bitstream buffer
 * \param 	pos index to set
 *
 */
/*****************************************************************************/
void btst_set_pos(tps_bitstream_buffer p_bitstream,
                  t_uint32 pos)
{
    ASSERT(pos<p_bitstream->size_in_bits);

    p_bitstream->index_current_bit = pos;
    p_bitstream->pt_current_byte   = p_bitstream->data + (pos / 8);
}


/****************************************************************************/
/**
 * \brief 	read up to 4 bytes in little endian format
 * \author 	jean-marc volle
 * \param 	p_bitstream: pointer on bitstream struct
 * \param 	nb_bytes number of bytes to read: <=4
 * \param 	p_val: returned value
 * \return 	 bitstream error code
 *  
 * 
 **/
/****************************************************************************/

t_uint32 btst_get_bytes_le(tps_bitstream_buffer p_bitstream,
                                    t_sint16 nb_bytes)

{
    t_sint16 i;
    t_uint16 byte;
//    t_btst_error_code err = BTST_NO_ERROR;
    t_uint32 res;

    ASSERT (nb_bytes <5);
    
    res = 0L;

    for (i=0;i<nb_bytes;i++)
    {
        byte = arm_btst_getbits(p_bitstream,8);
        res |= ((t_uint32) byte) <<(8*i);
    }

    
    return res;

}

/****************************************************************************/
/**
 * \brief 	show up to 4 bytes in little endian format
 * \author 	jean-marc volle
 * \param 	p_bitstream: pointer on bitstream struct
 * \param 	nb_bytes number of bytes to read: <=4
 * \param 	p_val: returned value
 * \return 	 bitstream error code
 *  
 * 
 **/
/****************************************************************************/

t_uint32 btst_show_bytes_le(tps_bitstream_buffer p_bitstream,
                            t_sint16 nb_bytes)
{
    t_sint16 i;
    t_uint8 *pt_byte;
    t_uint32 res;

    ASSERT (nb_bytes <5);
    ASSERT( btst_is_byte_aligned(p_bitstream));

//    if (p_bitstream->index_current_bit + 8*nb_bytes > p_bitstream->size_in_bits) return BTST_END_OF_DATA;


    res = 0L;
    pt_byte = p_bitstream->pt_current_byte;

    for (i=0;i<nb_bytes;i++)
    {
        res |= ((t_uint32) *pt_byte) <<(8*i);
        pt_byte++;
    }

/*    Note: we do not update any of the p_bitstream fields because we just did a show */

    
    return res;

}
