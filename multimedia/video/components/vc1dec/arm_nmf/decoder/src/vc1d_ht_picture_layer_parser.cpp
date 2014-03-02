/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


/*------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------*/

#ifdef NMF_BUILD
#include <vc1dec/arm_nmf/decoder.nmf>
#endif
#include "vc1d_common.h"
#include "vc1d_co_bitstream.h"

#include "vc1d_ht_bitstream.h"
#include "vc1d_ht_picture_layer_parser.h"

/*------------------------------------------------------------------------
 * Defines                                                            
 *----------------------------------------------------------------------*/

#if  0
#define PRINTF(value) if (1)  NMF_LOG value /**< \brief debug printf function: use double (( when calling   */
#else
#define PRINTF(value)
#endif

/*------------------------------------------------------------------------
 * Types                                                            
 *----------------------------------------------------------------------*/


/*------------------------------------------------------------------------
 * Constants                                                            
 *----------------------------------------------------------------------*/
/* Table used to read BFRACTION value */
static const ts_vlc_entry c_vlc_b_fraction_table[24] =
{
    {0, 23, 7},
    {0, 3, 0},      {1, 3, 1},      {2, 3, 2},      {3, 3, 3},
    {4, 3, 4},      {5, 3, 5},      {6, 3, 6},      {112, 7, 7},
    {113, 7, 8},    {114, 7, 9},    {115, 7, 10},   {116, 7, 11},
    {117, 7, 12},   {118, 7, 13},   {119, 7, 14},   {120, 7, 15},
    {121, 7, 16},   {122, 7, 17},   {123, 7, 18},   {124, 7, 19},
    {125, 7, 20},   {126, 7, 21},   {127, 7, 22}
};

/** Description:
 * Table of BFraction numerator and denominator values
 * \note This table is also defined on the sva side
 */
#define B_FRACTION_TABLE_SIZE 23


/*------------------------------------------------------------------------
 * Functions (locals)                                                            
 *----------------------------------------------------------------------*/
static volatile t_uint16 thirty_two=32;

static t_uint16 vlc_read_vlc_value(tps_bitstream_buffer p_bitstream,
                            const ts_vlc_entry  *p_vlc_table)
{
    t_uint16 cnt;//;,end_loop;
    t_uint32 showed_bits;
    t_uint16 nb_bits;

/*     In any vlc_table, entry 0 has a special meaning: */
/*    - bits   = position of last entry with lenght<16 or 0; */
/*    - length = Number of codes in the array (Length of array - 1) */
/*    - value  = Maximum code length */

    ASSERT(p_vlc_table->length > 0);
    ASSERT(p_vlc_table->value > 0);

    PRINTF(("VLC: Reading \n"));
    

    showed_bits = arm_btst_showbits32(p_bitstream,32);

    PRINTF(("\t Showed %d bits =0x%Lx\n",32,showed_bits));

    nb_bits = thirty_two;
    //ASSERT_HW_LOOP(p_vlc_table->length);

    for (cnt = 1;cnt <= p_vlc_table->length;cnt++)
    {
        
#if 0
        PRINTF(("\tVLC: showed %d bits = 0x%lx\n",p_vlc_table[cnt].length,(showed_bits>>(nb_bits - p_vlc_table[cnt].length))));
#endif
        if ((showed_bits>>(nb_bits - p_vlc_table[cnt].length)) == p_vlc_table[cnt].bits) 
        {
            arm_btst_flushbits(p_bitstream,p_vlc_table[cnt].length);
            PRINTF(("\tVLC value = %d (get %d bits= 0x%lx) \n",p_vlc_table[cnt].value,p_vlc_table[cnt].length,(showed_bits>>(nb_bits - p_vlc_table[cnt].length))));
            return  p_vlc_table[cnt].value;
        }
    } /* end of for loop */


    /* Reached only if no code has been found */
    NMF_PANIC("ERROR: VLC reading error\n");
    //exit(-1); // we exit on the host if we have an error
    return 0;
}

/*------------------------------------------------------------------------
 * Functions (exported)                                                            
 *----------------------------------------------------------------------*/
/****************************************************************************/
/**
 * \brief 	read the picture type from the begining of the picture header data
 * \author 	jean-marc volle
 * \param 	p_seq_param: sequence parameters
 * \param 	p_picture_type: pointer on picture type to update
 *  
 * 
 **/
/****************************************************************************/

t_btst_error_code  btpar_parse_picture_type(tps_bitstream_buffer p_bitstream,
                                           tps_sequence_parameters p_seq_param,
                                           t_vc1_picture_type *p_picture_type)
{
    t_btst_error_code err = BTST_NO_ERROR;
    t_uint16 vlc_value;
    t_uint16 read_bits;

    

    /* First check if the frame is skipped */
    if (arm_btst_get_byte_size(p_bitstream) <= 8)
    {
        *p_picture_type =PICTURE_TYPE_SKIPPED;
        PRINTF(("\tPicture is skipped\n"));
        
        return err;
    }

    /* 1- INTERPFRM if frame FINTERPFLAG is set at sequence level */
    /* ---------------------------------------------------------- */
    if (p_seq_param->frame_interpolation_enabled)
    {
         arm_btst_flushbits(p_bitstream,1);
    } 
    else 
    {

    }
    
    /* 2- FRMCNT (not used for decoding) */
    /* --------------------------------- */
    arm_btst_flushbits(p_bitstream,2);
    

    /* 3- RANGEREDFRM if RANGERED is set at sequence level */
    /* --------------------------------------------------- */
    if (p_seq_param->rangered_enabled)
    {
        arm_btst_flushbits(p_bitstream,1);

    }
    else
    {

    }


    
    /* 4- PTYPE */
    /* -------- */
    if (p_seq_param->max_b_frames == 0)
    {
        read_bits = arm_btst_getbits(p_bitstream,1);
        *p_picture_type = (read_bits==0?PICTURE_TYPE_I : PICTURE_TYPE_P);
        PRINTF(("\tPTYPE (Picture Type): %s\n", (read_bits==0?"I":"P")));
    }
    else
    {
        read_bits= arm_btst_showbits(p_bitstream,2);
        switch(read_bits)
        {
        case 1: 
            *p_picture_type = PICTURE_TYPE_I;
            arm_btst_flushbits(p_bitstream,2);
            PRINTF(("\tPTYPE (Picture Type): I\n"));
            break;
        case 0:
            *p_picture_type = PICTURE_TYPE_B;
        /* Note: in fact we do not know now if the picture is B or BI, we need to read the */
        /* B-fraction to differentiate the 2 types */

            arm_btst_flushbits(p_bitstream,2);
            PRINTF(("\tPTYPE (Picture Type): B\n"));
            break;
        default: /* (0b10 or 0bb11) */
            *p_picture_type = PICTURE_TYPE_P;
            arm_btst_flushbits(p_bitstream,1);
            PRINTF(("\tPTYPE (Picture Type): P\n"));            
        }
    }

    /* 5- BFRACTION (if frame is a B frame) */
    /* ------------------------------------ */

    if (*p_picture_type == PICTURE_TYPE_B)
    {
        vlc_value = vlc_read_vlc_value(p_bitstream,c_vlc_b_fraction_table);
#if 0
        if (err & BTST_VLC_ERROR) 
        {
            PRINTF(("ERROR skipping b fraction vlc\n"));
            return BTST_VLC_ERROR;
        }
#endif
        ASSERT(vlc_value <B_FRACTION_TABLE_SIZE);
        
        if (vlc_value == B_FRACTION_TABLE_SIZE -2)
        {
            PRINTF(("ERROR forbidden b fraction vlc\n"));
            return BTST_VLC_ERROR;
        }

        if (vlc_value == B_FRACTION_TABLE_SIZE -1)
        {
            /* The picture is a BI frame */
            *p_picture_type = PICTURE_TYPE_BI;
        }
        
    } /* end of BFRACTION reading */

    
    return err;


} /* end of btpar_parse_pitcure_type */
