/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */



#ifndef _VC1D_SVA_VLC_H_
#define _VC1D_SVA_VLC_H_

/*------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------*/

#include "vc1d_common.h"
#include "vc1d_target.h"
#include "vc1d_co_bitstream.h"

/*------------------------------------------------------------------------
 * Defines                                                            
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Types                                                            
 *----------------------------------------------------------------------*/


/*------------------------------------------------------------------------
 * Functions                                                            
 *----------------------------------------------------------------------*/

t_uint16 vlc_read_vlc_value(tps_bitstream_buffer p_bitstream,
                            const ts_vlc_entry  *p_vlc_table);




static t_uint16 vlc_read_vlc_value_lut(tps_bitstream_buffer p_bitstream,
                                                const ts_vlc_entry  * p_vlc_table,
                                                const t_uint16 * p_vlc_lut_table)
{
    t_uint16 showed_bits16;

    /*We start with a showbit of 10 and hope to find a valid entry in the lut */
#if 0
    printf("VLC: Reading using LUT\n");
#endif
    showed_bits16 = btst_showbits(p_bitstream,10);
    if (p_vlc_lut_table[showed_bits16] != 0)
    {
        /* 4 lsb of the LUT are the size of the huffman code */
        /* 12 msb are the symbol coded with the huffman code */
#if 0
        printf("\t 10 bits showed (0x%x) found in LUT: 0x%x",showed_bits16,p_vlc_lut_table[showed_bits16]);
        printf("\tVLC value = %d (get %d bits) \n",p_vlc_lut_table[showed_bits16] >> 4,(p_vlc_lut_table[showed_bits16] & 0xf));
#endif
///        btst_flushbits(p_bitstream,(p_vlc_lut_table[showed_bits16] & 0xf));
        btst_flushbits(p_bitstream,(VLC_LUT_SIZE(p_vlc_lut_table[showed_bits16])));
//        return ((p_vlc_lut_table[showed_bits16] >> 4));
        return ((VLC_LUT_VAL(p_vlc_lut_table[showed_bits16])));
    }
    else
    {
#if 0 
        printf("\t LUT failed, jumping to incremental search\n");
#endif
        /* We did not find an entry in the LUT table, */
        /* let's parse the vlc table the hard way */
        /* We jump the the basic vlc function */
        return (vlc_read_vlc_value(p_bitstream,p_vlc_table));
    }
    
}


#endif /* _VC1D_SVA_VLC_H_ */
