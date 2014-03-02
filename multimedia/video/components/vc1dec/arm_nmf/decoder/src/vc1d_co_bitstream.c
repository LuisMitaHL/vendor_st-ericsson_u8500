/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


/*
 * Includes							       
 */
#include "vc1d_co_bitstream.h"
#ifdef NMF_BUILD
#include <vc1dec/arm_nmf/decoder.nmf>
#endif


/*------------------------------------------------------------------------
 * Defines                                                            
 *----------------------------------------------------------------------*/
#if defined(PRINT_BITSTREAM_POS) || defined(PRINT_ALL)
#define PRINTF(value) if (Dbg) printf value /**< \brief debug printf function: use double (( when calling   */
#else
#define PRINTF(value)
#endif
/*
 * Functions							       
 */

/*****************************************************************************/
/**
 * \brief 	return byte size of bitstream
 * \author 	jean-marc volle
 * \param 	p_bitstream pointer on bitstream struct
 * \return 	bitstream data byte size (rounded up if necessary)
 *  
 */
/*****************************************************************************/
t_uint32  arm_btst_get_byte_size(tps_bitstream_buffer p_bitstream)
{
    t_uint32 byte_size;
    byte_size = p_bitstream->size_in_bits / 8;
    if (p_bitstream->size_in_bits % 8 != 0) byte_size++;
        
    return (t_uint32) byte_size;
}


/*****************************************************************************/
/**
 * \brief 	check if current bit position is byte aligned
 * \author 	jean-marc volle
 * \param   p_bitstream pointer on current bitstrean_buffer
 * \return 	TRUE if current bit in bistream is aligned on byte boundary,
 *          FALSE  otherwise
 * 
 */
/*****************************************************************************/
t_bool arm_btst_is_byte_aligned(tps_bitstream_buffer p_bitstream)
{
    if ((p_bitstream->index_current_bit % 8) == 0) return TRUE;
    else return FALSE;

}


/*****************************************************************************/
/**
 * \brief 	align bit pointer on next byte boundary if not current aligned
 * \author 	jean-marc volle
 * \param 	p_bitstream pointer on bitstream struct
 * 
 */
/*****************************************************************************/
void  arm_btst_align_byte(tps_bitstream_buffer p_bitstream)
{
    t_uint16 offset;
    
    offset = p_bitstream->index_current_bit % 8;
    if (offset == 0) return ;
    
    offset = 8 - offset;
    arm_btst_flushbits(p_bitstream,offset);

} /* end of btst_align_byte */


/*****************************************************************************/
/**
 * \brief 	show next n bits in bitstream
 * \author 	jean-marc volle
 * \param 	p_bitstream pointer on bitstream struct
 * \param 	n number of bits to show
 * \return 	showed bits
 *
 * This function shows the next bits (up to 16) in the bitstream. The current
 * bit index is not changed.
 */
/*****************************************************************************/
t_uint16 arm_btst_showbits(tps_bitstream_buffer p_bitstream,t_uint16 n)
{    
    t_uint32 nb_of_bytes_to_read;
    t_uint32 res;
    t_uint8 *pt_byte_to_read;
    t_uint32 i;
    t_uint32 offset_first_byte;
    t_uint32 offset_last_byte;
    t_uint16 current_byte; /*because char >> 8 gives 0xff!*/
//    t_uint32 index_last_bit;  /*index of the last bit to be read */

    ASSERT(n <= 16);

    /* can we access the bits asked ?*/
/*      index_last_bit = p_bitstream->index_current_bit + n;  */

/*     if (index_last_bit > p_bitstream->size_in_bits) return BTST_END_OF_DATA; */

    nb_of_bytes_to_read = 
        ((p_bitstream->index_current_bit + n)>>3) 
            - (p_bitstream->index_current_bit >> 3)+1 ;
    offset_first_byte = p_bitstream->index_current_bit % 8;
    offset_last_byte = (p_bitstream->index_current_bit+n) % 8;

    pt_byte_to_read = p_bitstream->pt_current_byte;

    res = 0x00000000UL;

    for (i=0;i<nb_of_bytes_to_read;i++)
    {
        current_byte = *pt_byte_to_read ;
        current_byte = current_byte & 0x00ff ;

        if (i==0) 
        {  
            current_byte = current_byte & (0x00ff>>offset_first_byte);
        }
        if (i != nb_of_bytes_to_read -1) 
        {
            res = (res << 8) |(t_uint32) current_byte;
        }
        else 
        {
            current_byte = current_byte >> (8-offset_last_byte);
            res = (res << offset_last_byte) | current_byte;
        }
        pt_byte_to_read++;
    }

//    *p_bits = (t_uint16) res;
//    return BTST_NO_ERROR;
    return (t_uint16) res;
} /* end of btst_showbits */

/*****************************************************************************/
/**
 * \brief 	show next n bits in bitstream
 * \author 	jean-marc volle
 * \param 	p_bitstream pointer on bitstream struct
 * \param 	n number of bits to show
 * \return 	bits showed
 *
 * This function shows the next bits (up to 32) in the bitstream. The current
 * bit index is not changed.
 */
/*****************************************************************************/
t_uint32 arm_btst_showbits32(tps_bitstream_buffer p_bitstream,
                                  t_uint16 n)
{
    t_uint32 nb_of_bytes_to_read;
    t_uint32 res;
    t_uint8 *pt_byte_to_read;
    t_uint32 i;
    t_uint32 offset_first_byte;
    t_uint32 offset_last_byte;
    t_uint16 current_byte; /*because char >> 8 gives 0xff!*/
//    t_uint32 index_last_bit;  /*index of the last bit to be read */

    ASSERT(n>=16);
    ASSERT(n<=32);

    /* can we access the bits asked ?*/
//    index_last_bit = p_bitstream->index_current_bit + n;

//    if ( index_last_bit > p_bitstream->size_in_bits) return BTST_END_OF_DATA;

    nb_of_bytes_to_read = 
        ((p_bitstream->index_current_bit + n)>>3) 
            - (p_bitstream->index_current_bit>>3)+1;
    offset_first_byte = p_bitstream->index_current_bit % 8;
    offset_last_byte = (p_bitstream->index_current_bit+n) % 8;

    pt_byte_to_read = p_bitstream->pt_current_byte;


    res = 0x00000000UL;

    for (i=0;i<nb_of_bytes_to_read;i++)
    {
        current_byte = *pt_byte_to_read ;
        current_byte = current_byte & 0x00ff ;

        if (i==0) 
        {  
            current_byte = current_byte & (0x00ff>>offset_first_byte);
        }
        if (i != nb_of_bytes_to_read -1) 
        {
            res = (res << 8) |(t_uint32) current_byte;
        }
        else 
        {
            current_byte = current_byte >> (8-offset_last_byte);
            res = (res << offset_last_byte) | current_byte;
        }
        pt_byte_to_read++;
    }

//    *p_bits = (t_uint32) res;
//    return BTST_NO_ERROR;
    return res;
} /* end of btst_showbits */


/*****************************************************************************/
/*
 * \brief 	advance the Bitstream current bit index and the current byte
 * \author 	jean-marc volle
 * \param 	p_bitstream pointer on bitstream struct
 * \param 	n increment to add to the bit index
 * \return  btst error code
 *  
 * This function increments the bit index in the bitstream. It also update the 
 * last_read_byte for byte stuffing removal.
 */
/*****************************************************************************/
void arm_btst_flushbits(tps_bitstream_buffer p_bitstream,t_uint16 n)
{
/*     if (p_bitstream->index_current_bit + n > p_bitstream->size_in_bits)  */
/*     { */
/*         return BTST_END_OF_DATA; */
/*     } */
    p_bitstream->index_current_bit += n;
    p_bitstream->pt_current_byte = 
        p_bitstream->data + (p_bitstream->index_current_bit >>3);

//    return BTST_NO_ERROR;
}

/*****************************************************************************/
/*
ONLY FOR DEBUGGING
 */
/*****************************************************************************/
/* t_btst_error_code btst_flushbits32(tps_bitstream_buffer p_bitstream,t_uint32 n) */
/* { */
/*     if (p_bitstream->index_current_bit + n > p_bitstream->size_in_bits)  */
/*     { */
/*         return BTST_END_OF_DATA; */
/*     } */
/*     p_bitstream->index_current_bit += n; */
/*     p_bitstream->pt_current_byte =  */
/*         p_bitstream->data + (p_bitstream->index_current_bit >>3); */

/*     return BTST_NO_ERROR; */
/* } */

/*****************************************************************************/
/**
 * \brief 	get next n bits in the bitstream
 * \author 	jean-marc volle
 * \param 	p_bitstream pointer on bitstream struct
 * \param 	n number of bits to get n<=16
 * \return      bits read
 *  
 * This function returns the next n bits in the bitstream and increases the
 * the bit index. It is equivalent to showbits (n) then flushbits(n)
 */
/*****************************************************************************/
t_uint16 arm_btst_getbits(tps_bitstream_buffer p_bitstream,
                               t_uint16 n)
{
    t_uint16 res;
    ASSERT(n<=16);
    res  = arm_btst_showbits(p_bitstream,n);
//    PRINTF(("get %d bits = 0x%x, from pos = %d\n",n,*p_bits,btst_get_bit_index(p_bitstream)));
    arm_btst_flushbits(p_bitstream,n);

    return res;
}



/*****************************************************************************/
/**
 * \brief 	get next n bits in the bitstream
 * \author 	jean-marc volle
 * \param 	p_bitstream pointer on bitstream struct
 * \param 	n number of bits to get  16<=n<=32
 * \return      bits read
 *  
 * This function returns the next n bits in the bitstream and increases the
 * the bit index. It is equivalent to showbits32 (n) then flushbits(n)
 */
/*****************************************************************************/
t_uint32 arm_btst_getbits32(tps_bitstream_buffer p_bitstream,
                        t_uint16 n)
{
    t_uint32 res;
    res  = arm_btst_showbits32(p_bitstream,n);
    arm_btst_flushbits(p_bitstream,n);
    return res;
}

/*****************************************************************************/
/**
 * \brief 	return the index of the current bit in the bitstream handling 
 *          structure
 * \author 	jean-marc volle
 * \param 	p_bitstream pointer on bitstream
 * \return  index of current bit
 *  
 * 
 */
/*****************************************************************************/
t_uint32 arm_btst_get_bit_index(tps_bitstream_buffer p_bitstream)
{
    return(p_bitstream->index_current_bit);
}

/*****************************************************************************/
/**
 * \brief 	print the next 32 bits from the current bitstream struct 
 *          (debugging purpose)
 * \author 	jean-marc volle
 * \param 	p_bitstream pointer on bitstream
 *  
 * 
 */
/*****************************************************************************/
void arm_btst_print_32bits(tps_bitstream_buffer p_bitstream)
{
    NMF_LOG("Common: Current add=0x%lx showed 32bits = 0x%2x%2x%2x%2x bit pos = %lu byte offset = 0x%lx \n",
           (t_address)p_bitstream->pt_current_byte,
           *(p_bitstream->pt_current_byte),
           *(p_bitstream->pt_current_byte+1),
           *(p_bitstream->pt_current_byte+2),
           *(p_bitstream->pt_current_byte+3),
           p_bitstream->index_current_bit,
           arm_btst_get_current_byte_index(p_bitstream));

}


/*****************************************************************************/
/**
 * \brief 	return adress of current byte in bitstream
 * \author 	jean-marc volle
 * \param 	p_bitstream pointer on bitstream struct
 * \return 	address of current byte in bitstream
 * 
 */
/*****************************************************************************/
t_address arm_btst_get_current_byte_address(tps_bitstream_buffer p_bitstream)
{

    return (t_address) p_bitstream->pt_current_byte;

}

/*****************************************************************************/
/**
 * \brief 	return offset of current bit in current byte
 * \author 	jean-marc volle
 * \param 	p_bitstream pointer on bitstream
 * \return  offset of current bit in current byte
 * 
 */
/*****************************************************************************/
t_uint32 arm_btst_get_current_bit_offset(tps_bitstream_buffer p_bitstream)
{

    return (p_bitstream->index_current_bit%8);

}

/*****************************************************************************/
/**
 * \brief 	return the number of bits remaining in the bitstream
 * \author 	jean-marc volle
 * \param 	p_bitstream pointer on bitstream
 * \return   number of bits remaining
 * 
 */
/*****************************************************************************/
t_uint32 arm_btst_get_bits_left_count(tps_bitstream_buffer p_bitstream)
{

    return (p_bitstream->size_in_bits - p_bitstream->index_current_bit);

}


/*****************************************************************************/
/**
 * \brief 	return byte index of current byte in bitstream
 * \author 	jean-marc volle
 * \param 	p_bitstream pointer on bitstream struct
 * \return 	index of the current byte in bitstream
 *
 */
/*****************************************************************************/
t_uint32 arm_btst_get_current_byte_index(tps_bitstream_buffer p_bitstream)
{

    return (arm_btst_get_bit_index(p_bitstream)/8);

}

