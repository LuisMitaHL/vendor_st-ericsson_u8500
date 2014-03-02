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
#include "host_sh_concealment.h"
#include "host_bitstream.h"



/* Local function prototypes */
void ConcealOneSliceHeader(t_pic_par *pps, t_seq_par *sp, t_dec_buff *buf, t_slice_hdr *old_sh, t_slice_hdr *sh);
t_uint16 ConcealMultipleSliceHeaders(t_pic_par *pps, t_seq_par *sp, t_dec_buff *buf, t_slice_hdr *old_sh, t_uint16 next_slice_flag, t_sint16 slice_num, t_slice_hdr *sh);
t_uint16 ConcealFirstIDRHeader(t_pic_par *pps, t_seq_par *sp, t_uint16 next_slice_flag, t_uint16 slice_num, t_slice_hdr *sh);
t_uint16 FindCodeLen(t_uint16 type, t_sint32 value);



/**
 * \brief Main interface to slice header concealment functions
 * \param pps Pointer to picture parameter set structure array 
 * \param sp Pointer to sequence parameter set structure array
 * \param buf Pointer to decoder buffer structure
 * \param old_sh Pointer to structure olding previous slice header parameter
 * \param old_sl Pointer to old_slice structure containing infos of previous slice
 * \param next_slice_flag Flag indicating the presence of a slice of the next frame in the array
 * \param slice_num Pointer to last parsed slice header in the array
 * \param sh Array of slice headers
 * \return Number of slices of the current frame
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>01-08-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function call the slice header error concealment specific functions. 
 */
 
t_uint16 conceal_sh(t_pic_par *pps, t_seq_par *sp, t_dec_buff *buf, t_slice_hdr *old_sh, t_old_slice *old_sl, t_uint16 next_slice_flag, t_sint16 slice_num, t_slice_hdr *sh)
{
    t_uint16 num_slices = next_slice_flag ? slice_num : slice_num+1;
    t_uint16 i, err;
    t_uint16 res = num_slices;
    
    if (num_slices == 1)
    {
        if (sh[0].error)
        {
            if (old_sh != NULL)
            {    
                ConcealOneSliceHeader(pps, sp, buf, old_sh, sh);
                res = 1;
            }
            else
            {
                res = ConcealFirstIDRHeader(pps, sp, next_slice_flag, num_slices, sh);
            }
            
            /* Reset error flag */
            sh[0].error = 0;
            
            if (!next_slice_flag)
            {
                saveOldPar(&sh[0], old_sl);
            }
        } 
    }
    else
    {
        err = 0;        
        for (i = 0; i < num_slices; i++)
        {
            err |= sh[i].error;
            sh[i].len = 0;
        }
        
        if (err)
        {
            if (old_sh != NULL)
            {
                res = ConcealMultipleSliceHeaders(pps, sp, buf, old_sh, next_slice_flag, num_slices, sh);
            }
            else
            {
                res = ConcealFirstIDRHeader(pps, sp, next_slice_flag, num_slices, sh);
            }
            
            for (i = 0; i < res; i++)
            {
                sh[i].error = (sh[i].error == 2) ? 1 : 0;
            }
            
            if (slice_num == (res-1))
            {
                saveOldPar(&sh[slice_num], old_sl);
            }
        }
    }
    
    return res;
}



/**
 * \brief Single slice header concealment 
 * \param pps Pointer to picture parameter set structure array 
 * \param sp Pointer to sequence parameter set structure array
 * \param buf Pointer to decoder buffer structure
 * \param old_sh Pointer to structure olding previous slice header parameter
 * \param sh Array of slice headers
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>01-08-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function perform concealment for a single slice header. Missing or erroneous parameters are concealed with those of
 * previous slice header (if available) or with default values.  
 */
 
void ConcealOneSliceHeader(t_pic_par *pps, t_seq_par *sp, t_dec_buff *buf, t_slice_hdr *old_sh, t_slice_hdr *sh)
{
    t_uint16 err = 0;
    t_slice_hdr *shd = &sh[0];
    t_uint32 MaxFrameNum = (1 << (sp->log2_max_frame_num_minus4 + 4));
    
    
    shd->len = 0;
    
    /* First MB in slice */
    if (shd->first_mb_in_slice != 0)
    {
        err = 1;
        shd->first_mb_in_slice = 0;

#if VERB_ERR_CONC == 1
        printf("Concealed first_mb_in_slice %i\n", shd->first_mb_in_slice);
#endif
    }
    
    shd->len += 1;

    /* Slice type */
    if (err || (shd->error_type == SLICE_TYPE))
    {
        if (shd->nut == 5)
        {
            shd->slice_type = 2;
            shd->len += 3;
        }
        else
        {
            if ((old_sh->nut == 1) && (!old_sh->error))
            {
                shd->slice_type = old_sh->slice_type;
            }
            else
            {
                shd->slice_type = 0;
            }           
        }
        
        err = 1;
        
#if VERB_ERR_CONC == 1
        printf("Concealed slice_type %i\n", shd->slice_type);
#endif
    }
    
    shd->len += FindCodeLen(UE, shd->slice_type);

   
    /* pic_par_set_id */
    if (err || (shd->error_type == PIC_PAR_SET_ID))
    {
        if (!old_sh->error)
            shd->pic_par_set_id = old_sh->pic_par_set_id;
        else
            shd->pic_par_set_id = 0;

        err = 1;
        
#if VERB_ERR_CONC == 1
        printf("Concealed pic_par_set_id %i\n", shd->pic_par_set_id);
#endif
    }
    
    shd->len += FindCodeLen(UE, shd->pic_par_set_id);
    
    /* Activate pps */
    while (pps->pic_par_set_id != shd->pic_par_set_id)
        pps++;
         
    if (err || (shd->error_type == FRAME_NUM))
    {
        if ((old_sh->nri == 0) && (shd->nri == 0))
        {
            shd->frame_num = old_sh->frame_num;
        }
        else
        {
            shd->frame_num = (buf->PreviousFrameNum + 1) % MaxFrameNum; 
        }
           
#if VERB_ERR_CONC == 1
        printf("Concealed frame_num %i\n", shd->frame_num);
#endif
    }   
    
    shd->len += sp->log2_max_frame_num_minus4 + 4;
        
    /* idr_pic_id */
    if (shd->nut == 5)
    {
        if (old_sh->nut == 5)
            shd->idr_pic_id = old_sh->idr_pic_id + 1;
        else
            shd->idr_pic_id = 0;
            
        shd->len += FindCodeLen(UE, shd->idr_pic_id);
        
#if VERB_ERR_CONC == 1
        printf("Concealed idr_pic_id %i\n", shd->idr_pic_id);
#endif    
    }
    
    /* POC info */
    if (sp->pic_order_cnt_type == 0)
    {
        /* pic_order_count_lsb */
        if (shd->nut == 5)
        {
            shd->pic_order_cnt_lsb = 0;
        }
        else
        {
            shd->pic_order_cnt_lsb = (buf->PrevPicOrderCntLsb + 1) % (1 << (sp->log2_max_pic_order_cnt_lsb_minus4 + 4));
        }
        
        shd->len += sp->log2_max_pic_order_cnt_lsb_minus4 + 4;

#if VERB_ERR_CONC == 1
        printf("Concealed pic_order_cnt_lsb %i\n", shd->pic_order_cnt_lsb);
#endif
           
        if (pps->pic_order_present_flag)
        {
            if (!old_sh->error)
                shd->delta_pic_order_cnt_bottom = old_sh->delta_pic_order_cnt_bottom;
            else
                shd->delta_pic_order_cnt_bottom = 1;
                
            shd->len += FindCodeLen(SE, shd->delta_pic_order_cnt_bottom);

#if VERB_ERR_CONC == 1
            printf("Concealed delta_pic_order_cnt_bottom %li\n", shd->delta_pic_order_cnt_bottom);
#endif
        }
        else
        {
            shd->delta_pic_order_cnt_bottom = 0;
        }
    }
    
    if ((sp->pic_order_cnt_type == 1) && (sp->delta_pic_order_always_zero_flag == 0))
    {
        shd->delta_pic_order_cnt[0] = 0;
        shd->delta_pic_order_cnt[1] = 0;
        
        shd->len += 1;
        
        if (pps->pic_order_present_flag)
        {         
            shd->len += 1;
        }    
    }
    else
    {
        shd->delta_pic_order_cnt[0] = 0;
        shd->delta_pic_order_cnt[1] = 0;    
    }
    
    /* Redundant pic count */
    if (pps->redundant_pic_cnt_present_flag)
    {
        shd->redundant_pic_cnt = 0;
        shd->len += 1;
        
#if VERB_ERR_CONC == 1
        printf("Concealed redundant_pic_cnt %i\n", shd->redundant_pic_cnt);
#endif
    }
    else
    {
        shd->redundant_pic_cnt = 0;
    }
    
    /* num_ref_idx */
    if ((shd->slice_type == 0) || (shd->slice_type == 5))
    {
        t_uint16 available_ref = MAX(buf->numShortRef + buf->numLongRef, 1);
        
        if (available_ref < (pps->num_ref_idx_l0_active_minus1 + 1))
        {
            shd->num_ref_idx_active_override_flag = 1;
            shd->num_ref_idx_l0_active_minus1 = available_ref - 1;
            
            shd->len += FindCodeLen(UE, shd->num_ref_idx_l0_active_minus1) + 1;
            
#if VERB_ERR_CONC == 1
            printf("Concealed num_ref_idx_active_override_flag %i\n", shd->num_ref_idx_active_override_flag);
            printf("Concealed num_ref_idx_l0_active_minus1 %i\n", shd->num_ref_idx_l0_active_minus1);
#endif
        }
        else
        {
            shd->num_ref_idx_active_override_flag = 0;
            shd->len += 1;
            
#if VERB_ERR_CONC == 1
            printf("Concealed num_ref_idx_active_override_flag %i\n", shd->num_ref_idx_active_override_flag);
#endif
        }    
    }
    
    /* Ref pic list reordering */
    if ((shd->slice_type == 0) || (shd->slice_type == 5))
    {
        shd->ref_pic_list_reordering_flag_l0 = 0;
        shd->len += 1;
        
#if VERB_ERR_CONC == 1
        printf("Concealed ref_pic_list_reordering_flag_l0 %i\n", shd->ref_pic_list_reordering_flag_l0);
#endif
    } 
    
    /* Dec ref pic marking */
    if (shd->nri > 0)
    {
        if (shd->nut == 5)
        {
            shd->no_output_of_prior_pics_flag = 0;
            shd->long_term_reference_flag = 0;           
            shd->len += 2;
        }
        else
        {
            shd->adaptive_ref_pic_marking_mode_flag = 0;            
            shd->len += 1;

#if VERB_ERR_CONC == 1
            printf("Concealed adaptive_ref_pic_marking_mode_flag %i\n", shd->adaptive_ref_pic_marking_mode_flag);
#endif
        }
    }
    
    /* Slice QP delta */
    if (!old_sh->error)
    {
        shd->slice_qp_delta = old_sh->slice_qp_delta;
    }
    else
    {
        shd->slice_qp_delta = 0;
    }
    
    shd->slice_qp = 26 + pps->pic_init_qp_minus26 + shd->slice_qp_delta;
    
    shd->len += FindCodeLen(SE, shd->slice_qp_delta);
    
#if VERB_ERR_CONC == 1
    printf("Concealed slice_qp_delta %i\n", shd->slice_qp_delta);
#endif
    
    /* Deblocking control */
    if (pps->debl_flt_control_present_flag)
    {
        shd->disable_deblocking_filter_idc = 1;
        shd->len += 3;   
             
#if VERB_ERR_CONC == 1
        printf("Concealed disable_deblocking_filter_idc %i\n", shd->disable_deblocking_filter_idc);
#endif
    }
    else
    {
        shd->disable_deblocking_filter_idc = 0;
    } 
    
    shd->slice_alpha_c0_offset_div2 = 0;
    shd->slice_beta_offset_div2 = 0;
    
    /* slice_group_change_cycle */
    /* Useless for one slice */
    
    /* Adjust bitstream pointer */
    shd->p_bsh_end = shd->p_bsh_start;
    
    shd->p_bsh_end.addr += (shd->len >> 3) + 1; 
    shd->p_bsh_end.os = 8 - (shd->len & 7);
}



/**
 * \brief Slice header concealment for multiple slice  
 * \param pps Pointer to picture parameter set structure array 
 * \param sp Pointer to sequence parameter set structure array
 * \param buf Pointer to decoder buffer structure
 * \param old_sh Pointer to structure olding previous slice header parameter
 * \param next_slice_flag Flag indicating the presence of a slice of the next frame in the array
 * \param slice_num Pointer to last parsed slice header in the array
 * \param sh Array of slice headers
 * \return Number of slices of the current frame
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>01-08-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function perform concealment for multiple slice headers. Missing or erroneous parameters are concealed with those of
 * correct slice header from the same frame (if any) or with default values.  
 */

t_uint16 ConcealMultipleSliceHeaders(t_pic_par *pps, t_seq_par *sp, t_dec_buff *buf, t_slice_hdr *old_sh, t_uint16 next_slice_flag, t_sint16 slice_num, t_slice_hdr *sh)
{
    t_uint16 i, nslice;
    t_sint16 j;
    t_sint32 conc_val;
#if CONC_FRAME_NUM == 1
    t_uint16 flag, index, last_frn, curr_frn, next_frn;
    t_uint32 MaxFrameNum = (1 << (sp->log2_max_frame_num_minus4 + 4));
    t_slice_hdr *prevsh, *nextsh;
#endif
    
    nslice = slice_num;

#if CONC_FRAME_NUM == 1    
    /* Check frame numbers */
    flag = 0;
          
    for (i = 0; (i < slice_num) & (!flag); i++)
    {
        if (sh[i].error_type <= FRAME_NUM)
        {
            flag = 1;
            index = i;
        }
    }
      
    if (flag)
    {
        last_frn = ((index == 0) || (sh[0].error_type <= FRAME_NUM)) ? (old_sh->frame_num + 1) % MaxFrameNum : sh[0].frame_num;
        curr_frn = (last_frn + 1) % MaxFrameNum;
        next_frn = (last_frn + 2) % MaxFrameNum; 
    
        /* Try to conceal frame numbers and find missed frames */        
        for (i = index; i < slice_num; i++)
        {
            prevsh = (i == 0) ? old_sh : &sh[i-1];
            
            if ((i < (slice_num-1)) || next_slice_flag)
            {
                nextsh = &sh[i+1];
            }
            else
            {
                nextsh = NULL;
            }
               
            if ((prevsh->nri == 0) && (sh[i].nri == 0))
            {
                sh[i].frame_num = prevsh->frame_num;
            }
            else
            {
                if ((nextsh != NULL) && (nextsh->error_type > FRAME_NUM))
                {
                    if (nextsh->frame_num == last_frn)
                    {
                        sh[i].frame_num = last_frn;
                    }
                    else if (nextsh->frame_num == curr_frn)
                    {
                        if ((index == 0) || (nextsh->first_mb_in_slice == 0))
                        {
                            sh[i].frame_num = last_frn;
                            nslice = i+1;
                            break;
                        }
                        else
                        {
                            sh[i].frame_num = curr_frn;
                            nslice = i;
                            break;
                        }
                    }
                    else if (nextsh->frame_num == next_frn)
                    {
                        sh[i].frame_num = curr_frn;
                        nslice = i;
                        break;
                    }
                    else
                    {
                        nextsh->error_type = MIN(FRAME_NUM, nextsh->error_type);
                        nextsh->error = 1;
                        
                        if (i == 0)
                        {
                            sh[i].frame_num = last_frn;
                        }
                        else
                        {
                            sh[i].frame_num = curr_frn;
                            nslice = i;
                            break;
                        }
                    }
                }
                else
                {
                    sh[i].frame_num = curr_frn;
                    nslice = i;
                    break;
                }
            }            
        }    
    }
    
    /* From here we assume the correct number of slices of the frame */
    if (nslice == 1)
    {
        if (sh[0].error)
            ConcealOneSliceHeader(pps, sp, buf, old_sh, sh);
        
        return 1;
    }
    
    flag = 0;
    for (i = 0; i < nslice; i++)
    {
        if (sh[i].error ==  1)
            flag = 1;
    }
    
    if (!flag)
        return nslice;
    
    /* first_mb_in_slice */
    if (sh[0].error_type <= FRAME_NUM)
    {
        sh[0].first_mb_in_slice = 0; 
    }
    
    sh[0].len = FindCodeLen(UE, sh[0].first_mb_in_slice);
    
    for (i = 1; i < nslice; i++)
    {
        if (sh[i].error_type <= PIC_PAR_SET_ID)
        {
            sh[i].error = 2;  /* Impossible to recover. Drop entire slice */
        }
        else
        {
            sh[i].len = FindCodeLen(UE, sh[i].first_mb_in_slice);
        }
    }
    
    j = -1;
    
    for (i = 0; i < nslice; i++)
    {
        if (sh[i].error_type > PIC_PAR_SET_ID)
        {
            j = i;
            break;
        }
    }
    
    /* Slice type */
    if (sh[0].error_type <= FRAME_NUM)
    {
        if (j == -1)
        {
            sh[0].slice_type = (sh[0].nut == 5) ? 2 : (old_sh->nut == 1) ? old_sh->slice_type : 0;
        }
        else
        {
            sh[0].slice_type = sh[j].slice_type;
        }
    }

    for (i = 0; i < nslice; i++)
    {
        if (sh[i].error < 2)
            sh[i].len += FindCodeLen(UE, sh[i].slice_type);       
    }
            
    /* Pic_par_set_id */   
    if (sh[0].error_type <= FRAME_NUM)
    {    
        if (j == -1)
        {
            sh[0].pic_par_set_id = (old_sh->error_type > PIC_PAR_SET_ID) ? old_sh->pic_par_set_id : 0;
        }
        else
        {
            sh[0].pic_par_set_id = sh[j].pic_par_set_id;
        }
    }
    
    for (i = 1; i < nslice; i++)
    {
        sh[i].pic_par_set_id = sh[0].pic_par_set_id;
        sh[i].len += FindCodeLen(UE, sh[i].pic_par_set_id);   
    }
#else
    j = 0;
    
    for (i = 0; i < nslice; i++)
    {
        if (sh[i].error_type <= FRAME_NUM)
        {
            sh[i].error = 2;
        }
        else
        {
            j++;
        }
    }
    
    if (j == 0)
        return 0;

    if (sh[0].error_type <= FRAME_NUM)
    {
       sh[0].first_mb_in_slice = 0;
       sh[0].slice_type = sh[j].slice_type;
       sh[0].pic_par_set_id = sh[j].pic_par_set_id;
       sh[0].frame_num = sh[j].frame_num; 
    }
    
    for (i = 0; i < nslice; i++)
    {
        if (sh[i].error < 2)
        {
            sh[i].len = FindCodeLen(UE, sh[i].first_mb_in_slice);
            sh[i].len += FindCodeLen(UE, sh[i].slice_type);
            sh[i].len += FindCodeLen(UE, sh[i].pic_par_set_id);
        }
    }
    
    next_slice_flag++;
#endif
    
    /* Activate pps */
    while (pps->pic_par_set_id != sh[0].pic_par_set_id)
        pps++;
         
    /* Frame_num */
    for (i = 0; i < nslice; i++)
    {
        if (sh[i].error < 2)
        {
            sh[i].len += sp->log2_max_frame_num_minus4 + 4;  
        }
    }
        
    /* idr_pic_id */
    if (sh[0].nut == 5)
    {    
        if (sh[0].error)
            conc_val = (old_sh->nut == 5) ? old_sh->idr_pic_id + 1 : 0;
        else
            conc_val = sh[0].idr_pic_id;
        
        for (i =0; i < nslice; i++)
        {
            sh[i].idr_pic_id = conc_val;
            sh[i].len += FindCodeLen(UE, sh[i].idr_pic_id); 
        }       
    }

    j = -1;
    for (i = 0; i < nslice; i++)
    {
        if (!sh[i].error)
        {
            j = i;
            break;
        }
    }
               
    /* POC info */
    if (sp->pic_order_cnt_type == 0)
    {
        /* pic_order_count_lsb */
        if (sh[0].nut == 5)
        {
            for (i = 0; i < nslice; i++)
            {
                sh[i].pic_order_cnt_lsb = 0;
                sh[i].len += sp->log2_max_pic_order_cnt_lsb_minus4 + 4; 
            }
        }
        else
        {
            if (j == -1)
            {
                conc_val = (buf->PrevPicOrderCntLsb + 1) % (1 << (sp->log2_max_pic_order_cnt_lsb_minus4 + 4));
            }
            else
            {
                conc_val = sh[j].pic_order_cnt_lsb;
            }
        
            for (i = 0; i < nslice; i++)
            {
                sh[i].pic_order_cnt_lsb = conc_val;
                sh[i].len += sp->log2_max_pic_order_cnt_lsb_minus4 + 4; 
            } 
        }    
                        
        if (pps->pic_order_present_flag)
        {
            if (j == -1)
            {
                conc_val = (old_sh->error_type <= IDR_ID) ? 1 : old_sh->delta_pic_order_cnt_bottom;
            }
            else
            {
                conc_val = sh[j].delta_pic_order_cnt_bottom;
            }    
            
            for (i = 0; i < nslice; i++)
            {
                sh[i].delta_pic_order_cnt_bottom = conc_val;
                sh[i].len += FindCodeLen(SE, conc_val);
            }
        }
        else
        {
            for (i = 0; i < nslice; i++)
            {
                sh[i].delta_pic_order_cnt_bottom = 0;
            }
        }
    }
    
    if ((sp->pic_order_cnt_type == 1) && (sp->delta_pic_order_always_zero_flag == 0))
    {
        for (i = 0; i < nslice; i++)
        {
            sh[i].delta_pic_order_cnt[0] = (j == -1) ? 0 : sh[j].delta_pic_order_cnt[0];
            sh[i].len += FindCodeLen(SE, sh[i].delta_pic_order_cnt[0]); 
        }
        
        if (pps->pic_order_present_flag)
        {         
            for (i = 0; i < nslice; i++)
            {
                sh[i].delta_pic_order_cnt[1] = (j == -1) ? 0 : sh[j].delta_pic_order_cnt[1];
                sh[i].len += FindCodeLen(SE, sh[i].delta_pic_order_cnt[1]); 
            }
        }
        else
        {
            for (i = 0; i < nslice; i++)
            {
                sh[i].delta_pic_order_cnt[1] = 0;    
            }        
        }    
    }
    else
    {
        for (i = 0; i < nslice; i++)
        {
            sh[i].delta_pic_order_cnt[0] = 0;
            sh[i].delta_pic_order_cnt[1] = 0;    
        }
    }
    
    /* Redundant pic count */
    if (pps->redundant_pic_cnt_present_flag)
    {
        for (i = 0; i < nslice; i++)
        {
            sh[i].redundant_pic_cnt = (j == -1) ? sh[j].redundant_pic_cnt : 0;
            sh[i].len += FindCodeLen(UE, sh[i].redundant_pic_cnt);
        }
    }
    else
    {
        for (i = 0; i < nslice; i++)
        {
            sh[i].redundant_pic_cnt = 0;
        }
    }

    /* num_ref_idx */
    if ((sh[0].slice_type == 0) || (sh[0].slice_type == 5)
      ||(sh[0].slice_type == 1) || (sh[0].slice_type == 6))
    {
        t_uint16 available_ref = MAX(buf->numShortRef + buf->numLongRef, 1);
        
        conc_val = (j != -1) ? sh[j].num_ref_idx_active_override_flag : available_ref < (pps->num_ref_idx_l0_active_minus1 + 1) ? 1 : 0;
        
        for (i = 0; i < nslice; i++)
        {
            sh[i].num_ref_idx_active_override_flag = conc_val;
            sh[i].len += 1;
        }
        
        if (conc_val == 1)
        {
            conc_val = (j != -1) ? sh[j].num_ref_idx_l0_active_minus1 : available_ref - 1;
            
            for (i = 0; i < nslice; i++)
            {
                sh[i].num_ref_idx_l0_active_minus1 = conc_val;
                sh[i].len += FindCodeLen(UE, conc_val);            
            }
            
            conc_val = (j != -1) ? sh[j].num_ref_idx_l1_active_minus1 : available_ref - 1;
            
            for (i = 0; i < nslice; i++)
            {
                sh[i].num_ref_idx_l1_active_minus1 = conc_val;
                if(sh[0].slice_type%5==1)
                  sh[i].len += FindCodeLen(UE, conc_val);            
            }            
        }
    }
    
    /* ref_pic_list_reordering */
    if ((sh[0].slice_type == 0) || (sh[0].slice_type == 5)
      ||(sh[0].slice_type == 1) || (sh[0].slice_type == 6))
    {
        if (j != -1)
        {
            for (i = 0; i < nslice; i++)
            {
                sh[i].ref_pic_list_reordering_flag_l0 = sh[j].ref_pic_list_reordering_flag_l0;
                sh[i].len += 1;
                sh[i].ref_pic_list_reordering_flag_l1 = sh[j].ref_pic_list_reordering_flag_l1;
                if(sh[0].slice_type%5==1)
                  sh[i].len += 1;
            }
            
            if (sh[j].ref_pic_list_reordering_flag_l0)
            {
                t_uint16 idx = 0;
                t_uint16 stop = 0;
                
                do
                {
                    for (i = 0; i < nslice; i++)
                    {
                        sh[i].reordering_of_pic_nums_idc[idx] = sh[j].reordering_of_pic_nums_idc[idx];
                        sh[i].len += FindCodeLen(UE, sh[i].reordering_of_pic_nums_idc[idx]);
                    }
                    
                    if ((sh[j].reordering_of_pic_nums_idc[idx] == 0) || (sh[j].reordering_of_pic_nums_idc[idx] == 1))
                    {
                        for (i = 0; i < nslice; i++)
                        {
                            sh[i].abs_diff_pic_num_minus1[idx] = sh[j].abs_diff_pic_num_minus1[idx];
                            sh[i].len += FindCodeLen(UE, sh[i].abs_diff_pic_num_minus1[idx]);
                        }
                        
                        idx++;
                    }
                    else if (sh[j].reordering_of_pic_nums_idc[idx] == 3)
                    {
                        for (i = 0; i < nslice; i++)
                        {
                            sh[i].long_term_pic_num[idx] = sh[j].long_term_pic_num[idx];
                            sh[i].len += FindCodeLen(UE, sh[i].long_term_pic_num[idx]);
                        }
                        
                        idx++;                    
                    }
                    else
                        stop = 1;
                }
                while (stop == 0);
            }
            
    if ((sh[0].slice_type == 1) || (sh[0].slice_type == 6))
    {
            if (sh[j].ref_pic_list_reordering_flag_l1)
            {
                t_uint16 idx = 0;
                t_uint16 stop = 0;
                
                do
                {
                    for (i = 0; i < nslice; i++)
                    {
                        sh[i].reordering_of_pic_nums_idc_l1[idx] = sh[j].reordering_of_pic_nums_idc_l1[idx];
                        sh[i].len += FindCodeLen(UE, sh[i].reordering_of_pic_nums_idc_l1[idx]);
                    }
                    
                    if ((sh[j].reordering_of_pic_nums_idc_l1[idx] == 0) || (sh[j].reordering_of_pic_nums_idc_l1[idx] == 1))
                    {
                        for (i = 0; i < nslice; i++)
                        {
                            sh[i].abs_diff_pic_num_minus1_l1[idx] = sh[j].abs_diff_pic_num_minus1_l1[idx];
                            sh[i].len += FindCodeLen(UE, sh[i].abs_diff_pic_num_minus1_l1[idx]);
                        }
                        
                        idx++;
                    }
                    else if (sh[j].reordering_of_pic_nums_idc_l1[idx] == 3)
                    {
                        for (i = 0; i < nslice; i++)
                        {
                            sh[i].long_term_pic_num_l1[idx] = sh[j].long_term_pic_num_l1[idx];
                            sh[i].len += FindCodeLen(UE, sh[i].long_term_pic_num_l1[idx]);
                        }
                        
                        idx++;                    
                    }
                    else
                        stop = 1;
                }
                while (stop == 0);
            }
            
            
        }

        }
        else
        {
            for (i = 0; i < nslice; i++)
            {
                sh[i].ref_pic_list_reordering_flag_l0 = 0;
                sh[i].len += 1;
                sh[i].ref_pic_list_reordering_flag_l1 = 0;
                if(sh[0].slice_type%5==1)
                  sh[i].len += 1;
            }
        }
    }
    
    /* Dec_ref_pic_marking */
    if (sh[0].nri != 0)
    {
        if (sh[0].nut == 5)
        {
            if (j != -1)
            {
                for (i = 0; i < nslice; i++)
                {
                    sh[i].no_output_of_prior_pics_flag = sh[j].no_output_of_prior_pics_flag;
                    sh[i].long_term_reference_flag = sh[j].long_term_reference_flag;
                    sh[i].len += 2;
                }
            }
            else
            {
                for (i = 0; i < nslice; i++)
                {
                    sh[i].no_output_of_prior_pics_flag = 0;
                    sh[i].long_term_reference_flag = 0;
                    sh[i].len += 2;                
                }
            }
        }
        else
        {
            if (j != -1)
            {
                for (i = 0; i < nslice; i++)
                {
                    sh[i].adaptive_ref_pic_marking_mode_flag = sh[j].adaptive_ref_pic_marking_mode_flag;
                    sh[i].len += 1;
                }
                
                if (sh[j].adaptive_ref_pic_marking_mode_flag)
                {
                    t_uint16 idx = 0;
                    t_uint16 stop = 0;
                    
                    do
                    {
                        for (i = 0; i < nslice; i++)
                        {
                            sh[i].memory_management_control_operation[idx] = sh[j].memory_management_control_operation[idx];
                            sh[i].len += FindCodeLen(UE, sh[i].memory_management_control_operation[idx]);
                        }
                        
                        switch (sh[j].memory_management_control_operation[idx])
                        {
                            case 1:
                            case 3:
                                for (i = 0; i < nslice; i++)
                                {
                                    sh[i].difference_of_pic_nums_minus1[idx] = sh[j].difference_of_pic_nums_minus1[idx];
                                    sh[i].len += FindCodeLen(UE, sh[j].difference_of_pic_nums_minus1[idx]);
                                }
                                
                                if (sh[j].memory_management_control_operation[idx] == 3)
                                {
                                    for (i = 0; i < nslice; i++)
                                    {
                                        sh[i].long_term_frame_idx[idx] = sh[j].long_term_frame_idx[idx];
                                        sh[i].len += FindCodeLen(UE, sh[j].long_term_frame_idx[idx]);
                                    }    
                                }
                                
                                idx++;
                            break;
                            
                            case 2:
                                for (i = 0; i < nslice; i++)
                                {
                                    sh[i].marking_long_term_pic_num[idx] = sh[j].marking_long_term_pic_num[idx];
                                    sh[i].len += FindCodeLen(UE, sh[j].marking_long_term_pic_num[idx]);
                                }
                                
                                idx++;
                            break;
                            
                            case 4:
                                for (i = 0; i < nslice; i++)
                                {
                                    sh[i].max_long_term_frame_idx_plus1[idx] = sh[j].max_long_term_frame_idx_plus1[idx];
                                    sh[i].len += FindCodeLen(UE, sh[j].max_long_term_frame_idx_plus1[idx]);
                                }
                                
                                idx++; 
                            break;
                            
                            case 5:
                                idx++;
                            break;
                            
                            case 6:
                                for (i = 0; i < nslice; i++)
                                {
                                    sh[i].long_term_frame_idx[idx] = sh[j].long_term_frame_idx[idx];
                                    sh[i].len += FindCodeLen(UE, sh[j].long_term_frame_idx[idx]);
                                }
                                
                                idx++; 
                            break;
                            
                            case 0:
                                stop = 1;
                            break;
                        }
                    }
                    while (stop == 0);
                }
            }
            else
            {
                for (i = 0; i < nslice; i++)
                {
                    sh[i].adaptive_ref_pic_marking_mode_flag = 0;
                    sh[i].len += 1;
                }               
            }
        }
    
    }
    
    /* Slice QP delta */
    conc_val = (j != -1) ? sh[j].slice_qp_delta : (old_sh->error) ? 0 : old_sh->slice_qp_delta;
    
    for (i = 0; i < nslice; i++)
    {
        if (sh[i].error)
        {
            sh[i].slice_qp_delta = conc_val;    
        }
        
        sh[i].slice_qp = 26 + pps->pic_init_qp_minus26 + sh[i].slice_qp_delta;
        
        sh[i].len += FindCodeLen(SE, sh[i].slice_qp_delta);
    }
    
    /* Deblocking control */
    if (pps->debl_flt_control_present_flag)
    {
        conc_val = (j != -1) ? sh[j].disable_deblocking_filter_idc : 1; 
        
        for (i = 0; i < nslice; i++)
        {
            sh[i].disable_deblocking_filter_idc = conc_val;
            sh[i].len += FindCodeLen(UE, conc_val);
        }
        
        if (conc_val != 1)
        {
            for (i = 0; i < nslice; i++)
            {
                sh[i].slice_alpha_c0_offset_div2 = (j != -1) ? sh[j].slice_alpha_c0_offset_div2 : 0;
                sh[i].slice_beta_offset_div2 = (j != -1) ? sh[j].slice_beta_offset_div2 : 0;
                sh[i].len += FindCodeLen(SE, sh[i].slice_alpha_c0_offset_div2);
                sh[i].len += FindCodeLen(SE, sh[i].slice_beta_offset_div2);
            }
        }
    }
    else
    {
        for (i = 0; i < nslice; i++)
        {
            sh[i].disable_deblocking_filter_idc = 0;
            sh[i].slice_alpha_c0_offset_div2 = 0;
            sh[i].slice_beta_offset_div2 = 0;
        }
    } 
    
    /* slice_group_change_cycle */
    if ((pps->num_slice_groups_minus1 != 0) && (pps->slice_group_map_type >= 3) && (pps->slice_group_map_type <= 5))
    {
        t_uint16 bits;
        t_uint32 value;
        
        /* Determine ceil(log2(PicSizeInMapUnits/SliceGroupChangeRate + 1)) */
        bits = 15;
        value = ((sp->pic_width_in_mbs_minus1+1) * (sp->pic_height_in_map_units_minus1+1) / (pps->slice_group_change_rate_minus1+1)) + 1;
        
        if ((((sp->pic_width_in_mbs_minus1+1) * (sp->pic_height_in_map_units_minus1+1)) % (pps->slice_group_change_rate_minus1+1)))
            value++;
            
        while (((value>>bits)&0x1)==0)
            bits--;
        if ((value&((1<<bits)-1))!=0)
            bits+=1;
        
        conc_val = (j != -1) ? sh[j].slice_group_change_cycle : 1;    
                
        for (i = 0; i < nslice; i++)
        {
            sh[i].slice_group_change_cycle = conc_val;
            sh[i].len += bits;
        }
    }
    
    /* Adjust bitstream pointer */
    for (i = 0; i < nslice; i++)
    {
        sh[i].p_bsh_end = sh[i].p_bsh_start;
        
        if (sh[i].error < 2)
        {
            sh[i].p_bsh_end.addr += (sh[i].len >> 3) + 1; 
            sh[i].p_bsh_end.os = 8 - (sh[i].len & 7);
            //sh[i].header_length_in_bits = sh[i].len;
        }
    }
    
    return nslice;
}



/**
 * \brief Slice header concealment for first IDR slice header
 * \param pps Pointer to picture parameter set structure array 
 * \param sp Pointer to sequence parameter set structure array
 * \param next_slice_flag Flag indicating the presence of a slice of the next frame in the array
 * \param slice_num Pointer to last parsed slice header in the array
 * \param sh Array of slice headers
 * \return Number of slices of the current frame
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>01-08-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function perform concealment for first IDR slice header. Missing or erroneous parameters are concealed with those of
 * correct slice header from the same frame (if any) or with default values.  
 */
 
t_uint16 ConcealFirstIDRHeader(t_pic_par *pps, t_seq_par *sp, t_uint16 next_slice_flag, t_uint16 slice_num, t_slice_hdr *sh)
{
    t_uint16 i, flag;
    t_sint16 j;
    t_sint32 conc_val;
    t_uint16 nslice;        // , index;
    
    nslice = slice_num;
    
    flag = 0;
          
    for (i = 0; (i < slice_num) & (!flag); i++)
    {
        if (sh[i].error_type <= FRAME_NUM)
        {
            flag = 1;
            // index = i;
        }
    }

    j = 0;
    
    for (i = 0; i < nslice; i++)
    {
        if (sh[i].error_type <= FRAME_NUM)
        {
            sh[i].error = 2;
        }
        else
        {
            j++;
        }
    }
    
    if (j == 0)
        return 0;

    if (sh[0].error_type <= FRAME_NUM)
    {
       sh[0].first_mb_in_slice = 0;
       sh[0].slice_type = sh[j].slice_type;
       sh[0].pic_par_set_id = sh[j].pic_par_set_id;
       sh[0].frame_num = sh[j].frame_num; 
    }
    
    for (i = 0; i < nslice; i++)
    {
        if (sh[i].error < 2)
        {
            sh[i].len = FindCodeLen(UE, sh[i].first_mb_in_slice);
            sh[i].len += FindCodeLen(UE, sh[i].slice_type);
            sh[i].len += FindCodeLen(UE, sh[i].pic_par_set_id);
        }
    }
    
    next_slice_flag++;
    
    /* Activate pps */
    while (pps->pic_par_set_id != sh[0].pic_par_set_id)
        pps++;
        
    /* Frame_num */
    for (i = 0; i < nslice; i++)
    {
        if (sh[i].error < 2)
        {
            sh[i].len += sp->log2_max_frame_num_minus4 + 4;  
        }
    }
        
    /* idr_pic_id */
    for (i =0; i < nslice; i++)
    {
        sh[i].idr_pic_id = 0;
        sh[i].len += FindCodeLen(UE, sh[i].idr_pic_id); 
    }       
    
    j = -1;
    for (i = 0; i < nslice; i++)
    {
        if (!sh[i].error)
        {
            j = i;
            break;
        }
    }
               
    /* POC info */
    if (sp->pic_order_cnt_type == 0)
    {
        /* pic_order_count_lsb */
        for (i = 0; i < nslice; i++)
        {
            sh[i].pic_order_cnt_lsb = 0;
            sh[i].len += sp->log2_max_pic_order_cnt_lsb_minus4 + 4; 
        }
 
        if (pps->pic_order_present_flag)
        {
            if (j == -1)
            {
                conc_val = 1;
            }
            else
            {
                conc_val = sh[j].delta_pic_order_cnt_bottom;
            }    
            
            for (i = 0; i < nslice; i++)
            {
                sh[i].delta_pic_order_cnt_bottom = conc_val;
                sh[i].len += FindCodeLen(SE, conc_val);
            }
        }
        else
        {
            for (i = 0; i < nslice; i++)
            {
                sh[i].delta_pic_order_cnt_bottom = 0;
            }
        }
    }
    
    if ((sp->pic_order_cnt_type == 1) && (sp->delta_pic_order_always_zero_flag == 0))
    {
        for (i = 0; i < nslice; i++)
        {
            sh[i].delta_pic_order_cnt[0] = (j == -1) ? 0 : sh[j].delta_pic_order_cnt[0];
            sh[i].len += FindCodeLen(SE, sh[i].delta_pic_order_cnt[0]); 
        }
        
        if (pps->pic_order_present_flag)
        {         
            for (i = 0; i < nslice; i++)
            {
                sh[i].delta_pic_order_cnt[1] = (j == -1) ? 0 : sh[j].delta_pic_order_cnt[1];
                sh[i].len += FindCodeLen(SE, sh[i].delta_pic_order_cnt[1]); 
            }
        }
        else
        {
            for (i = 0; i < nslice; i++)
            {
                sh[i].delta_pic_order_cnt[1] = 0;    
            }        
        }    
    }
    else
    {
        for (i = 0; i < nslice; i++)
        {
            sh[i].delta_pic_order_cnt[0] = 0;
            sh[i].delta_pic_order_cnt[1] = 0;    
        }
    }
    
    /* Redundant pic count */
    if (pps->redundant_pic_cnt_present_flag)
    {
        for (i = 0; i < nslice; i++)
        {
            sh[i].redundant_pic_cnt = (j == -1) ? sh[j].redundant_pic_cnt : 0;
            sh[i].len += FindCodeLen(UE, sh[i].redundant_pic_cnt);
        }
    }
    else
    {
        for (i = 0; i < nslice; i++)
        {
            sh[i].redundant_pic_cnt = 0;
        }
    }

    /* No num_ref_idx in IDR*/
    
    /* No ref_pic_list_reordering in IDR*/

    /* Dec_ref_pic_marking */
    if (j != -1)
    {
        for (i = 0; i < nslice; i++)
        {
            sh[i].no_output_of_prior_pics_flag = sh[j].no_output_of_prior_pics_flag;
            sh[i].long_term_reference_flag = sh[j].long_term_reference_flag;
            sh[i].len += 2;
        }
    }
    else
    {
        for (i = 0; i < nslice; i++)
        {
            sh[i].no_output_of_prior_pics_flag = 0;
            sh[i].long_term_reference_flag = 0;
            sh[i].len += 2;                
        }
    }
        
    /* Slice QP delta */
    conc_val = (j != -1) ? sh[j].slice_qp_delta : 0;
    
    for (i = 0; i < nslice; i++)
    {
        if (sh[i].error)
        {
            sh[i].slice_qp_delta = conc_val;
        }
        
        sh[i].slice_qp = 26 + pps->pic_init_qp_minus26 + sh[i].slice_qp_delta;
        
        sh[i].len += FindCodeLen(SE, sh[i].slice_qp_delta);
    }
    
    /* Deblocking control */
    if (pps->debl_flt_control_present_flag)
    {
        conc_val = (j != -1) ? sh[j].disable_deblocking_filter_idc : 1; 
        
        for (i = 0; i < nslice; i++)
        {
            sh[i].disable_deblocking_filter_idc = conc_val;
            sh[i].len += FindCodeLen(UE, conc_val);
        }
        
        if (conc_val != 1)
        {
            for (i = 0; i < nslice; i++)
            {
                sh[i].slice_alpha_c0_offset_div2 = (j != -1) ? sh[j].slice_alpha_c0_offset_div2 : 0;
                sh[i].slice_beta_offset_div2 = (j != -1) ? sh[j].slice_beta_offset_div2 : 0;
                sh[i].len += FindCodeLen(SE, sh[i].slice_alpha_c0_offset_div2);
                sh[i].len += FindCodeLen(SE, sh[i].slice_beta_offset_div2);
            }
        }
    }
    else
    {
        for (i = 0; i < nslice; i++)
        {
            sh[i].disable_deblocking_filter_idc = 0;
            sh[i].slice_alpha_c0_offset_div2 = 0;
            sh[i].slice_beta_offset_div2 = 0;
        }
    } 
    
    /* slice_group_change_cycle */
    if ((pps->num_slice_groups_minus1 != 0) && (pps->slice_group_map_type >= 3) && (pps->slice_group_map_type <= 5))
    {
        t_uint16 bits;
        t_uint32 value;
        
        /* Determine ceil(log2(PicSizeInMapUnits/SliceGroupChangeRate + 1)) */
        bits = 15;
        value = ((sp->pic_width_in_mbs_minus1+1) * (sp->pic_height_in_map_units_minus1+1) / (pps->slice_group_change_rate_minus1+1)) + 1;
        
        if ((((sp->pic_width_in_mbs_minus1+1) * (sp->pic_height_in_map_units_minus1+1)) % (pps->slice_group_change_rate_minus1+1)))
            value++;
            
        while (((value>>bits)&0x1)==0)
            bits--;
        if ((value&((1<<bits)-1))!=0)
            bits+=1;
        
        conc_val = (j != -1) ? sh[j].slice_group_change_cycle : 1;    
                
        for (i = 0; i < nslice; i++)
        {
            sh[i].slice_group_change_cycle = conc_val;
            sh[i].len += bits;
        }
    }
    
    /* Adjust bitstream pointer */
    for (i = 0; i < nslice; i++)
    {
        sh[i].p_bsh_end = sh[i].p_bsh_start;
        
        if (sh[i].error < 2)
        {
            sh[i].p_bsh_end.addr += (sh[i].len >> 3) + 1; 
            sh[i].p_bsh_end.os = 8 - (sh[i].len & 7);
            //sh[i].header_length_in_bits = sh[i].len;
        }
    }
        
    return nslice;
}



/**
 * \brief Find length of ExpGolomb code for a value  
 * \param type Type of code, UE or SE 
 * \param value Input value 
 * \return Number of bits of ExpGolomb code for value
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>01-08-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function find length of ExpGolomb code for a value.    
 */
 
t_uint16 FindCodeLen(t_uint16 type, t_sint32 value)
{
    t_uint16 i = 0;
    t_uint32 codeNum;
    
    if (type == SE)
    {
        codeNum = (ABS(value)*2) - ((value > 0) ? 1 : 0);
    }
    else
    {
        codeNum = value; 
    }
    
    codeNum = (codeNum + 1) / 2;
    
    while (codeNum)
    {
        i++;
        codeNum >>= 1;
    }
    
    return (i*2+1);
}

