/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifdef NMF_BUILD
#include <h264dec/arm_nmf/decoder.nmf>
#endif

#include "types.h"
#include "macros.h"
#include "settings.h"
#include "common_bitstream.h"



static const t_uint16 block_pattern[48] =
{(47<<8)+ 0,(31<<8)+16,(15<<8)+ 1,( 0<<8)+ 2,(23<<8)+ 4,(27<<8)+ 8,
 (29<<8)+32,(30<<8)+ 3,( 7<<8)+ 5,(11<<8)+10,(13<<8)+12,(14<<8)+15,
 (39<<8)+47,(43<<8)+ 7,(45<<8)+11,(46<<8)+13,(16<<8)+14,( 3<<8)+ 6,
 ( 5<<8)+ 9,(10<<8)+31,(12<<8)+35,(19<<8)+37,(21<<8)+42,(26<<8)+44,
 (28<<8)+33,(35<<8)+34,(37<<8)+36,(42<<8)+40,(44<<8)+39,( 1<<8)+43,
 ( 2<<8)+45,( 4<<8)+46,( 8<<8)+17,(17<<8)+18,(18<<8)+20,(20<<8)+24,
 (24<<8)+19,( 6<<8)+21,( 9<<8)+26,(22<<8)+28,(25<<8)+23,(32<<8)+27,
 (33<<8)+29,(34<<8)+30,(36<<8)+22,(40<<8)+25,(38<<8)+38,(41<<8)+41};



/**
 * \brief Extracts a bit string from the bitstream without updating the pointers.
 * \param p_b Pointer to the bit_buffer struct
 * \param n Length of the bit string in bits
 * \param p_val Pointer to the memory area where to store the extracted bit string.
 * \return 0 if successful, 1 otherwise
 * \author Filippo Santinello
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>23-06-2004&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * ShowBits reads a bit string of specified length without updating the position
 * inside the bitstream buffer. It also checks for the presence of the emulation
 * prevention byte and skips it if found.
 */
 
t_uint16 ShowBits(t_bit_buffer *p_b, t_sint16 n, t_uint32 *p_val)
{
    t_uint32 v, m;
    t_uint8 *tmp = p_b->addr;
    t_uint16 ost = p_b->os;

    /* Check if buffer end has been passed */
    if ((((t_sint32)(p_b->end-tmp)<<3)+ost) < n)
        return 1;
    
    //FIXED! JMG
	if (n==32) m=0xFFFFFFFFU;
	else m = (1<<n)-1;
    if (n<=ost)
    {
        ost -= n;
        v = ((t_uint32) *tmp) >> ost;
    }
    else
    {
        v = (t_uint32) *tmp;
        n -= ost;
        tmp += 1;
        ost = 8;
        while ((tmp <= p_b->end) && (n>=8))
        {
#if ON_THE_FLY_EMULATION_BYTE==1
            /* Detect emulation_prevention_three_byte and skip it if found */
            if ((t_sint32)(tmp-p_b->start)>=2)
            {
                if (*(tmp-2)==0 && *(tmp-1)==0 && *tmp==3)
                    tmp += 1;
            }
#endif

            v = (v<<8) | ((t_uint32) *tmp);
            n -= 8;
            tmp += 1;
        }
#if ON_THE_FLY_EMULATION_BYTE==1
        /* Detect emulation_prevention_three_byte and skip it if found */
        if ((tmp <= p_b->end) && ((t_sint32)(tmp-p_b->start)>=2))
        {
            if (*(tmp-2)==0 && *(tmp-1)==0 && *tmp==3)
                tmp += 1;
        }
#endif
        
        if (n > 0)
        {
            ost = 8-n;
            v = (v<<n) | (t_uint32)(*tmp>>ost);
        }
    }
    *p_val = v & m;
    return 0;
}




/**
 * \brief Updates the current position within the bitstream buffer.
 * \param p_b Pointer to the bit_buffer struct
 * \param n Number of bits to be flushed from the bitstream
 * \return 0 if successful, 1 otherwise
 * \author Filippo Santinello
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>23-06-2004&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * FlushBits updates the current position for bitstream reading and
 * flushes away the specified amount of bits. It also removes the
 * emulation prevention byte if found.
 */
 
t_uint16 FlushBits(t_bit_buffer *p_b, t_sint16 n)
{
    if (n<=p_b->os)
        p_b->os -= n;
    else
    {
        n -= p_b->os;
        p_b->addr += 1;
        p_b->os = 8;
        while (n>=8)
        {
#if ON_THE_FLY_EMULATION_BYTE==1
            /* Detect emulation_prevention_three_byte and skip it if found */
            if ((t_sint32)(p_b->addr-p_b->start)>=2)
            {
                if (*(p_b->addr-2)==0 && *(p_b->addr-1)==0 && *p_b->addr==3)
                  {  p_b->addr += 1; 
                     p_b->reserved++; // aeb_counter    
                     } 
            }
#endif
            /* Check if buffer end has been passed */
            if (((t_sint32)(p_b->end-p_b->addr)<<3)+p_b->os<n)
                return 1;
            n -= 8;
            p_b->addr += 1;
        }
#if ON_THE_FLY_EMULATION_BYTE==1
        /* Detect emulation_prevention_three_byte and skip it if found */
        if ((t_sint32)(p_b->addr-p_b->start)>=2)
        {
            if (*(p_b->addr-2)==0 && *(p_b->addr-1)==0 && *p_b->addr==3)
            {    p_b->addr += 1;
                p_b->reserved++; // aeb_counter   
                }
        }
#endif
        p_b->os = 8-n;
    }
    return 0;
}




/**
 * \brief Gets an Exp Golomb code from the bitstream.
 * \param p_b Pointer to the bit_buffer struct
 * \param mode Type of Exp Golomb code to get (unsigned, signed, block pattern or truncated)
 * \param p_val Pointer to the memory location where the Exp Golomb code will be written
 * \return 0 if successful, 1 otherwise
 * \author Filippo Santinello
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>23-06-2004&nbsp;</td><td>First release.</td></tr>
 * <tr><td valign=top>10-12-2004&nbsp;</td><td>Removed TE type because useless.</td></tr>
 * </table>
 * </dl>
 *
 * GetExpGolomb gets all possible Exp Golomb codes inside the bitstream. These can be
 * unsigned or signed integers, coded block patterns, or truncated codes. In case the coded
 * block pattern mode is selected, a 16-bit number is returned: the most significant 8 bits
 * contain the coded block pattern for INTRA 4x4 modes, the least significant 8 bits
 * contain the pattern for INTER modes.
 */
 
t_uint16 GetExpGolomb(t_bit_buffer *p_b, t_uint16 mode, t_sint32 *p_val)
{
    t_uint32 v;
    t_uint16 zero_bits, status;
    
    zero_bits = 0;
    do
    {
        status = ShowBits(p_b,1,&v);
        if (status!=0)
            return 1;
        if (v==0)
            zero_bits++;
        (void)FlushBits(p_b,1);
    } while (v==0);

    if (zero_bits!=0)
    {
        status = ShowBits(p_b,zero_bits,&v);
        if (status!=0)
            return 1;
        v = (1<<zero_bits)+v-1;
        (void)FlushBits(p_b,zero_bits);
    }
    else
        v = 0;
    
    switch(mode)
    {
        case EXPGOLOMB_UE:
            *p_val = (t_sint32)v;
        break;
        
        case EXPGOLOMB_SE:
            /* *p_val = ((v&1)==0) ? -((t_sint32)v>>1) : (((t_sint32)v+1)>>1);*/
            *p_val = ((v&1)==0) ? -(v>>1) : ((v+1)>>1);
        break;
        
        case EXPGOLOMB_ME_INTRA4:
            if (v>47)
                return 1;
            else
                *p_val = (t_sint32)(block_pattern[v] >> 8);
        break;
        
        case EXPGOLOMB_ME_INTER:
            if (v>47)
                return 1;
            else
                *p_val = (t_sint32)(block_pattern[v] & 0xff);
        break;
        
        default:
            return 1;
            // break;
    }
    
    return 0;
}


