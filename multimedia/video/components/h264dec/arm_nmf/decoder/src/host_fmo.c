/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include "types.h"
#include "host_types.h"
#include "macros.h"
#include "settings.h"
#include "host_fmo.h"

/* Local functions prototypes */
void Interleaved(t_uint16 PicSizeInMapUnits, t_pic_par *p_p, t_uint16 *p_mb_slice_map);
void Dispersed(t_uint16 PicSizeInMapUnits, t_seq_par *p_sp, t_pic_par *p_p, t_uint16 *p_mb_slice_map);
void ForeGround(t_uint16 PicSizeInMapUnits, t_seq_par *p_sp, t_pic_par *p_p, t_uint16 *p_mb_slice_map);
void BoxOut(t_uint16 PicSizeInMapUnits, t_seq_par *p_sp, t_pic_par *p_p, t_slice_hdr *p_sh, t_uint16 *p_mb_slice_map);
void Raster(t_uint16 PicSizeInMapUnits, t_pic_par *p_p, t_slice_hdr *p_sh, t_uint16 *p_mb_slice_map);
void Wipe(t_uint16 PicSizeInMapUnits, t_seq_par *p_sp, t_pic_par *p_p, t_slice_hdr *p_sh, t_uint16 *p_mb_slice_map);
void Explicit(t_uint16 PicSizeInMapUnits, t_pic_par *p_p, t_uint16 *p_mb_slice_map);



/**
 * \brief Perform decoding of macroblock to slice group map
 * \param p_sp Pointer to the active sequence parameter set
 * \param p_p Pointer to the active picture parameter set
 * \param p_sh Pointer to the slice header structure
 * \param p_buff Pointer to the buffers structure    
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>02-11-2004&nbsp;</td><td>First release.</td></tr>
 * <tr><td valign=top>25-01-2005&nbsp;</td><td>Added FMO support.</td></tr>
 * </table>
 * </dl>
 *
 * Perform decoding of macroblock to slice group map.
 */
 
void MbSliceMap(t_seq_par *p_sp, t_pic_par *p_p, t_slice_hdr *p_sh, t_dec_buff *p_buff)
{
    t_uint16 i;
    t_uint16 PicSizeInMapUnits = (p_sp->pic_width_in_mbs_minus1+1) * (p_sp->pic_height_in_map_units_minus1+1)
                                    * (2-p_sp->frame_mbs_only_flag);
        
    if (p_p->num_slice_groups_minus1 == 0)
    {
        for (i=0; i < PicSizeInMapUnits; i++)
            p_buff->mb_slice_map[i] = 0;
        
        return;    
    }
    
    switch (p_p->slice_group_map_type)
    {
        case 0:
            Interleaved(PicSizeInMapUnits, p_p, p_buff->mb_slice_map);
        break;
        
        case 1:
            Dispersed(PicSizeInMapUnits, p_sp, p_p, p_buff->mb_slice_map); 
        break;
        
        case 2:
            ForeGround(PicSizeInMapUnits, p_sp, p_p, p_buff->mb_slice_map);  
        break;
        
        case 3:
            BoxOut(PicSizeInMapUnits, p_sp, p_p, p_sh, p_buff->mb_slice_map);
        break;
        
        case 4:
            Raster(PicSizeInMapUnits, p_p, p_sh, p_buff->mb_slice_map);
        break;
        
        case 5:
            Wipe(PicSizeInMapUnits, p_sp, p_p, p_sh, p_buff->mb_slice_map); 
        break;
        
        case 6:
            Explicit(PicSizeInMapUnits, p_p, p_buff->mb_slice_map);
        break;
        
        default:
        break;
    }
    
    return;
}



/**
 * \brief Decode interleaved map type
 * \param PicSizeInMapUnits Picture size in map units
 * \param p_p Pointer to the active picture parameter set
 * \param p_mb_slice_map Output array
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>25-01-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * Perform decoding of interleaved map type.
 */
 
void Interleaved(t_uint16 PicSizeInMapUnits, t_pic_par *p_p, t_uint16 *p_mb_slice_map)
{
    t_uint16 i, j, igrp;
    
    i = 0;
    
    do
    {
        for(igrp = 0; (igrp <= p_p->num_slice_groups_minus1) && (i < PicSizeInMapUnits); i+= p_p->run_length_minus1[igrp++] + 1)
            for (j = 0; (j <= p_p->run_length_minus1[igrp]) && ((i+j) < PicSizeInMapUnits); j++)
                p_mb_slice_map[i+j] = igrp;
    }
    while (i < PicSizeInMapUnits);

}



/**
 * \brief Decode dispersed map type
 * \param PicSizeInMapUnits Picture size in map units
 * \param p_sp Pointer to the active sequence parameter set
 * \param p_p Pointer to the active picture parameter set
 * \param p_mb_slice_map Output array      
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>25-01-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * Perform decoding of dispersed map type.
 */
 
void Dispersed(t_uint16 PicSizeInMapUnits, t_seq_par *p_sp, t_pic_par *p_p, t_uint16 *p_mb_slice_map)
{
    t_uint16 i;
    
    for (i = 0; i < PicSizeInMapUnits; i++)
    {
        p_mb_slice_map[i] =     ((i % (p_sp->pic_width_in_mbs_minus1+1)) +
                                (((i / (p_sp->pic_width_in_mbs_minus1+1)) * (p_p->num_slice_groups_minus1 + 1)) / 2))
                                % (p_p->num_slice_groups_minus1 + 1);      
    }
}



/**
 * \brief Decode foreground-background map type
 * \param PicSizeInMapUnits Picture size in map units
 * \param p_sp Pointer to the active sequence parameter set
 * \param p_p Pointer to the active picture parameter set
 * \param p_mb_slice_map Output array      
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>25-01-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * Perform decoding of foreground-background map type.
 */
 
void ForeGround(t_uint16 PicSizeInMapUnits, t_seq_par *p_sp, t_pic_par *p_p, t_uint16 *p_mb_slice_map)
{
    t_uint16 i, x, y;
    t_sint16 iGroup;
    t_uint16 xTopLeft, yTopLeft, xBottomRight, yBottomRight;
    
    for (i = 0; i < PicSizeInMapUnits; i++)
        p_mb_slice_map[i] = p_p->num_slice_groups_minus1;
        
    for (iGroup = p_p->num_slice_groups_minus1 - 1; iGroup >= 0; iGroup--)
    {
        yTopLeft = p_p->top_left[iGroup] / (p_sp->pic_width_in_mbs_minus1+1);
        xTopLeft = p_p->top_left[iGroup] % (p_sp->pic_width_in_mbs_minus1+1);
        yBottomRight = p_p->bottom_right[iGroup] / (p_sp->pic_width_in_mbs_minus1+1);
        xBottomRight = p_p->bottom_right[iGroup] % (p_sp->pic_width_in_mbs_minus1+1);
        
        for (y = yTopLeft; y <= yBottomRight; y++)
            for (x = xTopLeft; x <= xBottomRight; x++)
                p_mb_slice_map[y * (p_sp->pic_width_in_mbs_minus1+1) + x] = iGroup;
    
    }
}



/**
 * \brief Decode box-out map type
 * \param PicSizeInMapUnits Picture size in map units
 * \param p_sp Pointer to the active sequence parameter set
 * \param p_p Pointer to the active picture parameter set
 * \param p_sh Pointer to the slice header structure
 * \param p_mb_slice_map Output array      
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>25-01-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * Perform decoding of box-out map type.
 */
 
void BoxOut(t_uint16 PicSizeInMapUnits, t_seq_par *p_sp, t_pic_par *p_p, t_slice_hdr *p_sh, t_uint16 *p_mb_slice_map)
{
    t_uint16 i, k;
    t_sint16 leftBound, topBound, rightBound, bottomBound;
    t_sint16 x, y, xDir, yDir;
    t_sint16 mapUnitVacant;

    t_uint16 mapUnitsInSliceGroup0 = MIN((p_p->slice_group_change_rate_minus1 + 1) * p_sh->slice_group_change_cycle, PicSizeInMapUnits);

    for( i = 0; i < PicSizeInMapUnits; i++ )
        p_mb_slice_map[i] = 2;

    x = (p_sp->pic_width_in_mbs_minus1 + 1 - p_p->slice_group_change_dir_flag) / 2;
    y = (p_sp->pic_height_in_map_units_minus1 + 1 - p_p->slice_group_change_dir_flag) / 2;

    leftBound   = x;
    topBound    = y;
    rightBound  = x;
    bottomBound = y;

    xDir =  p_p->slice_group_change_dir_flag - 1;
    yDir =  p_p->slice_group_change_dir_flag;

    for(k = 0; k < PicSizeInMapUnits; k += mapUnitVacant) 
    {
        mapUnitVacant = (p_mb_slice_map[y * (p_sp->pic_width_in_mbs_minus1 + 1) + x] == 2);
        
        if(mapUnitVacant)
            p_mb_slice_map[y * (p_sp->pic_width_in_mbs_minus1 + 1) + x] = (k >= mapUnitsInSliceGroup0);

        if((xDir == -1) && (x == leftBound)) 
        {
            leftBound = MAX(leftBound - 1, 0);
            x = leftBound;
            xDir = 0;
            yDir = 2 * p_p->slice_group_change_dir_flag - 1;
        } 
        else if((xDir == 1) && (x == rightBound)) 
        {
            rightBound = MIN(rightBound + 1, p_sp->pic_width_in_mbs_minus1);
            x = rightBound;
            xDir = 0;
            yDir = 1 - 2 * p_p->slice_group_change_dir_flag;
        } 
        else if((yDir == -1) && (y == topBound)) 
        {
            topBound = MAX(topBound - 1, 0);
            y = topBound;
            xDir = 1 - 2 * p_p->slice_group_change_dir_flag;
            yDir = 0;
        } 
        else if((yDir == 1) && (y == bottomBound)) 
        {
            bottomBound = MIN(bottomBound + 1, p_sp->pic_height_in_map_units_minus1);
            y = bottomBound;
            xDir = 2 * p_p->slice_group_change_dir_flag - 1;
            yDir = 0;
        } 
        else
        {
            x = x + xDir;
            y = y + yDir;
        }
    }
}



/**
 * \brief Decode raster scan map type
 * \param PicSizeInMapUnits Picture size in map units
 * \param p_p Pointer to the active picture parameter set
 * \param p_sh Pointer to the slice header structure
 * \param p_mb_slice_map Output array      
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>25-01-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * Perform decoding of raster scan map type.
 */
 
void Raster(t_uint16 PicSizeInMapUnits, t_pic_par *p_p, t_slice_hdr *p_sh, t_uint16 *p_mb_slice_map)
{
    t_uint16 mapUnitsInSliceGroup0 = MIN((p_p->slice_group_change_rate_minus1 + 1) * p_sh->slice_group_change_cycle, PicSizeInMapUnits);
    t_uint16 sizeOfUpperLeftGroup = p_p->slice_group_change_dir_flag ? (PicSizeInMapUnits - mapUnitsInSliceGroup0) : mapUnitsInSliceGroup0;

    t_uint16 i;

    for(i = 0; i < PicSizeInMapUnits; i++)
        if( i < sizeOfUpperLeftGroup )
            p_mb_slice_map[i] = p_p->slice_group_change_dir_flag;
        else
            p_mb_slice_map[i] = 1 - p_p->slice_group_change_dir_flag;
}



/**
 * \brief Decode wipe map type
 * \param PicSizeInMapUnits Picture size in map units
 * \param p_sp Pointer to the active sequence parameter set
 * \param p_p Pointer to the active picture parameter set
 * \param p_sh Pointer to the slice header structure
 * \param p_mb_slice_map Output array      
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>25-01-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * Perform decoding of wipe map type.
 */
 
void Wipe(t_uint16 PicSizeInMapUnits, t_seq_par *p_sp, t_pic_par *p_p, t_slice_hdr *p_sh, t_uint16 *p_mb_slice_map)
{
    t_uint16 mapUnitsInSliceGroup0 = MIN((p_p->slice_group_change_rate_minus1 + 1) * p_sh->slice_group_change_cycle, PicSizeInMapUnits);
    t_uint16 sizeOfUpperLeftGroup = p_p->slice_group_change_dir_flag ? (PicSizeInMapUnits - mapUnitsInSliceGroup0) : mapUnitsInSliceGroup0;

    t_uint16 i, j, k = 0;

    for(j = 0; j < (p_sp->pic_width_in_mbs_minus1 + 1); j++)
        for(i = 0; i < (p_sp->pic_height_in_map_units_minus1 + 1); i++)
            if(k++ < sizeOfUpperLeftGroup)
                p_mb_slice_map[i * (p_sp->pic_width_in_mbs_minus1 + 1) + j] = 1 - p_p->slice_group_change_dir_flag;
            else
                p_mb_slice_map[i * (p_sp->pic_width_in_mbs_minus1 + 1) + j] = p_p->slice_group_change_dir_flag;

}



/**
 * \brief Decode explicit map type
 * \param PicSizeInMapUnits Picture size in map units
 * \param p_p Pointer to the active picture parameter set
 * \param p_mb_slice_map Output array      
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>25-01-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * Perform decoding of explicit map type.
 */
 
void Explicit(t_uint16 PicSizeInMapUnits, t_pic_par *p_p, t_uint16 *p_mb_slice_map)
{
    t_uint16 i;
    
    for (i = 0; i < PicSizeInMapUnits; i++)
        p_mb_slice_map[i] = p_p->slice_group_id[i];

}
