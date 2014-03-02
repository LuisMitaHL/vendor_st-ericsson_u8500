/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 

#include "types.h"
#include "hamac_types.h"
#include "macros.h"
#include "settings.h" 
#include "host_sch.h"
#include "host_block_info.h"




/**
 * \brief Scene change detection for P frames
 * \param mb_count Number of decoded macroblocks 
 * \param mb_intra Number of decoded intra macroblocks
 * \param p_hamac Pointer to structure with hamac concealment info.
 * \return 1 if scene change detected, 0 otherwise.
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>08-04-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function estimates scene change detection for P frames. 
 */
 
t_uint16 SceneChangeP(t_uint16 mb_count, t_uint16 mb_intra, t_hamac_conc_info *p_hamac)
{
    return (mb_count > (p_hamac->PicSizeInMbs / 3)) & (mb_intra > (mb_count / 2));
}



/**
 * \brief Scene change detection for I frames
 * \param p_hamac Pointer to structure with hamac concealment info.
 * \return 1 if scene change detected, 0 otherwise.
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>08-04-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function estimates scene change detection for I frames. 
 */
 
t_uint16 SceneChangeI(t_hamac_conc_info *p_hamac)
{
    t_uint16 i, k, m, nmb; 
    t_uint16 change_xy, max_inc, inc, dir;
    t_uint16 mb_pos[4][2];
    t_uint16 good_pos[16][2];
    t_uint16 width = p_hamac->pic_width_in_mbs_minus1+1;
    t_uint16 pel_width = (p_hamac->pic_width_in_mbs_minus1+1) << 3;
    t_uint16 height = p_hamac->pic_height_in_map_units_minus1+1;
    t_uint16 *ptr1, *ptr2;
    t_uint32 sad = 0;
        
    mb_pos[0][0] = mb_pos[2][0] = width >> 2;
    mb_pos[1][0] = mb_pos[3][0] = 3 * (width >> 2);
    
    mb_pos[0][1] = mb_pos[1][1] = height >> 2;
    mb_pos[2][1] = mb_pos[3][1] = 3 * (height >> 2);
    
    nmb = 0;
    
    for (i = 0; i < 4; i++)
    {
        max_inc = 1;
        inc = 0;
        dir = (i < 2) ? 1 : 0;
        change_xy = 1;
        k = 0;
         
        while ((k < 4) && (inc < MIN(width >> 2, height >> 2)))
        {
            if (!NotDecoded(mb_pos[i][1]*width+mb_pos[i][0], (t_block_info *)p_hamac->b_info))
            {
                good_pos[nmb][0] = mb_pos[i][0];
                good_pos[nmb][1] = mb_pos[i][1];
                nmb++;
                k++;
            }
            
            if (change_xy) /* moving along x */
            {
                if (dir) 
                    mb_pos[i][0]--;
                else     
                    mb_pos[i][0]++;
                
                inc++;
                
                if (inc == max_inc)
                {
                    change_xy = 0;
                    inc = 0;
                }
            }
            else           /* moving along y */
            {
                if (dir) 
                    mb_pos[i][1]--;
                else      
                    mb_pos[i][1]++;
                    
                inc++;
                
                if (inc == max_inc)
                {
                    change_xy = 1;
                    max_inc++;
                    inc = 0;
                    dir ^= 1; 
                }
            }
        }                 
    }
    
    
    /* Not enough macroblocks */
    if (nmb < 12)
    {
#if VERB_ERR_CONC == 1
        printf("\nNot enough macroblocks for scene change detection.\n");
#endif
        return 0;
    }

#if VERB_SCD == 1
    printf("Scene change evaluated on mbs:\n");
    for (i = 0; i < nmb; i++)
    {
        printf("%i %i\n", good_pos[i][0], good_pos[i][1]);   
    }
#endif
  
    for (i = 0; i < nmb; i++)
    {
        ptr1 = (t_uint16 *)p_hamac->curr_frame[0] + good_pos[i][1]*pel_width*16 + (good_pos[i][0] << 3);
        ptr2 = (t_uint16 *)p_hamac->ref_frame[0] + good_pos[i][1]*pel_width*16 + (good_pos[i][0] << 3);

        for (k = 0; k < 16; k++)
        {
            for (m = 0; m < 8; m++)
            {
                sad += ABS(LEFT_PEL(*ptr1) - LEFT_PEL(*ptr2));
                sad += ABS(RIGHT_PEL(*ptr1) - RIGHT_PEL(*ptr2));

                ptr1++;
                ptr2++;
            }

            ptr1 += pel_width - 8;
            ptr2 += pel_width - 8;           
        }        
    }
    
    if (sad > SC_TH)
    {
#if VERB_ERR_CONC == 1
        printf("\nScene change detected: SAD %li\n", sad); 
#endif
        return 1;
    }
    else
    {
#if VERB_ERR_CONC == 1
        printf("\nNo scene change detected: SAD %li\n", sad); 
#endif
        return 0;
    }
}




